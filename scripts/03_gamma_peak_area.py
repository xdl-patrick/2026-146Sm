#!/usr/bin/env python3
"""03 — HPGe 谱峰面积与计数率。

替代的旧脚本
------------
* `AnalyzeSPEandROOTFiles.C` —— 747 keV 峰的线性本底积分（每复制一个新峰就多一份代码）
* `runFullAnalysis.C`        —— 630 + 747 keV 两套几乎逐行重复的积分函数
* `GammaPeakAnalysis_Full_10.18-10.28.txt` / `GammaPeakAnalysis_Full_11.18-12.02.txt`
  —— 手写的结果表，现在自动生成

输出
----
* `data/results/gamma_peak_areas.csv` —— 每个文件、每个峰的净计数/误差/计数率
* `figures/gamma/` —— 峰的窗口示意与计数率时间序列

例子
----
    python scripts/03_gamma_peak_area.py
    python scripts/03_gamma_peak_area.py --source root     # 用 01 生成的 ROOT 加速
    python scripts/03_gamma_peak_area.py --limit 5
"""

import argparse
import csv
import sys
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import plotting, rootio, spectrum
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="HPGe 峰面积与计数率")
    ap.add_argument("--config", default=None)
    ap.add_argument("--source", choices=("spe", "root"), default="spe",
                    help="从原始 .Spe 读取（默认，含死时间）或从 01 生成的 .root 读取（更快）")
    ap.add_argument("--input-dir", default=None)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--peak", default=None, help="只分析某个峰（配置中的键，如 747keV）")
    ap.add_argument("--bin-width", type=float, default=0.5, help="能量重分箱宽度 (keV)")
    add_verbosity_argument(ap)
    return ap.parse_args()


def peaks_from_config(cfg, only: Optional[str] = None) -> Dict[str, Dict]:
    peaks = cfg.section("gamma.peaks")
    if only:
        if only not in peaks:
            raise KeyError(f"配置中没有峰 {only!r}，可选: {list(peaks)}")
        peaks = {only: peaks[only]}
    return {name: {
        "window": (float(v["window"][0]), float(v["window"][1])),
        "bg_width": float(v.get("bg_width", 4.0)),
        "bg_gap": float(v.get("bg_gap", 2.0)),
    } for name, v in peaks.items()}


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()

    slope = float(cfg.require("gamma.energy_calibration.slope_kev_per_channel"))
    intercept = float(cfg.require("gamma.energy_calibration.intercept_kev"))
    peaks = peaks_from_config(cfg, args.peak)

    spe_dir = Path(args.input_dir) if args.input_dir else cfg.path("paths.raw_gamma_eu146")
    root_dir = cfg.path("paths.processed") / "gamma_eu146_2025_root"
    results_dir = cfgmod.ensure_dir(cfg.path("paths.results"))

    banner("03 HPGe 峰面积")
    log.info("数据来源: %s (%s)", spe_dir if args.source == "spe" else root_dir, args.source)
    log.info("积分峰: %s", ", ".join(f"{k} {v['window']}" for k, v in peaks.items()))

    patterns = ["*.Spe"] if args.source == "spe" else ["*.root"]
    files: List[Path] = []
    for pat in patterns:
        files.extend(sorted((spe_dir if args.source == "spe" else root_dir).glob(pat)))
    if args.limit:
        files = files[:args.limit]
    if not files:
        log.error("没有找到输入文件")
        return 1
    log.info("待处理: %d 个文件", len(files))

    rows: List[Dict] = []
    n_failed = 0
    for i, path in enumerate(files, start=1):
        try:
            row = analyze_one(path, args.source, slope, intercept, peaks, args.bin_width, cfg)
        except Exception as exc:  # noqa: BLE001
            log.error("处理失败 %s: %s", path.name, exc)
            n_failed += 1
            continue
        if row is None:
            n_failed += 1
            continue
        rows.append(row)
        if i % 50 == 0 or i == len(files):
            log.info("进度 %d/%d ...", i, len(files))

    if not rows:
        log.error("没有任何文件分析成功")
        return 1

    csv_path = results_dir / "gamma_peak_areas.csv"
    fields = list(rows[0].keys())
    with csv_path.open("w", newline="", encoding="utf-8-sig") as fh:
        writer = csv.DictWriter(fh, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)
    log.info("峰面积表 -> %s (%d 行)", csv_path.relative_to(cfgmod.PROJECT_ROOT), len(rows))
    if n_failed:
        log.warning("%d 个文件被跳过", n_failed)

    _plot_rate_series(cfg, rows, peaks)
    _plot_example(cfg, rows, peaks)
    return 0


# --------------------------------------------------------------------------- 单个文件

def analyze_one(path: Path, source: str, slope: float, intercept: float,
                peaks: Dict[str, Dict], bin_width: float, cfg) -> Optional[Dict]:
    """积分一个谱的所有峰，返回一行结果。"""
    acquired_at, live_time, counts, centers = _load(path, source, slope, intercept, bin_width)
    if counts is None or counts.size == 0:
        return None
    if live_time is None or live_time <= 0:
        log.warning("%s: 缺失活时间，该文件的计数率将无法计算", path.name)
        live_time = float("nan")

    row: Dict = {
        "file": path.name,
        "acquired_at": acquired_at.strftime("%Y-%m-%d %H:%M:%S") if acquired_at else "",
        "live_time_s": f"{live_time:.1f}" if np.isfinite(live_time) else "",
    }
    for name, spec in peaks.items():
        area = spectrum.peak_area_linear_bg(centers, counts, spec["window"],
                                            spec["bg_width"], spec["bg_gap"], live_time)
        row[f"{name}_net"] = f"{area.net_counts:.2f}"
        row[f"{name}_err"] = f"{area.net_error:.2f}"
        row[f"{name}_gross"] = f"{area.gross_counts:.1f}"
        row[f"{name}_bg"] = f"{area.background_counts:.1f}"
        row[f"{name}_rate"] = f"{area.count_rate:.6f}" if np.isfinite(area.count_rate) else ""
        row[f"{name}_rate_err"] = f"{area.count_rate_error:.6f}" if np.isfinite(area.count_rate_error) else ""
    return row


def _load(path: Path, source: str, slope: float, intercept: float, bin_width: float):
    """返回 `(测量时间, 活时间, counts, 能量 bin 中心)`。"""
    if source == "spe":
        try:
            spec = spectrum.read_spe(path)
        except Exception as exc:  # noqa: BLE001
            log.error("%s: 解析失败 %s", path.name, exc)
            return None, None, None, None
        counts, edges = spec.calibrated_hist(slope, intercept, bin_width)
        centers = 0.5 * (edges[:-1] + edges[1:])
        return spec.acquired_at, spec.live_time, counts, centers

    with rootio.open_root(path) as f:
        got = rootio.read_hist(f, "h")
        if got is None:
            got = rootio.read_hist(f, "spectrum")
        if got is None:
            log.error("%s: 没有 'h' 或 'spectrum' 直方图", path.name)
            return None, None, None, None
        counts, edges = got
    centers = 0.5 * (edges[:-1] + edges[1:])
    acquired_at = _date_from_name(path.name)
    live_time = float("nan")  # ROOT 里没存活时间，需要时请用 --source spe
    return acquired_at, live_time, counts, centers


def _date_from_name(name: str) -> Optional[datetime]:
    """从 `10.18_1_with Pb.root` 这类文件名推断日期（缺年份，默认 2025）。"""
    import re
    m = re.match(r"^(\d{1,2})\.(\d{1,2})", name)
    if not m:
        return None
    month, day = int(m.group(1)), int(m.group(2))
    try:
        return datetime(2025, month, day)
    except ValueError:
        return None


# --------------------------------------------------------------------------- 绘图

def _series(rows: List[Dict], peak: str):
    pts = []
    for r in rows:
        if not r.get("acquired_at") or not r.get(f"{peak}_rate"):
            continue
        pts.append((datetime.strptime(r["acquired_at"], "%Y-%m-%d %H:%M:%S"),
                    float(r[f"{peak}_rate"]), float(r[f"{peak}_rate_err"] or 0.0)))
    pts.sort(key=lambda t: t[0])
    return pts


def _plot_rate_series(cfg, rows: List[Dict], peaks: Dict[str, Dict]) -> None:
    fig, axes = plotting.new_figure(width=11, height=4.4, nrows=1, ncols=len(peaks))
    axes = np.atleast_1d(axes)
    for ax, name in zip(axes, peaks):
        pts = _series(rows, name)
        if not pts:
            ax.set_title(f"{name}（无有效数据）")
            continue
        t = [p[0] for p in pts]
        y = np.array([p[1] for p in pts])
        e = np.array([p[2] for p in pts])
        ax.errorbar(t, y, yerr=e, fmt="o", ms=3, lw=1, capsize=2, color="tab:blue")
        ax.set_title(f"{name} 峰计数率")
        ax.set_ylabel("计数率 (cps)")
        ax.set_yscale("log")
        ax.tick_params(axis="x", rotation=30)
    fig.tight_layout()
    plotting.save_figure(fig, cfg, "gamma", "gamma_eu146_rate_series")


def _plot_example(cfg, rows: List[Dict], peaks: Dict[str, Dict]) -> None:
    """用第一个文件做一张积分窗口示意图（含本底区）。"""
    first = rows[0]
    src = cfg.path("paths.raw_gamma_eu146") / first["file"]
    if not src.is_file():
        src = cfg.path("paths.processed") / "gamma_eu146_2025_root" / Path(first["file"]).with_suffix(".root")
    slope = float(cfg.require("gamma.energy_calibration.slope_kev_per_channel"))
    intercept = float(cfg.require("gamma.energy_calibration.intercept_kev"))
    acquired, live, counts, centers = _load(src, "spe" if src.suffix == ".Spe" else "root",
                                            slope, intercept, 0.5)
    if counts is None:
        return
    lo = min(s["window"][0] for s in peaks.values()) - 30
    hi = max(s["window"][1] for s in peaks.values()) + 30
    mask = (centers >= lo) & (centers <= hi)

    fig, ax = plotting.new_figure(width=9, height=5)
    ax.step(centers[mask], counts[mask], where="mid", lw=1, color="black")
    colors = ["tab:red", "tab:green", "tab:orange"]
    for (name, spec), color in zip(peaks.items(), colors):
        w0, w1 = spec["window"]
        ax.axvspan(w0, w1, color=color, alpha=0.25, label=f"{name} 峰区")
        for bg_lo, bg_hi in ((w0 - spec["bg_gap"] - spec["bg_width"], w0 - spec["bg_gap"]),
                             (w1 + spec["bg_gap"], w1 + spec["bg_gap"] + spec["bg_width"])):
            ax.axvspan(bg_lo, bg_hi, color=color, alpha=0.08)
    ax.set_yscale("log")
    ax.set_xlabel("能量 (keV)")
    ax.set_ylabel("计数")
    ax.set_title(f"HPGe 谱与峰积分窗口 —— {first['file']}")
    ax.legend()
    fig.tight_layout()
    plotting.save_figure(fig, cfg, "gamma", "gamma_eu146_peak_windows")


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""02 — 双面硅条 α 探测器离线分析（逐通道求和谱 + 可选逐通道刻度）。

替代的旧脚本
------------
* `Offline_146Sm.C`, `Offline_146Sm2025.C`, `Offline_146Sm2025_12.C`,
  `Offline_146Sm2025_2.C`, `Offline_146Sm2026_01.C` ... `Offline_146Sm2026_0602.C`
  —— **11 个几乎相同的副本**，每个 200~500 行，只是刻度系数/阈值不同。
* `Cal12.C`, `Cal13.C`, `Cal14.C`, `Cal15.C`, `Cal146.C`, `Cal147.C`, `Cal147_01.C`,
  `Cal2026.C` —— 8 个逐通道刻度宏，其中 6 个用 128 段 `if(num==N)` 手写展开。
* `am241_calibration.C`, `SmEu_improved_calibration.C`, `CalculateCalibrationCoefficients.C`

物理逻辑（与旧脚本一致）
------------------------
每个 run 文件里 4 块双面硅条探测器对应 4 个 ADC 模块，每模块 32 道：
0-15 为正面（front）、16-31 为背面（back）。逐事件：

1. 对每个通道做 `E = slope * ADC + intercept`；
2. 只保留 `ADC > 阈值`（逐道阈值，见 `config/alpha_thresholds_*.json`）；
3. 每块探测器的每个面把**所有条的能量求和** -> E1f/E1b/.../E4b；
4. **多重性筛选**：某面有 >= 2 条同时触发时该面整道丢弃（多重性上限见配置）；
5. 填充 8 个一维谱 + 4 个 front-vs-back 二维谱。

用法
----
    python scripts/02_alpha_offline.py                       # 处理配置目录下全部 2026 run
    python scripts/02_alpha_offline.py --runs 71,72,75       # 只处理指定 run
    python scripts/02_alpha_offline.py --limit 5             # 快速自检
    python scripts/02_alpha_offline.py --calibrate           # 逐通道刻度（写回 config/）
"""

import argparse
import csv
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import fitting, plotting, rootio
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()

SIDES = ("f", "b")                       # front / back
SPECTRUM_NAMES: List[str] = [f"E{det + 1}{side}" for det in range(4) for side in SIDES]


# --------------------------------------------------------------------------- 配置

def load_alpha_config(cfg) -> Dict:
    """把刻度系数、阈值、多重性等参数一次性读齐。"""
    calib_name = cfg.require("alpha_offline.calibration_json")
    calib = cfgmod.load_json(calib_name)
    thresh_path = cfg.get("alpha_offline.thresholds_json", "alpha_thresholds_2026.json")
    try:
        thresh = cfgmod.load_json(thresh_path)
    except FileNotFoundError:
        log.warning("找不到 %s，使用配置中的默认阈值", thresh_path)
        thresh = {"default": cfg.get("alpha_offline.default_threshold_adc", 180),
                  "threshold": None}

    n_adc = int(cfg.get("alpha_offline.adc_modules", 4))
    n_ch = int(cfg.get("alpha_offline.channels_per_module", 32))
    return {
        "n_adc": n_adc,
        "n_ch": n_ch,
        "split": int(cfg.get("alpha_offline.side_split", 16)),
        "slope": np.asarray(calib["slope"], dtype=float),
        "intercept": np.asarray(calib["intercept"], dtype=float),
        "threshold": (np.asarray(thresh["threshold"], dtype=float)
                      if thresh.get("threshold") else np.full((n_adc, n_ch), thresh["default"], float)),
        "multiplicity_max": int(cfg.get("alpha_offline.multiplicity_max", 1)),
        "bins": int(cfg.get("alpha_offline.hist.bins", 400)),
        "emin": float(cfg.get("alpha_offline.hist.min", 0.5)),
        "emax": float(cfg.get("alpha_offline.hist.max", 8.0)),
        "tree": cfg.get("alpha_offline.tree", "Tree1"),
    }


def channel_names(n_adc: int, n_ch: int) -> List[str]:
    return [f"adc{a}ch{c}" for a in range(n_adc) for c in range(n_ch)]


# --------------------------------------------------------------------------- 核心

def build_spectra(adc_arrays: Dict[str, np.ndarray], acfg: Dict) -> Tuple[Dict[str, np.ndarray], Dict[str, np.ndarray]]:
    """由逐通道 ADC 数组计算 8 个能量数组（未被任何面丢弃的事件取 0）。

    返回 `(spectra, multiplicities)`，两者都是以 `E1f ... E4b` 为键的字典。
    """
    n_adc, n_ch, split = acfg["n_adc"], acfg["n_ch"], acfg["split"]
    slope, intercept, thresh = acfg["slope"], acfg["intercept"], acfg["threshold"]
    mult_max = acfg["multiplicity_max"]

    spectra: Dict[str, np.ndarray] = {}
    multiplicities: Dict[str, np.ndarray] = {}

    for adc in range(n_adc):
        for side_idx, side in enumerate(SIDES):
            lo = side_idx * split
            hi = lo + split
            energy = np.zeros_like(next(iter(adc_arrays.values())), dtype=float)
            multi = np.zeros_like(energy, dtype=np.int16)
            for ch in range(lo, hi):
                raw = adc_arrays[f"adc{adc}ch{ch}"].astype(float)
                hit = raw > thresh[adc, ch]
                if not hit.any():
                    continue
                # 未刻度的通道 slope==0 会被这里自动屏蔽
                energy += np.where(hit, raw * slope[adc, ch] + intercept[adc, ch], 0.0)
                multi += hit.astype(np.int16)
            # 多重性筛选：超过上限的面整道丢弃
            dropped = multi > mult_max
            energy = np.where(dropped, 0.0, energy)
            key = f"E{adc + 1}{side}"
            spectra[key] = energy
            multiplicities[key] = multi
    return spectra, multiplicities


def histogram_spectra(spectra: Dict[str, np.ndarray], acfg: Dict) -> Dict[str, Tuple[np.ndarray, np.ndarray]]:
    """把 8 个能量数组填成直方图。"""
    edges = np.linspace(acfg["emin"], acfg["emax"], acfg["bins"] + 1)
    hist: Dict[str, Tuple[np.ndarray, np.ndarray]] = {}
    for name, energy in spectra.items():
        good = (energy > 0) & (energy < acfg["emax"])
        counts, _ = np.histogram(energy[good], bins=edges)
        hist[name] = (counts.astype(float), edges)
    return hist


def count_in_window(edges: np.ndarray, counts: np.ndarray, lo: float, hi: float) -> float:
    centers = 0.5 * (edges[:-1] + edges[1:])
    return float(counts[(centers >= lo) & (centers <= hi)].sum())


# --------------------------------------------------------------------------- 主流程

def process_run(run_path: Path, acfg: Dict, cfg, out_dir: Path,
                run_id: Optional[int] = None, source: str = "") -> Optional[Dict]:
    """处理单个 run，返回汇总记录（失败返回 None）。"""
    rid = run_id if run_id is not None else rootio.run_number(run_path)
    label = f"{source}/{rid}" if source else str(rid)
    try:
        with rootio.open_root(run_path) as f:
            present = rootio.tree_branches(f, acfg["tree"])
            if not present:
                # 通常是采集失败留下的空文件（几百字节），不是需要报错的异常
                log.warning("run %s: 文件里没有 %s（大小 %.0f B），判为空 run，跳过",
                            label, acfg["tree"], run_path.stat().st_size)
                return None
            if all(b.startswith("adc4ch") for b in present):
                # 这类 run 只有 adc4ch12-15，是 241Am α 刻度测量，不是 8 块靶的数据。
                # 对应代码在 IMPSiSqTest/（cbFit.cc 等），不属于本流程。
                log.info("run %s: 只有 adc4ch* 通道，判为 241Am α 刻度测量，跳过", label)
                return None
            arrays = rootio.read_tree(f, acfg["tree"], branch_names(acfg))
    except Exception as exc:  # noqa: BLE001 - 坏文件跳过
        log.error("run %s: 读取失败 (%s)", label, exc)
        return None

    n_entries = len(next(iter(arrays.values()))) if arrays else 0
    spectra, mults = build_spectra(arrays, acfg)
    hists = histogram_spectra(spectra, acfg)

    stem = f"{source}_run{rid}" if source else f"run{rid}"
    out_path = out_dir / f"{stem}_spectra.root"
    rootio.write_hists(out_path, hists)

    record = {"source": source, "run": rid, "file": run_path.name, "entries": n_entries}
    for name in SPECTRUM_NAMES:
        counts, edges = hists[name]
        record[f"{name}_counts"] = int(counts.sum())
        for peak_name, (lo, hi) in alpha_peak_windows(cfg).items():
            record[f"{name}_{peak_name}"] = count_in_window(edges, counts, lo, hi)
    record["_spectra"] = spectra
    record["_hists"] = hists
    record["_stem"] = stem
    return record


def branch_names(acfg: Dict) -> List[str]:
    return channel_names(acfg["n_adc"], acfg["n_ch"])


def alpha_peak_windows(cfg) -> Dict[str, Tuple[float, float]]:
    """α 特征峰窗口（MeV），来自配置。忽略 `_comment` 之类的说明项。"""
    raw = cfg.get("alpha_peaks", {}) or {}
    return {name: (float(v["min"]), float(v["max"]))
            for name, v in raw.items()
            if not name.startswith("_") and isinstance(v, dict)}


def alpha_sources(cfg, only: Optional[str] = None, include_disabled: bool = False) -> List[Dict]:
    """读配置里的多代数据源；`only` 指定时只保留该源（或含该源名）。"""
    raw = cfg.get("alpha_offline.sources", None)
    if not raw:
        raw = [{"name": "alpha_2026", "dir_key": "paths.raw_alpha_2026",
                "pattern": cfg.get("alpha_offline.file_pattern", r"^2026_146Sm(\d+)$"),
                "enabled": True}]
    out = []
    for item in raw:
        if item.get("name", "").startswith("_"):
            continue
        if not item.get("enabled", True) and not (only or include_disabled):
            continue
        if only and only not in item.get("name", ""):
            continue
        out.append({
            "name": item.get("name", "alpha"),
            "dir": cfg.path(item["dir_key"]),
            "pattern": item.get("pattern", r"(\d+)$"),
        })
    return out


def run_offline(cfg, args) -> int:
    acfg = load_alpha_config(cfg)
    if args.input_dir:
        sources = [{"name": "custom", "dir": Path(args.input_dir),
                    "pattern": cfg.get("alpha_offline.file_pattern", r"^2026_146Sm(\d+)$")}]
    else:
        sources = alpha_sources(cfg, args.source, include_disabled=args.all_sources)
    if not sources:
        log.error("没有任何可用的数据源（检查 alpha_offline.sources 或 --source）")
        return 1

    out_dir = cfgmod.ensure_dir(cfg.path("paths.processed") / "alpha_spectra")
    results_dir = cfgmod.ensure_dir(cfg.path("paths.results"))

    banner("02 α 离线分析")
    log.info("多重性上限: %d（超过则该面整体丢弃）", acfg["multiplicity_max"])
    for src in sources:
        log.info("数据源 %-12s %s", src["name"], src["dir"])

    records: List[Dict] = []
    for src in sources:
        files = rootio.find_run_files(src["dir"], src["pattern"],
                                      max_files=cfg.get("alpha_offline.max_files", 0))
        if args.runs:
            wanted = {int(x) for x in args.runs.split(",") if x.strip()}
            files = [p for p in files if rootio.run_number(p) in wanted]
        if args.limit:
            files = files[:args.limit]
        if not files:
            log.warning("数据源 %s 没有匹配的 run 文件，跳过", src["name"])
            continue
        log.info("数据源 %s: 待处理 %d 个 run", src["name"], len(files))
        for path in files:
            rec = process_run(path, acfg, cfg, out_dir, source=src["name"])
            if rec:
                records.append(rec)

    if not records:
        log.error("所有 run 都处理失败")
        return 1

    # 汇总表：去掉内部字段
    csv_fields = [k for k in records[0] if not k.startswith("_")]
    csv_path = results_dir / "alpha_run_summary.csv"
    with csv_path.open("w", newline="", encoding="utf-8-sig") as fh:
        writer = csv.DictWriter(fh, fieldnames=csv_fields, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(records)
    log.info("run 汇总 -> %s (%d 行，来自 %d 个数据源)",
             csv_path.relative_to(cfgmod.PROJECT_ROOT), len(records), len(sources))
    if args.limit or args.runs or args.source or args.input_dir:
        log.warning("本次使用了 --limit/--runs/--source/--input-dir，汇总表**只包含本次处理的 run**；"
                    "需要完整表格请不带这些参数再跑一次。")

    _plot_run(cfg, records[0], args)
    _plot_run_overview(cfg, records)
    return 0


def _plot_run(cfg, record: Dict, args) -> None:
    """8 面板能量谱图。"""
    from lib import plotting as P
    hists = record["_hists"]
    fig, axes = P.new_figure(width=13, height=7, nrows=2, ncols=4)
    for ax, name in zip(axes.ravel(), SPECTRUM_NAMES):
        counts, edges = hists[name]
        centers = 0.5 * (edges[:-1] + edges[1:])
        ax.step(centers, counts, where="mid", lw=1, color="tab:blue")
        if counts.max() > 0:
            ax.set_yscale("log")
        else:
            ax.text(0.5, 0.5, "无计数", transform=ax.transAxes, ha="center", va="center",
                    color="0.5", fontsize=11)
        ax.set_title(f"{name}  (总计数 {int(counts.sum())})")
        for peak_name, (lo, hi) in alpha_peak_windows(cfg).items():
            ax.axvspan(lo, hi, color="tab:orange", alpha=0.18)
        ax.set_xlabel("能量 (MeV)")
        ax.set_ylabel("计数")
    fig.suptitle(f"Run {record['_stem']} —— 4 块双面硅条探测器求和谱（多重性 <= "
                 f"{cfg.get('alpha_offline.multiplicity_max', 1)}）", fontsize=13)
    fig.tight_layout(rect=(0, 0, 1, 0.96))
    plotting.save_figure(fig, cfg, "alpha", f"{record['_stem']}_spectra")


def _plot_run_overview(cfg, records) -> None:
    """各 run 的 E3f 谱叠加，用于观察靶间差异。"""
    want = cfg.get("alpha_offline.overview_spectrum", "E3f")
    fig, ax = plotting.new_figure(width=8.5, height=5)
    plotted = 0
    for rec in records[:20]:
        counts, edges = rec["_hists"][want]
        if counts.max() <= 0:
            continue          # 该 run 这个面没有计数，跳过，否则整轴无法取对数
        centers = 0.5 * (edges[:-1] + edges[1:])
        ax.step(centers, counts, where="mid", lw=1, alpha=0.75, label=f"run {rec['run']}")
        plotted += 1
    if plotted == 0:
        log.info("%s 在所有 run 中都没有计数，跳过对比图", want)
        import matplotlib.pyplot as plt
        plt.close(fig)
        return
    ax.set_yscale("log")
    ax.set_xlabel("能量 (MeV)")
    ax.set_ylabel("计数")
    ax.set_title(f"{want} 各 run 对比（最多显示 20 个）")
    ax.legend(ncol=2)
    plotting.save_figure(fig, cfg, "alpha", f"alpha_runs_overview_{want.lower()}")


# --------------------------------------------------------------------------- 逐通道刻度

def run_calibration(cfg, args) -> int:
    """逐通道刻度：对指定 run 的每个通道找峰 -> 高斯拟合 -> 线性刻度。

    需要至少两个已知能量的参考测量（例如 241Am 5.4428 MeV 与 147Sm 2.2233 MeV），
    用两点确定 `E = a·ADC + b`。参考点写在 `config/alpha_calibration_points.json`。
    """
    acfg = load_alpha_config(cfg)
    points_file = cfg.get("alpha_offline.calibration_points", "alpha_calibration_points.json")
    try:
        points = cfgmod.load_json(points_file)
    except FileNotFoundError:
        log.error("找不到参考点文件 config/%s，无法刻度。", points_file)
        log.error("格式示例：{\"points\": [{\"file\": \"...root\", \"energy_mev\": 5.4428}, ...]}")
        return 1

    banner("02b 逐通道能量刻度")
    n_adc, n_ch = acfg["n_adc"], acfg["n_ch"]
    window = args.fit_window or (500, 6000)
    results = {}

    for adc in range(n_adc):
        for ch in range(n_ch):
            centers, energies = [], []
            for point in points["points"]:
                path = Path(point["file"])
                if not path.is_absolute():
                    path = cfgmod.PROJECT_ROOT / path
                peak = _fit_channel_peak(path, f"adc{adc}ch{ch}", acfg["tree"], window)
                if peak is not None:
                    centers.append(peak)
                    energies.append(float(point["energy_mev"]))
                if len(centers) >= 2:
                    break
            if len(centers) >= 2:
                res = fitting.fit_linear(centers, energies)
                results[f"{adc},{ch}"] = {
                    "slope": res.slope, "intercept": res.intercept,
                    "slope_error": res.slope_error, "intercept_error": res.intercept_error,
                    "n_points": len(centers),
                }
            else:
                results[f"{adc},{ch}"] = {"slope": 0.0, "intercept": 0.0, "n_points": len(centers)}

    slope = [[results[f"{a},{c}"]["slope"] for c in range(n_ch)] for a in range(n_adc)]
    intercept = [[results[f"{a},{c}"]["intercept"] for c in range(n_ch)] for a in range(n_adc)]
    out = cfgmod.save_json("alpha_calibration_recalibrated.json", {
        "_comment": "由 scripts/02_alpha_offline.py --calibrate 生成；slope 为 0 表示该道刻度失败",
        "n_adc": n_adc, "n_ch": n_ch,
        "slope": slope, "intercept": intercept,
    })
    fitted = sum(1 for v in results.values() if v["n_points"] >= 2)
    log.info("刻度完成: %d/%d 道成功 -> %s", fitted, n_adc * n_ch, out.name)
    return 0 if fitted else 1


def _fit_channel_peak(path: Path, branch: str, tree: str, window: Tuple[int, int]) -> Optional[float]:
    """对单通道 ADC 谱在 window 道范围内做高斯拟合，返回峰位（道数）。"""
    try:
        with rootio.open_root(path) as f:
            try:
                obj = f[branch]
                counts = np.asarray(obj.values(), dtype=float)
                edges = np.asarray(obj.axis().edges(), dtype=float)
                centers = 0.5 * (edges[:-1] + edges[1:])
            except KeyError:
                arrays = rootio.read_tree(f, tree, [branch])
                counts, edges = np.histogram(arrays[branch], bins=4096)
                centers = 0.5 * (edges[:-1] + edges[1:])
    except Exception as exc:  # noqa: BLE001
        log.debug("%s/%s 读取失败: %s", path.name, branch, exc)
        return None

    mask = (centers >= window[0]) & (centers <= window[1])
    if mask.sum() < 10:
        return None
    x, y = centers[mask], counts[mask]
    res = fitting.fit_gaussian(x, y, p0=[y.max(), x[int(np.argmax(y))], (window[1] - window[0]) / 20.0, 0.0])
    if not res.ok or res.params[2] <= 0:
        return None
    return float(res.params[1])


# --------------------------------------------------------------------------- CLI

def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="双面硅条 α 探测器离线分析")
    ap.add_argument("--config", default=None)
    ap.add_argument("--input-dir", default=None)
    ap.add_argument("--runs", default="", help="只处理这些 run，逗号分隔，例如 71,72,75")
    ap.add_argument("--source", default=None,
                    help="只处理某个数据源（alpha_2024 / alpha_2025 / alpha_2026）")
    ap.add_argument("--all-sources", action="store_true",
                    help="处理配置里所有数据源（含 enabled=false 的 2024/2025）")
    ap.add_argument("--limit", type=int, default=0, help="最多处理多少个 run")
    ap.add_argument("--calibrate", action="store_true", help="改为逐通道刻度模式")
    ap.add_argument("--fit-window", type=int, nargs=2, default=None, help="刻度时的拟合道范围")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()
    return run_calibration(cfg, args) if args.calibrate else run_offline(cfg, args)


if __name__ == "__main__":
    raise SystemExit(main())

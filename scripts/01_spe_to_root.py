#!/usr/bin/env python3
"""01 — HPGe 原始谱（.Spe）转 ROOT，并导出测量元数据表。

替代的旧脚本
------------
* `SpeToRoot.C`          —— 单个文件转换，跳过 12 行硬读数
* `BatchSpeToRoot.C`     —— 手写 28 个文件名的批处理，刻度系数写死在源码里
* `BatchSpeToRoot2024.C` —— 同一功能的第二个副本

现在：扫描目录 -> 解析头部（测量时间/活时间/死时间/探测器） -> 写 ROOT -> 汇总 CSV。
刻度系数来自 `config/pipeline.json`，不再硬编码。

例子
----
    python scripts/01_spe_to_root.py                    # 转换全部
    python scripts/01_spe_to_root.py --limit 3          # 只转前 3 个（快速自检）
    python scripts/01_spe_to_root.py --force            # 覆盖已存在的输出
"""

import argparse
import csv
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import plotting, rootio, spectrum
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="SPE 原始谱 -> ROOT + 元数据表")
    ap.add_argument("--config", default=None, help="配置文件（默认 config/pipeline.json）")
    ap.add_argument("--input-dir", default=None, help="覆盖配置里的原始谱目录")
    ap.add_argument("--output-dir", default=None, help="覆盖配置里的输出目录")
    ap.add_argument("--pattern", default="*.Spe", help="输入文件通配符（默认 *.Spe）")
    ap.add_argument("--limit", type=int, default=0, help="最多处理多少个文件")
    ap.add_argument("--force", action="store_true", help="覆盖已存在的 ROOT 输出")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()

    in_dir = Path(args.input_dir) if args.input_dir else cfg.path("paths.raw_gamma_eu146")
    out_dir = Path(args.output_dir) if args.output_dir else cfg.path("paths.processed") / "gamma_eu146_2025_root"
    results_dir = cfgmod.ensure_dir(cfg.path("paths.results"))

    slope = float(cfg.require("gamma.energy_calibration.slope_kev_per_channel"))
    intercept = float(cfg.require("gamma.energy_calibration.intercept_kev"))

    banner("01 SPE -> ROOT")
    log.info("输入目录: %s", in_dir)
    log.info("输出目录: %s", out_dir)
    log.info("能量刻度: E[keV] = %.9f * ch + %.6f", slope, intercept)

    files = sorted(in_dir.glob(args.pattern))
    if args.limit:
        files = files[:args.limit]
    if not files:
        log.error("在 %s 中找不到匹配 %s 的文件", in_dir, args.pattern)
        return 1
    log.info("待处理文件: %d 个", len(files))

    out_dir.mkdir(parents=True, exist_ok=True)
    records = []
    n_failed = 0

    for i, spe_path in enumerate(files, start=1):
        target = out_dir / f"{spe_path.stem}.root"
        try:
            spec = spectrum.read_spe(spe_path)
        except Exception as exc:  # noqa: BLE001 - 单个坏文件不应中断批处理
            log.error("[%d/%d] 解析失败 %s: %s", i, len(files), spe_path.name, exc)
            n_failed += 1
            continue

        if not (target.exists() and not args.force):
            counts_kev, edges_kev = spec.calibrated_hist(slope, intercept)
            raw_edges = spec.first_channel + np.arange(spec.n_channels + 1)
            rootio.write_hists(target, {
                "spectrum": (spec.counts, raw_edges),   # 原始道谱（兼容旧脚本的直方图名）
                "h": (counts_kev, edges_kev),           # 能量刻度后（兼容旧脚本的 "h"）
            })

        records.append({
            "file": spe_path.name,
            "root_file": target.name,
            "spec_id": spec.spec_id,
            "detector": spec.detector,
            "acquired_at": spec.acquired_at.strftime("%Y-%m-%d %H:%M:%S") if spec.acquired_at else "",
            "live_time_s": f"{spec.live_time:.1f}",
            "real_time_s": f"{spec.real_time:.1f}",
            "dead_time_percent": f"{spec.dead_time_percent:.4f}",
            "n_channels": spec.n_channels,
            "total_counts": f"{spec.total_counts:.0f}",
        })

        if i % 50 == 0 or i == len(files):
            log.info("进度 %d/%d ...", i, len(files))

    if not records:
        log.error("没有任何文件转换成功")
        return 1

    csv_path = results_dir / "gamma_measurements.csv"
    with csv_path.open("w", newline="", encoding="utf-8-sig") as fh:
        writer = csv.DictWriter(fh, fieldnames=list(records[0].keys()))
        writer.writeheader()
        writer.writerows(records)

    log.info("测量元数据表 -> %s  (%d 行)", csv_path.relative_to(cfgmod.PROJECT_ROOT), len(records))
    if n_failed:
        log.warning("共 %d 个文件解析失败（已跳过）", n_failed)

    _quality_control_plots(cfg, records)
    return 0


def _quality_control_plots(cfg, records) -> None:
    """画活时间/死时间随测量日期的变化，用于发现异常测量。"""
    import matplotlib.dates as mdates
    from datetime import datetime

    good = [r for r in records if r["acquired_at"]]
    if len(good) < 3:
        log.info("有效时间戳太少，跳过 QC 图")
        return

    times = [datetime.strptime(r["acquired_at"], "%Y-%m-%d %H:%M:%S") for r in good]
    dead = [float(r["dead_time_percent"]) for r in good]
    live = [float(r["live_time_s"]) for r in good]

    fig, axes = plotting.new_figure(width=11, height=4.2, nrows=1, ncols=2)
    axes[0].plot(times, dead, "o-", ms=3, lw=1, color="tab:red")
    axes[0].set_ylabel("死时间 (%)")
    axes[0].set_title("HPGe 死时间随测量时间变化")
    axes[1].plot(times, live, "o-", ms=3, lw=1, color="tab:blue")
    axes[1].set_ylabel("活时间 (s)")
    axes[1].set_title("每次测量的活时间")
    for ax in axes:
        ax.xaxis.set_major_formatter(mdates.DateFormatter("%m-%d"))
        ax.tick_params(axis="x", rotation=30)
    fig.tight_layout()
    plotting.save_figure(fig, cfg, "gamma", "gamma_eu146_qc_deadtime")


if __name__ == "__main__":
    raise SystemExit(main())

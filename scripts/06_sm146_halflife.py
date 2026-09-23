#!/usr/bin/env python3
"""06 — 146Sm 半衰期（最终结果）。

说明（重要）
------------
**这一步在原工作流里也没有对应脚本。** 下面的实现按任务描述复原：

1. 从 `data/results/alpha_run_summary.csv`（02 步产出）取出每块靶的 α 峰区计数率；
2. 用 05 步的归一因子 `k_j` 修正靶与靶之间的几何/自吸收差异；
3. 把同一测量轮次内所有靶的结果加权平均，得到该轮次的 146Sm 活度
   $$R(t) = R_0 e^{-\\lambda_{\\rm Sm} t},\\qquad T_{1/2} = \\ln 2 / \\lambda$$
4. 对多个测量轮次做指数拟合，给出 `T1/2(146Sm)`。

物理注意事项
------------
146Sm 的参考半衰期约 1.03×10⁸ 年，**在几个月到几年的测量窗口内衰减量 < 1%**。
因此：
* 若只跨 2025–2026 一轮数据，拟合出的 λ 会在误差范围内与 0 相容，
  此时脚本会明确报告「无法在本测量窗口内测到衰变」，而不是给一个假的半衰期；
* 要真正测出半衰期，必须把 2024 年（`data/raw/alpha_2024`）的测量一并纳入，
  并且把系统误差（束流积分、几何、立体角）压到 10⁻³ 量级。

用法
----
    python scripts/06_sm146_halflife.py --level run      # 每个 run 一个点
    python scripts/06_sm146_halflife.py --level epoch    # 每个测量轮次一个点
"""

import argparse
import csv
import json
import math
import sys
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import fitting, plotting
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()

SECONDS_PER_YEAR = 365.25 * 86400.0


@dataclass
class Epoch:
    """一个测量点。"""

    label: str
    when: datetime
    rate: float
    rate_err: float
    n_targets: int


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="146Sm 半衰期")
    ap.add_argument("--config", default=None)
    ap.add_argument("--peak", default="Sm146", help="用哪个 α 峰窗口（alpha_peaks 的键）")
    ap.add_argument("--spectrum", default="E3f", help="用哪个探测器的谱（E1f..E4b）")
    ap.add_argument("--level", choices=("epoch", "run"), default="epoch",
                    help="按测量轮次或按单个 run 取点")
    ap.add_argument("--runs", default=None, help="runs.json（含 alpha_runs 的日期）")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()

    banner(f"06 146Sm 半衰期 —— {args.peak} 峰 / {args.spectrum}")
    summary_path = cfg.path("paths.results") / "alpha_run_summary.csv"
    if not summary_path.is_file():
        log.error("找不到 %s，请先运行 02_alpha_offline.py", summary_path)
        return 1

    runs_path = Path(args.runs) if args.runs else cfgmod.CONFIG_DIR / "runs.json"
    dates = load_run_dates(runs_path)
    factors = load_normalization(cfg)

    rows = list(csv.DictReader(summary_path.open(encoding="utf-8-sig")))
    key = f"{args.spectrum}_{args.peak}"
    if rows and key not in rows[0]:
        log.error("汇总表里没有列 %s，可用的列: %s",
                  key, [c for c in rows[0] if c.startswith(args.spectrum)])
        return 1

    epochs = build_epochs(rows, key, dates, factors, args.level)
    if len(epochs) < 2:
        log.error("只构造出 %d 个测量点，无法拟合。请在 runs.json 里补全 alpha_runs 的日期。",
                  len(epochs))
        return 1

    log.info("测量点: %d 个", len(epochs))
    for e in epochs:
        log.info("  %-14s %s  R = %10.4f ± %-8.4f cps  (%d 个靶)",
                 e.label, e.when.strftime("%Y-%m-%d %H:%M"), e.rate, e.rate_err, e.n_targets)

    t0 = min(e.when for e in epochs)
    t_days = np.array([(e.when - t0).total_seconds() / 86400.0 for e in epochs])
    rate = np.array([e.rate for e in epochs])
    err = np.array([e.rate_err for e in epochs])

    span_days = float(t_days.max() - t_days.min())
    log.info("时间跨度: %.1f 天 (%.3f 年)", span_days, span_days / 365.25)

    try:
        decay = fit_sm146(t_days, rate, err)
    except ValueError as exc:
        log.error("拟合失败: %s", exc)
        return 1

    report(decay, cfg, span_days)
    plot(cfg, epochs, t0, decay, args.peak)
    write_result(cfg, decay, args.peak, span_days)
    return 0


# --------------------------------------------------------------------------- 输入

def load_run_dates(path: Path) -> Dict[str, datetime]:
    """从 runs.json 读出 `run 名 -> 测量时间`。缺失时返回空表（脚本会提示）。"""
    if not path.is_file():
        log.warning("找不到 %s，无法按时间分轮次；请运行 05 步的 --make-template 生成并填写。", path)
        return {}
    with path.open(encoding="utf-8") as fh:
        runs = json.load(fh)
    out: Dict[str, datetime] = {}
    for entry in runs.get("targets", []):
        for run in entry.get("alpha_runs", []):
            name = run if isinstance(run, str) else run.get("name", "")
            when = run.get("datetime") if isinstance(run, dict) else None
            if name and when:
                out[Path(name).stem] = datetime.strptime(when, "%Y-%m-%d %H:%M:%S")
    return out


def load_normalization(cfg) -> Dict[str, float]:
    """读 05 步的归一因子 `target -> k`。没有就全用 1。"""
    path = cfg.path("paths.results") / "target_normalization.csv"
    if not path.is_file():
        log.warning("没有 target_normalization.csv，本步骤不做靶间归一（k = 1）")
        return {}
    out = {}
    with path.open(encoding="utf-8-sig") as fh:
        for row in csv.DictReader(fh):
            out[row["target"]] = float(row["normalization_k"])
    return out


def build_epochs(rows: List[Dict], key: str, dates: Dict[str, datetime],
                 factors: Dict[str, float], level: str) -> List[Epoch]:
    """把 run 级结果聚合为测量点。"""
    by_label: Dict[str, List[tuple]] = {}
    for row in rows:
        run_name = f"2026_146Sm{row['run']}" if not row.get("file") else Path(row["file"]).stem
        when = dates.get(run_name) or dates.get(str(row["run"]))
        if when is None:
            continue
        label = when.strftime("%Y-%m-%d") if level == "epoch" else run_name
        try:
            counts = float(row[key])
            entries = float(row["entries"])
        except (KeyError, ValueError):
            continue
        if entries <= 0:
            continue
        # 靶编号目前只能从 runs.json 推；无映射时按整体平均处理（k = 1）
        k = 1.0
        for tgt, factor in factors.items():
            if run_name in (tgt, str(tgt)):
                k = factor
                break
        by_label.setdefault(label, []).append((counts / entries, when, k))

    epochs = []
    for label, items in by_label.items():
        rates = np.array([r * k for r, _, k in items])
        # 泊松误差：单位应是「每事件计数率」，这里以计数平方根近似
        errs = np.sqrt(np.clip(rates, 1e-12, None))
        mean, mean_err = fitting.weighted_mean(rates, errs)
        epochs.append(Epoch(label=label, when=items[0][1], rate=mean,
                            rate_err=mean_err, n_targets=len(items)))
    epochs.sort(key=lambda e: e.when)
    return epochs


# --------------------------------------------------------------------------- 拟合

def fit_sm146(t_days: np.ndarray, rate: np.ndarray, err: np.ndarray):
    """指数拟合，返回 `(λ/年, σλ, t½/年, σt½, 是否探测到衰变)`。"""
    t_years = t_days / 365.25
    res = fitting.fit_decay(t_years, rate, err)   # 横轴为「年」，λ 单位即 /年
    lam_year = res.lam
    lam_year_err = res.lam_error
    detected = lam_year > 2.0 * lam_year_err
    if lam_year <= 0:
        return None, None, None, None, False
    half_life = math.log(2.0) / lam_year
    half_life_err = half_life * lam_year_err / lam_year
    return lam_year, lam_year_err, half_life, half_life_err, detected


def report(decay, cfg, span_days: float) -> None:
    lam, lam_err, half_life, half_life_err, detected = decay
    ref = float(cfg.get("sm146_halflife.reference_halflife_years", 1.03e8))
    ref_err = float(cfg.get("sm146_halflife.reference_halflife_error_years", 0.04e8))
    log.info("")
    log.info("拟合结果: λ = %.6e ± %.6e /年", lam, lam_err)
    log.info("          T1/2 = %.4e ± %.4e 年", half_life, half_life_err)
    log.info("参考值:   T1/2 = %.3e ± %.3e 年", ref, ref_err)
    log.info("")
    if not detected:
        log.warning("在本测量窗口（%.2f 年）内**没有探测到显著的衰变**：λ 与 0 的差别不足 2σ。", span_days / 365.25)
        log.warning("这与 146Sm 的真实半衰期（~1e8 年）完全一致——")
        log.warning("若要在本实验中真正测出 T1/2，需要把 2024 年的数据一并纳入，")
        log.warning("并把系统误差压到 %.1e 量级以下。", half_life_err / half_life if half_life_err else float("nan"))
    else:
        pull = (half_life - ref) / math.hypot(half_life_err, ref_err)
        log.info("与参考值偏差: %.2f σ", pull)


def plot(cfg, epochs: List[Epoch], t0: datetime, decay, peak: str) -> None:
    t = np.array([(e.when - t0).total_seconds() / 86400.0 for e in epochs])
    y = np.array([e.rate for e in epochs])
    e = np.array([e.rate_err for e in epochs])
    _, _, half_life, half_life_err, detected = decay

    fig, axes = plotting.new_figure(width=11, height=4.4, nrows=1, ncols=2)
    axes[0].errorbar(t, y, yerr=e, fmt="o", ms=4, capsize=3, color="tab:blue")
    if half_life and half_life > 0:
        lam = math.log(2.0) / (half_life * 365.25)
        tt = np.linspace(t.min(), t.max(), 200)
        axes[0].plot(tt, y[0] * np.exp(-lam * tt), "-", color="tab:red", lw=1.5,
                     label=f"T1/2 = {half_life:.3e} 年")
        axes[0].legend()
    axes[0].set_xlabel("相对时间 (天)")
    axes[0].set_ylabel(f"{peak} 计数率 (cps)")
    axes[0].set_title("146Sm 活度随时间变化")

    # 归一化到第一个点，便于直观看到衰变量级
    axes[1].errorbar(t, y / y[0], yerr=e / y[0], fmt="o", ms=4, capsize=3, color="tab:green")
    axes[1].axhline(1.0, color="k", lw=0.8, ls="--")
    axes[1].set_xlabel("相对时间 (天)")
    axes[1].set_ylabel("R / R0")
    axes[1].set_title("相对活度（虚线 = 无衰变）")
    fig.tight_layout()
    plotting.save_figure(fig, cfg, "halflife", f"halflife_sm146_{peak}_decay")


def write_result(cfg, decay, peak: str, span_days: float) -> None:
    lam, lam_err, half_life, half_life_err, detected = decay
    out = cfgmod.ensure_dir(cfg.path("paths.results")) / f"halflife_sm146_{peak}.csv"
    with out.open("w", newline="", encoding="utf-8-sig") as fh:
        writer = csv.writer(fh)
        writer.writerow(["peak", "span_days", "lambda_per_year", "lambda_error_per_year",
                         "halflife_years", "halflife_error_years", "decay_detected"])
        writer.writerow([peak, f"{span_days:.2f}",
                         f"{lam:.6e}", f"{lam_err:.6e}",
                         f"{half_life:.6e}" if half_life else "",
                         f"{half_life_err:.6e}" if half_life_err else "",
                         "yes" if detected else "no"])
    log.info("结果 -> %s", out.relative_to(cfgmod.PROJECT_ROOT))


if __name__ == "__main__":
    raise SystemExit(main())

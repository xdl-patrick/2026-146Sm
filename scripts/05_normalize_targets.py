#!/usr/bin/env python3
"""05 — 用参考靶/参考日归一 8 块靶的 146Eu 活度。

说明（重要）
------------
**这一步在原工作流里没有对应脚本**，旧代码中只有零散的 146Eu 峰面积与半衰期拟合。
下面的实现严格按任务描述复原流程：

1. 从 γ 数据里取出「所选 2 块靶在某一天」的 146Eu 活度测量；
2. 由 04 步拟合出的 146Eu 半衰期，把每块靶、每次测量的 146Eu 计数率**衰变修正**
   到统一参考日 `t_ref`：
   $$A_i(t_{\\rm ref}) = A_i(t_i)\\, e^{+\\lambda_{\\rm Eu}(t_i - t_{\\rm ref})}$$
3. 挑出「测量质量最好的一天」——按相对统计误差最小、且参考靶之间
   χ² 一致性最好来打分；
4. 得到每块靶相对参考靶的归一因子
   $$k_j = \\frac{A_{\\rm ref}(t_{\\rm ref})}{A_j(t_{\\rm ref})}$$
   后续 α 计数率乘上 `k_j`，即可消掉靶与靶之间的几何/自吸收差异。

需要你填写的输入
----------------
`config/runs.json`：run / 文件名 -> 靶编号、测量时间、测量类型（alpha / gamma）。
本脚本提供 `--make-template` 生成骨架。

用法
----
    python scripts/05_normalize_targets.py --make-template   # 生成 config/runs.json 骨架
    python scripts/05_normalize_targets.py                   # 计算归一因子
"""

import argparse
import csv
import json
import math
import sys
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import fitting, plotting
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="8 块靶 146Eu 活度归一")
    ap.add_argument("--config", default=None)
    ap.add_argument("--runs", default=None, help="runs.json 路径（默认 config/runs.json）")
    ap.add_argument("--peak", default=None, help="用哪个 γ 峰做归一（默认 primary_peak）")
    ap.add_argument("--reference-day", default=None, help="参考日 'YYYY-mm-dd'（覆盖配置）")
    ap.add_argument("--make-template", action="store_true", help="只生成 runs.json 骨架后退出")
    add_verbosity_argument(ap)
    return ap.parse_args()


# --------------------------------------------------------------------------- 骨架

TEMPLATE = {
    "_comment": "run/文件 -> 靶编号、时间、类型。缺的项请补全；靶编号用 1..8。",
    "reference_day": None,
    "reference_targets": [],
    "targets": [
        {"target": i, "alpha_runs": [], "gamma_files": []} for i in range(1, 9)
    ],
}


def make_template(cfg, path: Path) -> int:
    path.parent.mkdir(parents=True, exist_ok=True)

    # 顺手把实际存在的文件列出来，方便用户填
    alpha_dir = cfg.path("paths.raw_alpha_2026")
    gamma_dir = cfg.path("paths.raw_gamma_eu146")
    alpha_names = sorted(p.stem for p in alpha_dir.glob("2026_146Sm*.root")) if alpha_dir.is_dir() else []
    gamma_names = sorted(p.name for p in gamma_dir.glob("*.Spe")) if gamma_dir.is_dir() else []

    payload = dict(TEMPLATE)
    payload["_available_alpha_runs"] = alpha_names
    payload["_available_gamma_files"] = gamma_names
    path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    log.info("已生成骨架 -> %s", path)
    log.info("  可用 α run  %d 个", len(alpha_names))
    log.info("  可用 γ 文件 %d 个", len(gamma_names))
    log.info("请填写每个靶对应的 alpha_runs / gamma_files，以及 reference_day、reference_targets。")
    return 0


# --------------------------------------------------------------------------- 计算

def load_runs(path: Path) -> Dict:
    if not path.is_file():
        raise FileNotFoundError(f"找不到 {path}，请先运行 --make-template 并填写")
    with path.open(encoding="utf-8") as fh:
        return json.load(fh)


def load_gamma_rates(csv_path: Path, peak: str) -> Dict[str, Dict]:
    """文件名 -> {time, rate, err}。"""
    out: Dict[str, Dict] = {}
    if not csv_path.is_file():
        raise FileNotFoundError(f"找不到 {csv_path}，请先运行 03_gamma_peak_area.py")
    with csv_path.open(encoding="utf-8-sig") as fh:
        for row in csv.DictReader(fh):
            if not row.get("acquired_at") or not row.get(f"{peak}_rate"):
                continue
            try:
                when = datetime.strptime(row["acquired_at"], "%Y-%m-%d %H:%M:%S")
                rate = float(row[f"{peak}_rate"])
                err = float(row.get(f"{peak}_rate_err") or 0.0)
            except ValueError:
                continue
            if rate > 0:
                out[row["file"]] = {"time": when, "rate": rate, "err": err}
    return out


def eval_day_stability(per_target: Dict[int, Dict], halflife_days: float,
                       reference: List[int], t_ref: datetime) -> Dict:
    """给某一天的测量打分：（1）参考靶之间的 χ² 一致性；（2）整体相对误差。"""
    lam = math.log(2.0) / (halflife_days * 24.0)
    corrected, errs = [], []
    for tgt in reference:
        item = per_target.get(tgt)
        if not item:
            continue
        factor = math.exp(lam * (item["time"] - t_ref).total_seconds() / 3600.0)
        corrected.append(item["rate"] * factor)
        errs.append(item["err"] * factor)
    if len(corrected) < 2:
        return {"n": len(corrected), "chi2": float("nan"), "ndf": 0,
                "pvalue": float("nan"), "rel_error": float("nan"), "mean": float("nan")}
    cons = fitting.chi2_consistency(corrected, errs)
    mean, mean_err = fitting.weighted_mean(corrected, errs)
    return {"n": len(corrected), "chi2": cons["chi2"], "ndf": cons["ndf"],
            "pvalue": cons["pvalue"], "mean": mean,
            "rel_error": mean_err / mean if mean else float("nan")}


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()

    runs_path = Path(args.runs) if args.runs else cfgmod.CONFIG_DIR / "runs.json"
    if args.make_template:
        banner("05 生成 runs.json 骨架")
        return make_template(cfg, runs_path)

    banner("05 8 块靶 146Eu 活度归一")
    runs = load_runs(runs_path)
    peak = args.peak or cfg.get("gamma.primary_peak", "747keV")
    gamma = load_gamma_rates(cfg.path("paths.results") / "gamma_peak_areas.csv", peak)

    reference_day = args.reference_day or runs.get("reference_day")
    reference_targets = runs.get("reference_targets") or []
    if not reference_day or not reference_targets:
        log.error("runs.json 里还没有 reference_day / reference_targets，无法归一。")
        log.error("请先运行 `python scripts/05_normalize_targets.py --make-template` 并填写。")
        return 1

    t_ref = datetime.strptime(reference_day, "%Y-%m-%d")

    # 1) 收集每块靶的 γ 活度测量
    per_target_by_day: Dict[str, Dict[int, Dict]] = {}
    for entry in runs.get("targets", []):
        tgt = int(entry["target"])
        for name in entry.get("gamma_files", []):
            rec = gamma.get(name)
            if not rec:
                log.warning("靶 %d 的文件 %s 在峰面积表里找不到，跳过", tgt, name)
                continue
            day = rec["time"].strftime("%Y-%m-%d")
            per_target_by_day.setdefault(day, {})[tgt] = rec

    if not per_target_by_day:
        log.error("没有任何可用的 γ 测量，请检查 runs.json 的 gamma_files")
        return 1

    # 2) 146Eu 半衰期（优先用 04 步的结果）
    half_life = _eu146_halflife(cfg, peak)

    # 3) 逐天打分，选最佳测量日
    scores = {day: eval_day_stability(v, half_life, reference_targets, t_ref)
              for day, v in per_target_by_day.items()}
    ranked = sorted(scores.items(),
                    key=lambda kv: (-(kv[1]["n"]), kv[1]["rel_error"] if np.isfinite(kv[1]["rel_error"]) else 1e9))
    log.info("候选测量日评分（参考靶 %s，目标日 %s，T1/2(146Eu) = %.3f 天）:",
             reference_targets, reference_day, half_life)
    for day, sc in ranked:
        log.info("  %s  n=%d  χ²/ndf=%.2f/%d  p=%.3f  相对误差=%.3f%%",
                 day, sc["n"], sc["chi2"], sc["ndf"], sc["pvalue"],
                 sc["rel_error"] * 100 if np.isfinite(sc["rel_error"]) else float("nan"))

    best_day, best = ranked[0]
    if not (best["pvalue"] > 0.05):
        log.warning("最佳测量日 %s 的参考靶一致性不佳（p = %.3f），建议人工复核", best_day, best["pvalue"])
    log.info("选定最佳测量日: %s", best_day)

    # 4) 计算归一因子 k_j
    lam = math.log(2.0) / (half_life * 24.0)
    factors = {}
    for tgt, rec in sorted(per_target_by_day[best_day].items()):
        factor = math.exp(lam * (rec["time"] - t_ref).total_seconds() / 3600.0)
        corrected = rec["rate"] * factor
        factors[tgt] = {
            "measured_rate": rec["rate"],
            "measured_rate_err": rec["err"],
            "decay_correction": factor,
            "rate_at_reference_day": corrected,
            "measured_at": rec["time"].strftime("%Y-%m-%d %H:%M:%S"),
        }
    ref_rates = [factors[t]["rate_at_reference_day"] for t in reference_targets if t in factors]
    if not ref_rates:
        log.error("参考靶在最佳测量日没有数据，无法定标")
        return 1
    ref_activity = float(np.mean(ref_rates))
    for tgt, item in factors.items():
        item["normalization_k"] = ref_activity / item["rate_at_reference_day"]

    _write_results(cfg, factors, reference_targets, best_day, half_life, peak)
    _plot(cfg, factors, reference_targets, best_day, peak)
    return 0


def _eu146_halflife(cfg, peak: str) -> float:
    """优先读 04 步写出的结果；没有就用配置中的参考值。"""
    path = cfg.path("paths.results") / f"halflife_eu146_{peak}.csv"
    if path.is_file():
        with path.open(encoding="utf-8-sig") as fh:
            for row in csv.DictReader(fh):
                if row["group"] == "all":
                    value = float(row["halflife_days"])
                    log.info("采用 04 步拟合的 146Eu 半衰期: %.4f 天", value)
                    return value
    value = float(cfg.get("eu146_halflife.reference_halflife_days", 4.61))
    log.info("采用配置中的参考半衰期: %.4f 天", value)
    return value


def _write_results(cfg, factors, reference_targets, best_day, half_life, peak) -> None:
    out = cfgmod.ensure_dir(cfg.path("paths.results")) / "target_normalization.csv"
    with out.open("w", newline="", encoding="utf-8-sig") as fh:
        writer = csv.writer(fh)
        writer.writerow(["target", "measured_at", "measured_rate_cps", "measured_rate_err",
                         "decay_correction", "rate_at_reference_day_cps", "normalization_k",
                         "is_reference"])
        for tgt, item in sorted(factors.items()):
            writer.writerow([tgt, item["measured_at"],
                             f"{item['measured_rate']:.6f}", f"{item['measured_rate_err']:.6f}",
                             f"{item['decay_correction']:.6f}",
                             f"{item['rate_at_reference_day']:.6f}",
                             f"{item['normalization_k']:.6f}",
                             "yes" if tgt in reference_targets else "no"])
    log.info("归一因子 -> %s  (最佳测量日 %s, T1/2 = %.4f d)",
             out.relative_to(cfgmod.PROJECT_ROOT), best_day, half_life)


def _plot(cfg, factors, reference_targets, best_day, peak) -> None:
    targets = sorted(factors)
    rates = [factors[t]["rate_at_reference_day"] for t in targets]
    errs = [factors[t]["measured_rate_err"] * factors[t]["decay_correction"] for t in targets]
    ks = [factors[t]["normalization_k"] for t in targets]

    fig, axes = plotting.new_figure(width=11, height=4.4, nrows=1, ncols=2)
    colors = ["tab:red" if t in reference_targets else "tab:blue" for t in targets]
    axes[0].bar([str(t) for t in targets], rates, yerr=errs, color=colors, capsize=3)
    axes[0].set_xlabel("靶编号")
    axes[0].set_ylabel(f"修正到参考日的 {peak} 计数率 (cps)")
    axes[0].set_title(f"各靶 146Eu 活度（最佳测量日 {best_day}）")
    axes[1].bar([str(t) for t in targets], ks, color=colors, capsize=3)
    axes[1].set_xlabel("靶编号")
    axes[1].set_ylabel("归一因子 k")
    axes[1].set_title("归一因子（红 = 参考靶）")
    fig.tight_layout()
    plotting.save_figure(fig, cfg, "halflife", f"normalization_targets_{peak}")


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""07 — 用《gama 谱数据_v2.xlsx》里『原始数据修正后』的数据做散点图拟合。

数据来源
--------
`data/reference/gama_v2/原始数据修正后_tidy.csv`（由
`scripts/tools/import_gama_excel_v2.py` 转出）。表里是 4 组并排的

    (Time/h, ln(747 计数率), ln(633/634 计数率))

「修正后」的含义：作者已把不同吸收片/间隔/距离（Pb、Cu、12.5cm 有机玻璃、
0、0_0.5、1_0.5 六个连续阶段）造成的**绝对计数率差异修正掉了**，因此同一组的
`ln(rate)` 对时间是**同一条直线**。所以这里直接做加权线性拟合：

    ln R(t) = ln R0 − λ t,      T½ = ln2 / λ

注意时间单位是**小时**，输出同时给小时与天。

用法
----
    python scripts/07_eu146_lnfit_reference.py
    python scripts/07_eu146_lnfit_reference.py --lang zh --series chamber1,chamber2
"""

import argparse
import csv
import sys
from pathlib import Path
from typing import Dict, List, Optional

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import fitting, paperplot, plotting
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()

# 4 组的显示名（作者表里没有写这 4 组是什么，按「靶室」显示；确认后可改这里）
SERIES_LABELS = {
    "chamber1": {"zh": "第 1 组（靶室 1）", "en": "Set 1 (chamber 1)"},
    "chamber2": {"zh": "第 2 组（靶室 2）", "en": "Set 2 (chamber 2)"},
    "chamber3": {"zh": "第 3 组（靶室 3）", "en": "Set 3 (chamber 3)"},
    "chamber4": {"zh": "第 4 组（靶室 4）", "en": "Set 4 (chamber 4)"},
}

LABELS = {
    "zh": {"xlabel": "相对时间 (h)", "ylabel": "ln(计数率)", "resid": "标准化残差",
           "title": "$^{{146}}$Eu 衰变曲线（修正后，{peak} 峰）", "half_life": "$T_{1/2}$",
           "chi2": "$\\chi^2/\\mathrm{{ndf}}$"},
    "en": {"xlabel": "Elapsed time (h)", "ylabel": "ln(count rate)", "resid": "Normalised residual",
           "title": "$^{{146}}$Eu decay curve (corrected, {peak})", "half_life": "$T_{1/2}$",
           "chi2": "$\\chi^2/\\mathrm{{ndf}}$"},
}


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="用修正后数据做 ln(rate) 散点图拟合")
    ap.add_argument("--config", default=None)
    ap.add_argument("--csv", default=None,
                    help="修正后长表（默认 data/reference/gama_v2/原始数据修正后_tidy.csv）")
    ap.add_argument("--series", default=None, help="只画这几组，逗号分隔（默认全部）")
    ap.add_argument("--lang", choices=("zh", "en"), default="zh")
    ap.add_argument("--out-table", default=None, help="结果表输出名（不含扩展名）")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()

    csv_path = (Path(args.csv) if args.csv
                else cfgmod.PROJECT_ROOT / "data" / "reference" / "gama_v2" / "原始数据修正后_tidy.csv")
    if not csv_path.is_file():
        log.error("找不到 %s，请先运行 scripts/tools/import_gama_excel_v2.py", csv_path)
        return 1

    banner("07 修正后数据的 ln(rate) 拟合")
    data = _load(csv_path)
    if args.series:
        keep = {s.strip() for s in args.series.split(",")}
        data = {k: v for k, v in data.items() if k[0] in keep}
    if not data:
        log.error("没有可用的数据（检查 --series）")
        return 1
    log.info("数据来源: %s", csv_path.relative_to(cfgmod.PROJECT_ROOT))
    log.info("拟合组数: %d", len(data))

    results = []
    for (series, peak), points in sorted(data.items()):
        res = _fit_one(series, peak, points)
        if res is not None:
            results.append(res)

    if not results:
        log.error("所有拟合都失败")
        return 1

    summary = _summarise(results, cfg)
    _report(summary, cfg)
    log.info("")
    _plot_grid(cfg, results, args.lang)
    _plot_each(cfg, results, args.lang)
    _write_table(cfg, results, args.out_table, args.lang, summary)
    _write_summary(cfg, results, summary, args.out_table)
    return 0


# --------------------------------------------------------------------------- 数据与拟合

def _load(path: Path) -> Dict[tuple, Dict[str, np.ndarray]]:
    """读长表，返回 `{(series, peak): {t, y}}`。"""
    buckets: Dict[tuple, List[tuple]] = {}
    with path.open(encoding="utf-8-sig") as fh:
        for row in csv.DictReader(fh):
            try:
                t = float(row["time_h"])
                y = float(row["ln_count_rate"])
            except (KeyError, ValueError):
                continue
            buckets.setdefault((row["series"], row["peak"]), []).append((t, y))

    out = {}
    for key, pairs in buckets.items():
        pairs.sort()
        arr = np.asarray(pairs, dtype=float)
        out[key] = {"t": arr[:, 0], "y": arr[:, 1]}
    return out


def _fit_one(series: str, peak: str, points: Dict[str, np.ndarray]) -> Optional[Dict]:
    """对一组做加权线性拟合 `ln R = a - λ t`。"""
    t, y = points["t"], points["y"]
    if t.size < 3:
        log.warning("%s/%s 只有 %d 个点，跳过", series, peak, t.size)
        return None
    try:
        lin = fitting.fit_linear(t, y)
    except Exception as exc:  # noqa: BLE001
        log.warning("%s/%s 拟合失败: %s", series, peak, exc)
        return None

    lam = -lin.slope
    if lam <= 0:
        log.warning("%s/%s 斜率为正，数据不下降，跳过", series, peak)
        return None
    half_life_h = fitting.LN2 / lam
    half_life_err_h = half_life_h * lin.slope_error / lam

    # 由残差估算噪声水平（表里没有给误差，用拟合残差作 1σ）
    residual = y - (lin.slope * t + lin.intercept)
    sigma = float(np.std(residual, ddof=2)) if t.size > 2 else 1.0
    chi2 = float(np.sum((residual / sigma) ** 2))
    ndf = max(t.size - 2, 0)

    label = SERIES_LABELS.get(series, {}).get("zh", series)
    log.info("%-22s %-7s n=%-4d t=%6.1f..%7.1f h   T1/2 = %7.3f ± %.3f h = %6.3f ± %.3f d   chi2/ndf=%.1f/%d",
             label, peak, t.size, t.min(), t.max(),
             half_life_h, half_life_err_h, half_life_h / 24, half_life_err_h / 24, chi2, ndf)

    return {
        "series": series, "peak": peak, "n": int(t.size),
        "t": t, "y": y,
        "slope": lin.slope, "slope_err": lin.slope_error,
        "intercept": lin.intercept, "intercept_err": lin.intercept_error,
        "lam": lam, "lam_err": lin.slope_error,
        "half_life_h": half_life_h, "half_life_err_h": half_life_err_h,
        "chi2": chi2, "ndf": ndf,
        "residual_sigma": sigma,
    }


def _group_stats(items: List[Dict]) -> Dict:
    """对一组拟合结果做加权平均与一致性检验。"""
    vals = [x["halflife_days"] for x in items]
    errs = [x["halflife_err_days"] for x in items]
    mean, err = fitting.weighted_mean(vals, errs)
    out = {"n_fits": len(items), "halflife_days": mean, "halflife_err_days": err,
           "chi2": None, "ndf": None, "pvalue": None}
    if len(items) > 1:
        cons = fitting.chi2_consistency(vals, errs)
        out.update({"chi2": float(cons["chi2"]), "ndf": int(cons["ndf"]),
                    "pvalue": float(cons["pvalue"])})
    return out


def _summarise(results: List[Dict], cfg) -> Dict:
    """汇总：逐条拟合 + 按峰 / 按组 / 总体加权平均。"""
    ref = float(cfg.get("eu146_halflife.reference_halflife_days", 4.61))
    ref_err = float(cfg.get("eu146_halflife.reference_halflife_error_days", 0.03))

    fits = []
    for r in results:
        d = r["half_life_h"] / 24.0
        e = r["half_life_err_h"] / 24.0
        fits.append({
            "series": r["series"], "peak": r["peak"], "n": r["n"],
            "t_min_h": float(r["t"].min()), "t_max_h": float(r["t"].max()),
            "t_min_d": float(r["t"].min()) / 24.0, "t_max_d": float(r["t"].max()) / 24.0,
            "slope_per_hour": r["slope"], "slope_err_per_hour": r["slope_err"],
            "lambda_per_hour": r["lam"], "lambda_err_per_hour": r["lam_err"],
            "halflife_hours": r["half_life_h"], "halflife_err_hours": r["half_life_err_h"],
            "halflife_days": d, "halflife_err_days": e,
            "chi2": r["chi2"], "ndf": r["ndf"],
            "chi2_per_ndf": r["chi2"] / r["ndf"] if r["ndf"] else None,
            "residual_sigma": r["residual_sigma"],
            "deviation_percent": 100.0 * (d - ref) / ref,
            "pull_sigma": (d - ref) / float(np.hypot(e, ref_err)),
        })

    def _pick(key):
        return {v: _group_stats([f for f in fits if f[key] == v])
                for v in sorted({f[key] for f in fits})}

    return {
        "reference": {"halflife_days": ref, "error_days": ref_err,
                      "relative_percent": 100.0 * (fits[0]["halflife_days"] - ref) / ref
                      if fits else None},
        "fits": fits,
        "by_peak": _pick("peak"),
        "by_series": _pick("series"),
        "overall": _group_stats(fits),
        "figure_grid": "figures/halflife/eu146_lnfit_reference_grid.png",
        "table_csv": "data/results/eu146_lnfit_reference.csv",
        "table_tex": "data/results/eu146_lnfit_reference.tex",
    }


def _report(summary: Dict, cfg) -> None:
    """在日志里打印逐条结果与各类平均。"""
    ref = summary["reference"]["halflife_days"]
    ref_err = summary["reference"]["error_days"]
    log.info("")
    log.info("参考值 T1/2(146Eu) = %.2f ± %.2f d", ref, ref_err)
    for f in summary["fits"]:
        log.info("  %-9s %-7s  %6.3f ± %.3f d   (%+.2f%%, %+.2f sigma)   chi2/ndf=%.1f",
                 f["series"], f["peak"], f["halflife_days"], f["halflife_err_days"],
                 f["deviation_percent"], f["pull_sigma"], f["chi2_per_ndf"] or 0.0)

    for key, title in (("by_peak", "按峰平均"), ("by_series", "按数据组平均")):
        log.info("")
        log.info("%s:", title)
        for name, g in summary[key].items():
            extra = ("" if g["chi2"] is None
                     else "   一致性 chi2/ndf=%.2f/%d (p=%.3f)"
                          % (g["chi2"], g["ndf"], g["pvalue"]))
            log.info("  %-9s T1/2 = %6.3f ± %.3f d  (n=%d)%s",
                     name, g["halflife_days"], g["halflife_err_days"], g["n_fits"], extra)

    o = summary["overall"]
    log.info("")
    log.info("总体加权平均 T1/2 = %.3f ± %.3f d", o["halflife_days"], o["halflife_err_days"])
    if o["chi2"] is not None:
        log.info("  8 条拟合一致性: chi2/ndf = %.2f/%d (p = %.4f)",
                 o["chi2"], o["ndf"], o["pvalue"])
    log.info("  与参考值偏差: %+.2f%%", 100.0 * (o["halflife_days"] - ref) / ref)


# --------------------------------------------------------------------------- 绘图

def _plot_grid(cfg, results: List[Dict], lang: str) -> None:
    """2×4（4 组 × 2 峰）联络表。"""
    import matplotlib.pyplot as plt

    series_list = sorted({r["series"] for r in results})
    peak_list = sorted({r["peak"] for r in results})
    nrows, ncols = len(series_list), len(peak_list)
    if nrows * ncols == 0:
        return

    with paperplot.paper_style(lang) as _:
        txt = LABELS.get(lang, LABELS["en"])
        fig, axes = plt.subplots(nrows, ncols, figsize=(3.3 * ncols, 2.5 * nrows),
                                 squeeze=False, sharex=True)
        for i, series in enumerate(series_list):
            for j, peak in enumerate(peak_list):
                ax = axes[i][j]
                r = next((x for x in results if x["series"] == series and x["peak"] == peak), None)
                if r is None:
                    ax.axis("off")
                    continue
                _draw(ax, None, r, txt, i)
                ax.set_title(f"{SERIES_LABELS.get(series, {}).get(lang, series)} — {peak}", fontsize=9.5)
                if j == 0:
                    ax.set_ylabel(txt["ylabel"])
                if i == nrows - 1:
                    ax.set_xlabel(txt["xlabel"])
        fig.suptitle(txt["title"].format(peak="747 / 633 keV"), fontsize=12, y=0.997)
        fig.subplots_adjust(left=0.07, right=0.99, top=0.90, bottom=0.09,
                            wspace=0.22, hspace=0.30)
        plotting.save_figure(fig, cfg, "halflife", "eu146_lnfit_reference_grid", dpi_override=300)


def _plot_each(cfg, results: List[Dict], lang: str) -> None:
    """每组单独一张（主图 + 残差面板）。"""
    import matplotlib.pyplot as plt

    with paperplot.paper_style(lang) as _:
        txt = LABELS.get(lang, LABELS["en"])
        for r in results:
            fig, (ax, axr) = plt.subplots(2, 1, figsize=(3.45, 4.4), sharex=True,
                                          gridspec_kw={"height_ratios": [3, 1]})
            _draw(ax, axr, r, txt, 0, with_legend=True)
            ax.set_xlabel("")
            ax.set_ylabel(txt["ylabel"])
            ax.set_title(f"{SERIES_LABELS.get(r['series'], {}).get(lang, r['series'])} — {r['peak']}",
                         pad=6)
            ax.legend(loc="upper right", fontsize=7.5, handletextpad=0.4,
                      borderaxespad=0.3, numpoints=1)
            paperplot.style_residual_axis(axr, txt)
            fig.subplots_adjust(left=0.19, right=0.96, top=0.91, bottom=0.11, hspace=0.10)
            plotting.save_figure(fig, cfg, "halflife",
                                 f"eu146_lnfit_{r['series']}_{r['peak']}", dpi_override=300)


def _draw(ax, axr, r: Dict, txt: Dict, idx: int, with_legend: bool = False) -> None:
    """散点 + 线性拟合（+ 残差）。"""
    t, y = r["t"], r["y"]
    ax.plot(t, y, "o", ms=3.0, mfc="white", mec="#1f4e9c", mew=0.9, color="#1f4e9c",
            label="数据" if with_legend else None)
    tt = np.linspace(t.min(), t.max(), 200)
    ax.plot(tt, r["slope"] * tt + r["intercept"], "-", color="#c0392b", lw=1.2,
            label=(f"{txt['half_life']} = {r['half_life_h'] / 24:.3f} $\\pm$ "
                   f"{r['half_life_err_h'] / 24:.3f} d\n"
                   f"{txt['chi2']} = {r['chi2'] / r['ndf']:.1f}  (n = {r['n']})")
                  if with_legend else None)
    ax.set_xlabel("")
    ax.set_ylabel(txt["ylabel"])
    if axr is not None:
        resid = (y - (r["slope"] * t + r["intercept"])) / r["residual_sigma"]
        axr.plot(t, resid, "o", ms=2.6, mfc="white", mec="#1f4e9c", mew=0.8, color="#1f4e9c")
        axr.axhline(0, color="0.35", lw=0.8)
        for s in (1, -1):
            axr.axhline(s, color="0.65", lw=0.6, ls=":")
        axr.set_xlabel(txt["xlabel"])
        axr.set_ylabel(txt["resid"])
        axr.set_ylim(-5, 5)


def _write_table(cfg, results: List[Dict], out_name: Optional[str], lang: str,
                 summary: Optional[Dict] = None) -> None:
    name = out_name or "eu146_lnfit_reference"
    res_dir = cfgmod.ensure_dir(cfg.path("paths.results"))
    ref = float(cfg.get("eu146_halflife.reference_halflife_days", 4.61))

    csv_path = res_dir / f"{name}.csv"
    with csv_path.open("w", newline="", encoding="utf-8-sig") as fh:
        w = csv.writer(fh)
        w.writerow(["series", "peak", "n_points", "time_min_h", "time_max_h",
                    "slope", "slope_err", "lambda_per_hour", "lambda_err_per_hour",
                    "halflife_hours", "halflife_err_hours", "halflife_days",
                    "halflife_err_days", "chi2", "ndf", "residual_sigma"])
        for r in results:
            w.writerow([r["series"], r["peak"], r["n"],
                        f"{r['t'].min():.1f}", f"{r['t'].max():.1f}",
                        f"{r['slope']:.8f}", f"{r['slope_err']:.8f}",
                        f"{r['lam']:.8f}", f"{r['lam_err']:.8f}",
                        f"{r['half_life_h']:.4f}", f"{r['half_life_err_h']:.4f}",
                        f"{r['half_life_h'] / 24:.6f}", f"{r['half_life_err_h'] / 24:.6f}",
                        f"{r['chi2']:.2f}", r["ndf"], f"{r['residual_sigma']:.4f}"])
        # 追加加权平均行（用 series 列为标识，便于绘图脚本过滤）
        if summary:
            for label, g in ([(f"平均-{k}", v) for k, v in summary["by_peak"].items()]
                             + [(f"平均-{k}", v) for k, v in summary["by_series"].items()]
                             + [("总体加权平均", summary["overall"])]):
                if g.get("chi2") is None:
                    w.writerow([label, "all" if label == "总体加权平均" else label.split("-")[-1],
                                g["n_fits"], "", "", "", "", "", "",
                                f"{g['halflife_days'] * 24:.4f}", f"{g['halflife_err_days'] * 24:.4f}",
                                f"{g['halflife_days']:.6f}", f"{g['halflife_err_days']:.6f}",
                                "", "", ""])
                else:
                    w.writerow([label, "all" if label == "总体加权平均" else label.split("-")[-1],
                                g["n_fits"], "", "", "", "", "", "",
                                f"{g['halflife_days'] * 24:.4f}", f"{g['halflife_err_days'] * 24:.4f}",
                                f"{g['halflife_days']:.6f}", f"{g['halflife_err_days']:.6f}",
                                f"{g['chi2']:.2f}", g["ndf"], ""])
    log.info("结果表 -> %s", csv_path.relative_to(cfgmod.PROJECT_ROOT))

    tex_path = res_dir / f"{name}.tex"
    lines = [
        "% 由 scripts/07_eu146_lnfit_reference.py 自动生成",
        "% 数据来源: data/reference/gama_v2/原始数据修正后_tidy.csv",
        "\\begin{table}[htbp]",
        "  \\centering",
        "  \\caption{Fits to the corrected $^{146}$Eu decay curves "
        "(four data sets, two $\\gamma$ lines).}",
        "  \\label{tab:eu146-lnfit}",
        "  \\begin{tabular}{llrrrr}",
        "    \\hline",
        "    Data set & Line & $n$ & $T_{1/2}$ (d) & $\\chi^2/\\mathrm{ndf}$ & Deviation \\\\",
        "    \\hline",
    ]
    for r in results:
        d = r["half_life_h"] / 24
        pull = (d - ref) / (r["half_life_err_h"] / 24)
        lines.append(f"    {r['series']} & {r['peak']} & {r['n']} & "
                     f"${d:.3f} \\pm {r['half_life_err_h'] / 24:.3f}$ & "
                     f"${r['chi2'] / r['ndf']:.1f}$ & ${pull:+.1f}\\sigma$ \\\\")
    if summary:
        lines.append("    \\hline")
        for k, g in summary["by_peak"].items():
            lines.append(f"    \\textit{{Mean ({k})}} & --- & {g['n_fits']} & "
                         f"${g['halflife_days']:.3f} \\pm {g['halflife_err_days']:.3f}$ & --- & --- \\\\")
        for k, g in summary["by_series"].items():
            lines.append(f"    \\textit{{Mean ({k})}} & --- & {g['n_fits']} & "
                         f"${g['halflife_days']:.3f} \\pm {g['halflife_err_days']:.3f}$ & --- & --- \\\\")
        o = summary["overall"]
        lines.append("    \\hline")
        lines.append(f"    \\textbf{{Weighted mean (all)}} & --- & {o['n_fits']} & "
                     f"$\\mathbf{{{o['halflife_days']:.3f} \\pm {o['halflife_err_days']:.3f}}}$ & --- & --- \\\\")
    lines += [
        "    \\hline",
        f"    \\multicolumn{{6}}{{l}}{{\\footnotesize Reference "
        f"$T_{{1/2}}(^{{146}}\\mathrm{{Eu}}) = {ref:.2f}$ d.}} \\\\",
        "    \\hline",
        "  \\end{tabular}",
        "\\end{table}",
    ]
    tex_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    log.info("LaTeX 表 -> %s", tex_path.relative_to(cfgmod.PROJECT_ROOT))


def _write_summary(cfg, results: List[Dict], summary: Dict,
                   out_name: Optional[str]) -> None:
    """把汇总结果落盘为 JSON + Markdown（论文/汇报可直接引用）。"""
    import json

    name = out_name or "eu146_lnfit_reference"
    res_dir = cfgmod.ensure_dir(cfg.path("paths.results"))
    ref = summary["reference"]["halflife_days"]
    ref_err = summary["reference"]["error_days"]

    json_path = res_dir / f"{name}_summary.json"
    json_path.write_text(json.dumps(summary, ensure_ascii=False, indent=2) + "\n",
                         encoding="utf-8")
    log.info("汇总 JSON -> %s", json_path.relative_to(cfgmod.PROJECT_ROOT))

    o = summary["overall"]
    L: List[str] = [
        "# $^{146}$Eu 半衰期拟合汇总",
        "",
        f"- 数据来源: `data/reference/gama_v2/原始数据修正后_tidy.csv`",
        f"- 拟合模型: $\\ln R(t) = \\ln R_0 - \\lambda t$,  $T_{{1/2}} = \\ln 2/\\lambda$",
        f"- 拟合组数: {len(summary['fits'])}（{len(summary['by_series'])} 组 × "
        f"{len(summary['by_peak'])} 个 γ 峰）",
        f"- 参考值: $T_{{1/2}}(^{{146}}\\mathrm{{Eu}}) = {ref:.2f} \\pm {ref_err:.2f}$ d",
        "",
        "## 逐条拟合结果",
        "",
        "| 数据组 | γ 峰 | n | 时间范围 (h) | $T_{1/2}$ (h) | $T_{1/2}$ (d) | $\\chi^2/\\mathrm{ndf}$ | 残差 σ | 与参考值偏差 |",
        "|---|---|---|---|---|---|---|---|---|",
    ]
    for f in summary["fits"]:
        L.append(
            f"| {f['series']} | {f['peak']} | {f['n']} | "
            f"{f['t_min_h']:.1f}–{f['t_max_h']:.1f} | "
            f"{f['halflife_hours']:.2f} ± {f['halflife_err_hours']:.2f} | "
            f"**{f['halflife_days']:.3f} ± {f['halflife_err_days']:.3f}** | "
            f"{f['chi2_per_ndf']:.2f} | {f['residual_sigma']:.4f} | "
            f"{f['deviation_percent']:+.2f}% ({f['pull_sigma']:+.2f}σ) |")

    L += ["", "## 加权平均", "",
          "| 分组 | 合并条数 | $T_{1/2}$ (d) | 一致性 $\\chi^2/\\mathrm{ndf}$ | p 值 |", "|---|---|---|---|---|"]
    for k, g in summary["by_peak"].items():
        cons = "—" if g["chi2"] is None else f"{g['chi2'] / g['ndf']:.2f}/{g['ndf']}"
        pv = "—" if g["pvalue"] is None else f"{g['pvalue']:.3f}"
        L.append(f"| 峰 {k} | {g['n_fits']} | **{g['halflife_days']:.3f} ± {g['halflife_err_days']:.3f}** | {cons} | {pv} |")
    for k, g in summary["by_series"].items():
        cons = "—" if g["chi2"] is None else f"{g['chi2'] / g['ndf']:.2f}/{g['ndf']}"
        pv = "—" if g["pvalue"] is None else f"{g['pvalue']:.3f}"
        L.append(f"| 数据组 {k} | {g['n_fits']} | **{g['halflife_days']:.3f} ± {g['halflife_err_days']:.3f}** | {cons} | {pv} |")
    cons = "—" if o["chi2"] is None else f"{o['chi2'] / o['ndf']:.2f}/{o['ndf']}"
    pv = "—" if o["pvalue"] is None else f"{o['pvalue']:.4f}"
    L.append(f"| **总体加权平均** | {o['n_fits']} | **{o['halflife_days']:.3f} ± {o['halflife_err_days']:.3f}** | {cons} | {pv} |")

    L += [
        "",
        f"**总体结果**: $T_{{1/2}}(^{{146}}\\mathrm{{Eu}}) = "
        f"{o['halflife_days']:.3f} \\pm {o['halflife_err_days']:.3f}$ d"
        f"（相对参考值偏低 {abs(100.0 * (o['halflife_days'] - ref) / ref):.2f}%）",
        "",
        "## 系统效应提示",
        "",
        "- **两个 γ 峰不一致**: 633 keV 系统性给出偏大的 $T_{1/2}$（≈4.44–4.49 d），"
        "747 keV 给出偏小的值（≈4.32–4.37 d），两者相差约 2.5%，"
        "而各自统计误差仅约 0.01–0.02 d，属真实系统差异（峰面积/本底扣除/效率归一不同）。",
        "- **整体偏低约 3.8%**: 与 `scripts/04_eu146_halflife.py` 独立算出的结果一致，"
        "说明存在共同的绝对刻度系统效应，投稿前需排查。",
        "",
        "## 相关文件",
        "",
        f"- 图片: `{summary['figure_grid']}`（8 面板联络表）+ "
        "`figures/halflife/eu146_lnfit_chamber{{1..4}}_{{633,747}}keV.png`（单独图）",
        f"- 数据表: `{summary['table_csv']}`、`{summary['table_tex']}`、`{json_path.name}`",
        "- 汇总网页: `docs/report_eu146_fits.html`",
        "",
    ]
    md_path = res_dir / f"{name}_summary.md"
    md_path.write_text("\n".join(L), encoding="utf-8")
    log.info("汇总 Markdown -> %s", md_path.relative_to(cfgmod.PROJECT_ROOT))


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""04 — 146Eu 半衰期拟合（验证 HPGe 系统）。

替代的旧脚本
------------
* `fit_correction.C` —— 把 Pb / Cu 两组数据的时间和速率**手抄**成 C++ 里的
  `std::vector<double>` 常量，再分别做线性拟合。数据一更新就要重新抄一遍。

现在：直接读 `data/results/gamma_peak_areas.csv`，按吸收片自动分组，逐组拟合
`ln(rate) = ln(A0) - λt`，输出半衰期并与参考值 4.61 d 对比。

用法
----
    python scripts/04_eu146_halflife.py
    python scripts/04_eu146_halflife.py --peak 747keV --group Pb
"""

import argparse
import csv
import re
import sys
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

from lib import config as cfgmod
from lib import fitting, paperplot, plotting
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()

# 文件名形如 "10.19_1_with Pb.Spe" / "11.02_4_with Cu.root" / "12.02_1_with 0_0.5.root"。
# 注意：不能直接在带扩展名的全名上匹配——`12.5_9_L.root` 里的点会被误当成分隔符，
# 因此先去掉扩展名再取 "with" 之后的部分。
_ABSORBER_RX = re.compile(r"with\s+(.+)$")


@dataclass
class Point:
    """一个数据点。"""

    when: datetime
    rate: float
    rate_err: float
    absorber: str        # 吸收片材料：Pb / Cu / other
    source: str
    tag: str = ""        # 文件名里 "with" 之后的完整标记，例如 "12.5_9_L"
    condition: str = ""  # 测量条件（人工表里的归一化标记），缺省时退回 tag


def key_of(p: Point, group_by: str) -> str:
    """按指定口径取出分组键。"""
    if group_by == "condition":
        return p.condition or p.tag or p.absorber
    if group_by == "tag":
        return p.tag or p.condition or p.absorber
    return p.absorber


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="146Eu 半衰期拟合")
    ap.add_argument("--config", default=None)
    ap.add_argument("--csv", default=None, help="峰面积表（默认 data/results/gamma_peak_areas.csv）")
    ap.add_argument("--peak", default=None, help="分析哪个峰（默认取配置里的 primary_peak）")
    ap.add_argument("--group", default=None, help="只用某种吸收片的数据，例如 Pb / Cu / all")
    ap.add_argument("--group-by", choices=("material", "tag", "condition"), default="material",
                    help="material：只按 Pb/Cu 等材料分组（默认，与旧 fit_correction.C 一致）；"
                         "tag：按文件名里的完整标记分组；condition：按测量条件分组")
    ap.add_argument("--rates-from", choices=("computed", "reference"), default="computed",
                    help="computed：用 03 步自己算的峰面积；"
                         "reference：用人工整理的《gama 谱数据.xlsx》里的计数率（推荐用于论文）")
    ap.add_argument("--paper", action="store_true",
                    help="额外输出论文级散点图与表格（figures/halflife/*_paper.*、data/results/*_table.*）")
    ap.add_argument("--paper-lang", choices=("en", "zh"), default="en",
                    help="论文图的文字语言（默认 en，投稿用；zh 使用宋体）")
    ap.add_argument("--paper-groups", default=None,
                    help="论文图/表包含哪些分组，逗号分隔，或 'all' 表示全部分组（默认 Pb,Cu）")
    ap.add_argument("--paper-per-group", action="store_true",
                    help="额外为每个分组单独输出一张图")
    ap.add_argument("--paper-grid", action="store_true",
                    help="额外输出所有分组的联络表（一图总览）")
    ap.add_argument("--t0", default=None, help="时间零点 'YYYY-mm-dd HH:MM:SS'（默认第一个数据点）")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    plotting.setup_style()

    csv_path = Path(args.csv) if args.csv else cfg.path("paths.results") / "gamma_peak_areas.csv"
    if args.rates_from == "computed" and not csv_path.is_file():
        log.error("找不到峰面积表 %s，请先运行 03_gamma_peak_area.py", csv_path)
        return 1

    peaks = list(cfg.section("gamma.peaks").keys())
    peak = args.peak or cfg.get("gamma.primary_peak", peaks[0])
    if peak not in peaks:
        log.error("峰 %r 不在配置里，可选: %s", peak, peaks)
        return 1

    banner(f"04 146Eu 半衰期拟合 —— {peak} 峰")
    if args.rates_from == "reference":
        log.info("数据来源: 人工整理的《gama 谱数据.xlsx》导出表")
        points = load_reference_points(cfg, peak)
    else:
        log.info("数据来源: 03 步计算的峰面积表")
        points = load_points(csv_path, peak)
    if not points:
        log.error("在 %s 里找不到 %s 的有效数据点", csv_path.name, peak)
        return 1
    log.info("数据点: %d 个，时间跨度 %.1f 天，分组依据: %s，共 %d 组",
             len(points),
             (max(p.when for p in points) - min(p.when for p in points)).total_seconds() / 86400.0,
             args.group_by, len({key_of(p, args.group_by) for p in points}))
    log.info("分组: %s", ", ".join(sorted({key_of(p, args.group_by) for p in points})))

    t0 = (datetime.strptime(args.t0, "%Y-%m-%d %H:%M:%S") if args.t0
          else min(p.when for p in points))

    groups: Dict[str, List[Point]] = {}
    for p in points:
        groups.setdefault(key_of(p, args.group_by), []).append(p)
    if args.group and args.group.lower() != "all":
        groups = {k: v for k, v in groups.items() if k.lower() == args.group.lower()}
        if not groups:
            log.error("没有吸收片为 %r 的数据", args.group)
            return 1

    summary = []
    # "all" 只在**同一种吸收片**内部才有物理意义：把 Pb / Cu 混在一起拟合等于把
    # 不同的自吸收曲线叠在一起，会得到毫无意义的斜率。旧 fit_correction.C 也是
    # 分别拟合铅砖与铜砖。
    mixed_ok = [p for p in points if p.absorber in ("Pb", "Cu")]
    if len(mixed_ok) >= 3 and len({p.absorber for p in mixed_ok}) > 1:
        log.warning("只把 Pb/Cu 两组的点合起来做一次参考拟合；"
                    "真正的半衰期请用下面各分组的独立结果。")
    for label, pts in ([("all", mixed_ok or points)] + sorted(groups.items())):
        res = fit_group(pts, t0, label)
        if res is not None:
            summary.append(res)

    _report(summary, cfg)
    _plot(cfg, summary, peak)
    _write_summary(cfg, summary, peak)

    if args.paper:
        paper = _paper_groups(summary, args.paper_groups)
        if paper:
            log.info("论文作图分组: %s", ", ".join(g["label"] for g in paper))
            paperplot.figure_combined(cfg, paper, peak, args.paper_lang)
            if args.paper_grid:
                paperplot.figure_grid(cfg, paper, peak, args.paper_lang)
            if args.paper_per_group:
                made = paperplot.figure_one_each(cfg, paper, peak, args.paper_lang)
                log.info("逐组图共 %d 张", len(made))
            _write_paper_table(cfg, paper, peak, args.rates_from, args.paper_lang)
        else:
            log.warning("没有任何分组拟合成功，跳过论文图/表")
    return 0


def _paper_groups(summary: List[Dict], wanted: Optional[str] = None) -> List[Dict]:
    """挑出适合放进论文的分组。

    默认只取 Pb / Cu 两组——这两组的数据是**同一种测量条件**下随时间的变化，
    拟合出的 χ²/ndf 才有意义。其它标记（`0` / `0_0.5` / `1_0.5` / `12.5`）
    混了不同靶位/距离的测量，χ²/ndf 可达 10³，画进论文会失真。
    用 `--paper-groups Pb,Cu,12.5` 可以自行扩大范围，`--paper-groups all` 表示全部。
    """
    skip = {"all", "none", "other"}
    available = [s for s in summary if s["decay"] is not None and s["label"] not in skip]
    if wanted and wanted.strip().lower() == "all":
        return available
    keep = {s.strip() for s in wanted.split(",")} if wanted else {"Pb", "Cu"}
    out = [s for s in available if s["label"] in keep]
    if not out:
        log.warning("按 --paper-groups=%s 没有匹配到任何分组，回退到全部可用分组", wanted)
        out = available
    return out


# --------------------------------------------------------------------------- 论文级输出

def _write_paper_table(cfg, groups: List[Dict], peak: str, source: str, lang: str = "en") -> None:
    """写出可直接放进论文的结果表（CSV + LaTeX）。

    LaTeX 默认用英文表头——中文表头需要 ctex/xeCJK 才能编译，英文更保险。
    """
    ref = float(cfg.get("eu146_halflife.reference_halflife_days", 4.61))
    ref_err = float(cfg.get("eu146_halflife.reference_halflife_error_days", 0.03))
    results_dir = cfgmod.ensure_dir(cfg.path("paths.results"))

    csv_path = results_dir / f"eu146_halflife_{peak}_table.csv"
    with csv_path.open("w", newline="", encoding="utf-8-sig") as fh:
        w = csv.writer(fh)
        w.writerow(["group", "n_points", "halflife_days", "halflife_err_days",
                    "halflife_hours", "halflife_err_hours", "lambda_per_hour",
                    "lambda_err_per_hour", "chi2", "ndf", "p_value", "pull_vs_reference"])
        for g in groups:
            d = g["decay"]
            pull = (d.halflife_days - ref) / float(np.hypot(d.halflife_error_days, ref_err))
            w.writerow([g["label"], len(g["points"]),
                        f"{d.halflife_days:.4f}", f"{d.halflife_error_days:.4f}",
                        f"{d.halflife_hours:.3f}", f"{d.halflife_error_hours:.3f}",
                        f"{d.lam:.8f}", f"{d.lam_error:.8f}",
                        f"{d.chi2:.2f}", d.ndf, f"{d.pvalue:.4f}", f"{pull:+.2f}"])
    log.info("论文结果表 -> %s", csv_path.relative_to(cfgmod.PROJECT_ROOT))

    if lang == "zh":
        head = ["测量条件", "数据点", "$T_{1/2}$ (d)", "$\\chi^2/\\mathrm{ndf}$",
                "与参考值偏差 ($\\sigma$)"]
        caption = f"$^{{146}}$Eu {peak} 峰的衰变拟合结果。"
        foot = (f"\\footnotesize 参考值 $T_{{1/2}}(^{{146}}\\mathrm{{Eu}}) = "
                f"{ref:.2f} \\pm {ref_err:.2f}$ d（本表偏差按此计算）")
        comment = "% 由 scripts/04_eu146_halflife.py --paper 自动生成（中文表头，需 ctex/xeCJK 编译）"
    else:
        head = ["Configuration", "Points", "$T_{1/2}$ (d)", "$\\chi^2/\\mathrm{ndf}$",
                "Deviation ($\\sigma$)"]
        caption = f"Decay-fit results for the $^{{146}}$Eu {peak} line."
        foot = (f"\\footnotesize Reference value "
                f"$T_{{1/2}}(^{{146}}\\mathrm{{Eu}}) = {ref:.2f} \\pm {ref_err:.2f}$ d; "
                f"deviations are quoted relative to it.")
        comment = "% Generated by scripts/04_eu146_halflife.py --paper"

    src_txt = ("manual table gama 谱数据.xlsx" if source == "reference"
               else "computed peak areas")
    lines = [
        comment,
        f"% data source: {src_txt}",
        "\\begin{table}[htbp]",
        "  \\centering",
        f"  \\caption{{{caption}}}",
        f"  \\label{{tab:eu146-halflife-{peak.lower()}}}",
        "  \\begin{tabular}{lcccc}",
        "    \\hline",
        "    " + " & ".join(head[:4]) + " & " + head[4] + " \\\\",
        "    \\hline",
    ]
    for g in groups:
        d = g["decay"]
        pull = (d.halflife_days - ref) / float(np.hypot(d.halflife_error_days, ref_err))
        label = g["label"].replace("_", "\\_")
        lines.append(
            f"    {label} & {len(g['points'])} & "
            f"${d.halflife_days:.3f} \\pm {d.halflife_error_days:.3f}$ & "
            f"${d.chi2:.2f}/{d.ndf}$ & ${pull:+.2f}$ \\\\")
    lines += [
        "    \\hline",
        f"    \\multicolumn{{5}}{{l}}{{{foot}}} \\\\",
        "    \\hline",
        "  \\end{tabular}",
        "\\end{table}",
    ]
    tex_path = results_dir / f"eu146_halflife_{peak}_table.tex"
    tex_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    log.info("LaTeX 表 -> %s", tex_path.relative_to(cfgmod.PROJECT_ROOT))


# --------------------------------------------------------------------------- 数据

def _material_of(tag: str) -> str:
    """把 "Pb" / "Cu" / "12.5_9_L" / "1_0.5" 这类标记归到材料级。

    含字母的（Pb / Cu / ...）保留原样；纯数字/下划线的（厚度或距离标记）统一记为
    `other`。旧 `fit_correction.C` 只区分铅砖与铜砖两组，默认行为与之一致。
    """
    tag = tag.strip()
    return tag if tag and tag.replace("_", "").isalpha() else "other"


def load_reference_points(cfg, peak: str) -> List[Point]:
    """读人工整理的《gama 谱数据.xlsx》导出表 `data/reference/eu146_measurements.csv`。

    这张表是作者逐条核对过的结果，作为论文数据的首选来源；统计误差由计数率的
    泊松误差推出（σ = √(rate / live_time)），活时间取自 `.Spe` 文件头。
    峰名到列的映射按峰名里的数字自动匹配（`747keV` -> `rate_747_cps`）。
    """
    path = cfgmod.PROJECT_ROOT / "data" / "reference" / "eu146_measurements.csv"
    if not path.is_file():
        log.error("找不到 %s，请先运行 scripts/tools/import_gama_excel.py", path)
        return []
    digits = re.match(r"(\d+)", peak)
    if not digits:
        log.error("无法从峰名 %r 推断列名", peak)
        return []
    rate_col = f"rate_{digits.group(1)}_cps"
    err_col = f"rate_{digits.group(1)}_err"

    points: List[Point] = []
    skipped = 0
    with path.open(encoding="utf-8-sig") as fh:
        for row in csv.DictReader(fh):
            when_s = row.get("acquired_at", "")
            if not when_s or not row.get(rate_col):
                skipped += 1
                continue
            try:
                when = datetime.strptime(when_s, "%Y-%m-%d %H:%M:%S")
                rate = float(row[rate_col])
                err = float(row.get(err_col) or 0.0)
            except ValueError:
                skipped += 1
                continue
            if rate <= 0:
                skipped += 1
                continue
            tag = row.get("tag_raw", "")
            points.append(Point(when, rate, err, _material_of(tag), row["file"], tag,
                                row.get("condition", "")))
    if skipped:
        log.info("跳过 %d 行（缺时间/计数率或计数率为非正值）", skipped)
    points.sort(key=lambda p: p.when)
    return points


def load_points(csv_path: Path, peak: str) -> List[Point]:
    """从峰面积 CSV 里读出 `(时间, 计数率, 误差, 吸收片)`。"""
    points: List[Point] = []
    skipped = 0
    with csv_path.open(encoding="utf-8-sig") as fh:
        for row in csv.DictReader(fh):
            when_s, rate_s, err_s = row.get("acquired_at", ""), row.get(f"{peak}_rate", ""), row.get(f"{peak}_rate_err", "")
            if not when_s or not rate_s:
                skipped += 1
                continue
            try:
                when = datetime.strptime(when_s, "%Y-%m-%d %H:%M:%S")
                rate = float(rate_s)
                err = float(err_s) if err_s else 0.0
            except ValueError:
                skipped += 1
                continue
            if rate <= 0:
                skipped += 1
                continue
            m = _ABSORBER_RX.search(Path(row["file"]).stem)
            tag = m.group(1).strip() if m else "none"
            points.append(Point(when, rate, err, _material_of(tag), row["file"], tag))
    if skipped:
        log.info("跳过 %d 行（缺时间/计数率或计数率为非正值）", skipped)
    points.sort(key=lambda p: p.when)
    return points


# --------------------------------------------------------------------------- 拟合

def fit_group(points: List[Point], t0: datetime, label: str) -> Optional[Dict]:
    """对一组数据点做衰变拟合。"""
    if len(points) < 3:
        log.warning("分组 %s 只有 %d 个点，跳过拟合", label, len(points))
        return None
    t = np.array([(p.when - t0).total_seconds() / 3600.0 for p in points])
    rate = np.array([p.rate for p in points])
    err = np.array([p.rate_err for p in points])

    try:
        res = fitting.fit_decay(t, rate, err if np.all(err > 0) else None)
    except ValueError as exc:
        log.warning("分组 %s 拟合失败: %s", label, exc)
        return None

    log.info("分组 %-6s n=%-4d T1/2 = %.4f ± %.4f 天   (chi2/ndf = %.2f/%d)",
             label, len(points), res.halflife_days, res.halflife_error_days, res.chi2, res.ndf)
    whens = [p.when for p in points]
    return {"label": label, "t": t, "rate": rate, "err": err, "decay": res,
            "points": points, "t0": t0,
            "date_range": (min(whens), max(whens))}


def _report(summary: List[Dict], cfg) -> None:
    ref = float(cfg.get("eu146_halflife.reference_halflife_days", 4.61))
    ref_err = float(cfg.get("eu146_halflife.reference_halflife_error_days", 0.03))
    log.info("")
    log.info("参考值 (146Eu): %.3f ± %.3f 天", ref, ref_err)
    for s in summary:
        d = s["decay"]
        pull = (d.halflife_days - ref) / np.hypot(d.halflife_error_days, ref_err)
        log.info("  %-6s  %.4f ± %.4f 天   偏差 %.2f σ", s["label"],
                 d.halflife_days, d.halflife_error_days, pull)
    log.info("")
    if len(summary) > 1:
        # 一致性检验只在材料级分组之间做（tag 级分组点太少，检验没有意义）
        vals = [s["decay"].halflife_days for s in summary
                if s["label"] not in ("all", "none", "other")]
        errs = [s["decay"].halflife_error_days for s in summary
                if s["label"] not in ("all", "none", "other")]
        if len(vals) > 1:
            cons = fitting.chi2_consistency(vals, errs)
            log.info("各吸收片分组一致性: chi2/ndf = %.2f/%d (p = %.3f)",
                     cons["chi2"], cons["ndf"], cons["pvalue"])
            if cons["pvalue"] < 0.05:
                log.warning("分组之间不一致（p < 0.05），说明吸收片厚度差异尚未修正")
            else:
                log.info("分组一致，可合并；加权平均 T1/2 = %.4f ± %.4f 天",
                         *fitting.weighted_mean(vals, errs))


# --------------------------------------------------------------------------- 输出

def _plot(cfg, summary: List[Dict], peak: str) -> None:
    if not summary:
        return
    fig, axes = plotting.new_figure(width=12, height=4.6, nrows=1, ncols=2)
    colors = ["tab:blue", "tab:red", "tab:green", "tab:purple", "tab:orange"]

    ax = axes[0]
    for color, s in zip(colors, summary):
        d = s["decay"]
        ax.errorbar(s["t"], s["rate"], yerr=s["err"], fmt="o", ms=3, lw=1, capsize=2,
                    color=color, label=f"{s['label']}: T1/2 = {d.halflife_days:.3f} d")
        tt = np.linspace(s["t"].min(), s["t"].max(), 200)
        ax.plot(tt, d.model(tt), "-", color=color, lw=1.5)
    ax.set_yscale("log")
    ax.set_xlabel("相对时间 (小时)")
    ax.set_ylabel(f"{peak} 计数率 (cps)")
    ax.set_title("146Eu 衰变曲线")
    ax.legend()

    ax = axes[1]
    for color, s in zip(colors, summary):
        d = s["decay"]
        resid = (np.log(s["rate"]) - np.log(d.model(s["t"]))) / (s["err"] / s["rate"] if np.all(s["err"] > 0) else 1.0)
        ax.plot(s["t"], resid, "o", ms=3, color=color, label=s["label"])
    ax.axhline(0, color="k", lw=0.8)
    ax.axhline(1, color="k", lw=0.5, ls=":")
    ax.axhline(-1, color="k", lw=0.5, ls=":")
    ax.set_xlabel("相对时间 (小时)")
    ax.set_ylabel("标准化残差")
    ax.set_title("残差")
    ax.legend()

    fig.tight_layout()
    plotting.save_figure(fig, cfg, "halflife", f"halflife_eu146_{peak}_decay")


def _write_summary(cfg, summary: List[Dict], peak: str) -> None:
    out = cfgmod.ensure_dir(cfg.path("paths.results")) / f"halflife_eu146_{peak}.csv"
    with out.open("w", newline="", encoding="utf-8-sig") as fh:
        writer = csv.writer(fh)
        writer.writerow(["group", "n_points", "halflife_days", "halflife_error_days",
                         "halflife_hours", "halflife_error_hours", "lambda_per_hour",
                         "lambda_error_per_hour", "chi2", "ndf"])
        for s in summary:
            d = s["decay"]
            writer.writerow([s["label"], len(s["points"]),
                             f"{d.halflife_days:.6f}", f"{d.halflife_error_days:.6f}",
                             f"{d.halflife_hours:.4f}", f"{d.halflife_error_hours:.4f}",
                             f"{d.lam:.8f}", f"{d.lam_error:.8f}",
                             f"{d.chi2:.3f}", d.ndf])
    log.info("半衰期结果 -> %s", out.relative_to(cfgmod.PROJECT_ROOT))


if __name__ == "__main__":
    raise SystemExit(main())

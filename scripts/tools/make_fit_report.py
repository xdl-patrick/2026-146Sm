#!/usr/bin/env python3
"""把 ¹⁴⁶Eu 的全部拟合图与结果汇总成一个 HTML 报告页。

便于一眼看完全部测量条件的拟合情况（图片 + 结果表 + 残差），
生成的页面用相对路径引用 `figures/`，可以直接双击打开或提交到仓库。

用法
----
    python scripts/tools/make_fit_report.py
    python scripts/tools/make_fit_report.py --peak 747keV --lang zh
"""

import argparse
import csv
import html
import json
import sys
from pathlib import Path
from typing import Dict, List, Optional

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from lib import config as cfgmod
from lib.logutil import add_verbosity_argument, apply_verbosity, get_logger

log = get_logger()


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="生成 146Eu 拟合汇总报告页")
    ap.add_argument("--config", default=None)
    ap.add_argument("--peak", default=None, help="峰名（默认取配置 primary_peak）")
    ap.add_argument("--lang", choices=("zh", "en"), default="zh")
    ap.add_argument("--out", default=None, help="输出 HTML（默认 docs/report_eu146_fits.html）")
    add_verbosity_argument(ap)
    return ap.parse_args()


def read_table(path: Path) -> List[Dict[str, str]]:
    if not path.is_file():
        return []
    with path.open(encoding="utf-8-sig") as fh:
        return list(csv.DictReader(fh))


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    peak = args.peak or cfg.get("gamma.primary_peak", "747keV")
    root = cfgmod.PROJECT_ROOT
    fig_dir = cfg.path("paths.figures") / "halflife"

    ref = float(cfg.get("eu146_halflife.reference_halflife_days", 4.61))
    ref_err = float(cfg.get("eu146_halflife.reference_halflife_error_days", 0.03))
    rows = read_table(cfg.path("paths.results") / f"eu146_halflife_{peak}_table.csv")

    # 每组的单独图（由 04 --paper-per-group 生成）
    per_group = sorted(fig_dir.glob(f"eu146_fit_*_{peak}.png"))
    skip_words = ("allgroups", "grid")
    per_group = [p for p in per_group if not any(w in p.stem for w in skip_words)]

    out = Path(args.out) if args.out else root / "docs" / "report_eu146_fits.html"
    base = out.parent.resolve()

    def rel(p: Path) -> str:
        """相对 HTML 所在目录的路径（HTML 与 figures/ 不在同一层）。"""
        import os
        return Path(os.path.relpath(p.resolve(), base)).as_posix()

    html_parts: List[str] = [
        "<!doctype html>",
        '<html lang="zh">',
        "<head>",
        '<meta charset="utf-8">',
        "<title>¹⁴⁶Eu 半衰期拟合汇总</title>",
        "<style>",
        "  body { font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif; margin: 24px; color: #222; }",
        "  h1 { font-size: 20px; } h2 { font-size: 16px; margin-top: 28px; border-bottom: 1px solid #ddd; padding-bottom: 4px; }",
        "  table { border-collapse: collapse; font-size: 13px; }",
        "  th, td { border: 1px solid #ccc; padding: 4px 10px; text-align: right; }",
        "  th:first-child, td:first-child { text-align: left; }",
        "  th { background: #f3f5f8; }",
        "  .grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(330px, 1fr)); gap: 14px; }",
        "  .card { border: 1px solid #e0e0e0; border-radius: 6px; padding: 8px; background: #fafafa; }",
        "  .card img { width: 100%; display: block; }",
        "  .card .name { font-size: 13px; font-weight: 600; margin-bottom: 4px; }",
        "  .wide img { max-width: 100%; }",
        "  .note { font-size: 12px; color: #666; }",
        "  code { background: #f0f0f0; padding: 1px 4px; border-radius: 3px; }",
        "</style>",
        "</head>",
        "<body>",
        f"<h1>¹⁴⁶Eu {html.escape(peak)} 峰衰变拟合汇总</h1>",
        '<p class="note">数据来源：<code>data/reference/eu146_measurements.csv</code>'
        "（由人工整理的《gama 谱数据.xlsx》转出）。"
        f"参考值 T<sub>1/2</sub>(¹⁴⁶Eu) = {ref:.2f} ± {ref_err:.2f} d。</p>",
    ]

    # 结果表
    if rows:
        html_parts += [
            "<h2>拟合结果</h2>",
            "<table>",
            "<tr><th>测量条件</th><th>数据点</th><th>T<sub>1/2</sub> (d)</th>"
            "<th>λ (1/h)</th><th>χ²/ndf</th><th>p 值</th><th>与参考值偏差 (σ)</th></tr>",
        ]
        for r in rows:
            html_parts.append(
                f"<tr><td>{html.escape(r['group'])}</td><td>{r['n_points']}</td>"
                f"<td>{float(r['halflife_days']):.3f} ± {float(r['halflife_err_days']):.3f}</td>"
                f"<td>{float(r['lambda_per_hour']):.6f}</td>"
                f"<td>{float(r['chi2']):.1f}/{r['ndf']}</td>"
                f"<td>{float(r['p_value']):.3f}</td>"
                f"<td>{float(r['pull_vs_reference']):+.2f}</td></tr>")
        html_parts.append("</table>")

    # 总览图
    html_parts.append("<h2>总览</h2>")
    for name, caption in ((f"eu146_fit_grid_{peak}.png", "全部条件联络表"),
                          (f"eu146_fit_allgroups_{peak}.png", "全部条件叠加")):
        p = fig_dir / name
        if p.is_file():
            html_parts += [
                f'<div class="card wide"><div class="name">{caption}</div>'
                f'<img src="{rel(p)}" alt="{caption}"></div>',
            ]

    # 逐组图
    if per_group:
        html_parts.append("<h2>各测量条件单独拟合</h2>")
        html_parts.append('<div class="grid">')
        for p in per_group:
            label = p.stem.replace(f"eu146_fit_", "").replace(f"_{peak}", "")
            html_parts += [
                f'<div class="card"><div class="name">{html.escape(label)}</div>'
                f'<img src="{rel(p)}" alt="{html.escape(label)}"></div>',
            ]
        html_parts.append("</div>")

    # 修正后数据的 ln(rate) 线性拟合（来自作者整理的《gama 谱数据_v2.xlsx》）
    ln_grid = fig_dir / "eu146_lnfit_reference_grid.png"
    if ln_grid.is_file():
        html_parts.append("<h2>修正后数据：ln(计数率) 线性拟合</h2>")
        html_parts += [
            '<p class="note">数据来源：<code>data/reference/gama_v2/原始数据修正后_tidy.csv</code>'
            "（作者已把 Pb / Cu / 12.5cm 有机玻璃 / 0 / 0_0.5 / 1_0.5 六个阶段的绝对计数率差异修正掉，"
            "所以同一组的 ln(rate) 对时间是一条直线）。</p>",
        ]
        ln_rows = read_table(cfg.path("paths.results") / "eu146_lnfit_reference.csv")
        ln_fits = [r for r in ln_rows if r.get("peak") in ("633keV", "747keV")]
        if ln_fits:
            html_parts += [
                "<table>",
                "<tr><th>数据组</th><th>峰</th><th>点数</th><th>时间范围 (h)</th>"
                "<th>T<sub>1/2</sub> (h)</th><th>T<sub>1/2</sub> (d)</th>"
                "<th>χ²/ndf</th><th>与参考值偏差</th></tr>",
            ]
            for r in ln_fits:
                html_parts.append(
                    f"<tr><td>{html.escape(r['series'])}</td><td>{r['peak']}</td>"
                    f"<td>{r['n_points']}</td>"
                    f"<td>{r['time_min_h']} – {r['time_max_h']}</td>"
                    f"<td>{float(r['halflife_hours']):.2f} ± {float(r['halflife_err_hours']):.2f}</td>"
                    f"<td><b>{float(r['halflife_days']):.3f} ± {float(r['halflife_err_days']):.3f}</b></td>"
                    f"<td>{float(r['chi2']):.1f}/{r['ndf']}</td>"
                    f"<td>{100 * (float(r['halflife_days']) - ref) / ref:+.2f}%</td></tr>")
            html_parts.append("</table>")

        # 加权平均汇总（来自 07 生成的 summary.json）
        ln_sum_path = cfg.path("paths.results") / "eu146_lnfit_reference_summary.json"
        if ln_sum_path.is_file():
            with ln_sum_path.open(encoding="utf-8") as fh:
                ln_sum = json.load(fh)
            html_parts += [
                "<h3>加权平均与一致性检验</h3>",
                "<table>",
                "<tr><th>分组</th><th>合并条数</th><th>T<sub>1/2</sub> (d)</th>"
                "<th>一致性 χ²/ndf</th><th>p 值</th></tr>",
            ]
            def _row(name, g):
                if g.get("chi2") is None:
                    cons = pv = "—"
                else:
                    cons = f"{g['chi2'] / g['ndf']:.2f}/{g['ndf']}"
                    pv = f"{g['pvalue']:.3f}"
                return (f"<tr><td>{html.escape(name)}</td><td>{g['n_fits']}</td>"
                        f"<td>{g['halflife_days']:.3f} ± {g['halflife_err_days']:.3f}</td>"
                        f"<td>{cons}</td><td>{pv}</td></tr>")
            for k, g in ln_sum["by_peak"].items():
                html_parts.append(_row(f"峰 {k}", g))
            for k, g in ln_sum["by_series"].items():
                html_parts.append(_row(f"数据组 {k}", g))
            o = ln_sum["overall"]
            html_parts.append(
                f"<tr style='font-weight:bold'><td>总体加权平均</td><td>{o['n_fits']}</td>"
                f"<td>{o['halflife_days']:.3f} ± {o['halflife_err_days']:.3f}</td>"
                f"<td>{o['chi2'] / o['ndf']:.2f}/{o['ndf']} (p={o['pvalue']:.4f})</td>"
                f"<td>{o['pvalue']:.4f}</td></tr>")
            html_parts.append("</table>")
            html_parts += [
                f'<p class="note"><b>总体结果</b>：T<sub>1/2</sub>(¹⁴⁶Eu) = '
                f'<b>{o["halflife_days"]:.3f} ± {o["halflife_err_days"]:.3f} d</b>，'
                f'比参考值 {ref:.3f} d 偏低 '
                f'{abs(100 * (o["halflife_days"] - ref) / ref):.2f}%。'
                f'<br><b>两个 γ 峰不一致</b>：633 keV 给出 '
                f'{ln_sum["by_peak"]["633keV"]["halflife_days"]:.3f} ± '
                f'{ln_sum["by_peak"]["633keV"]["halflife_err_days"]:.3f} d，747 keV 给出 '
                f'{ln_sum["by_peak"]["747keV"]["halflife_days"]:.3f} ± '
                f'{ln_sum["by_peak"]["747keV"]["halflife_err_days"]:.3f} d，'
                f'相差约 2.5%，而各自统计误差仅 0.004–0.007 d — 这是真实的系统差异'
                f'（峰面积/本底扣除/效率归一不同），投稿前需要排查。</p>',
            ]
        html_parts += [
            f'<div class="card wide"><div class="name">4 组 × 2 个峰</div>'
            f'<img src="{rel(ln_grid)}" alt="ln fit grid"></div>',
        ]
        ln_single = sorted(fig_dir.glob("eu146_lnfit_chamber*_*.png"))
        if ln_single:
            html_parts.append('<div class="grid">')
            for p in ln_single:
                label = p.stem.replace("eu146_lnfit_", "")
                html_parts += [
                    f'<div class="card"><div class="name">{html.escape(label)}</div>'
                    f'<img src="{rel(p)}" alt="{html.escape(label)}"></div>',
                ]
            html_parts.append("</div>")

    html_parts += [
        "<h2>说明</h2>",
        '<p class="note">上排：主图为计数率随时间变化（对数纵轴）与指数拟合，下排为标准化残差。'
        "χ²/ndf 明显大于 1 的分组（如 <code>12.5</code>）说明该标记下混入了不同靶位/距离的测量，"
        "不是一条单一的衰变曲线，需要先按靶号拆分。</p>",
        "</body></html>",
    ]

    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text("\n".join(html_parts), encoding="utf-8")
    log.info("报告页 -> %s", out.relative_to(root))
    log.info("  结果行 %d，逐组图 %d 张", len(rows), len(per_group))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

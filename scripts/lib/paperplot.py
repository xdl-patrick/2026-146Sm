"""论文级衰变曲线作图。

把「散点 + 误差棒 + 指数拟合 + 残差」这套版面集中在这里，供
`04_eu146_halflife.py` 调用，支持三种版式：

* `figure_combined`  —— 所有分组叠在一张图上（带残差面板）
* `figure_one_each`  —— 每个分组单独一张图（带残差面板）
* `figure_grid`      —— 所有分组拼成一张联络表（每格一个分组，便于总览）

版面约定（投稿习惯）：衬线字体、内向刻度、四边刻度、无网格、无边框图例、
单栏宽度 3.45 in。
"""

from contextlib import contextmanager
from typing import Dict, List, Optional

import matplotlib as mpl
import numpy as np

from . import plotting
from .logutil import get_logger

log = get_logger()

# 拟合与数据表里的时间一律以「小时」为单位（`fit_decay` 的入参也是小时），
# 但论文图横轴用「天」更直观，所以绘图时统一除以这个常数。
HOURS_PER_DAY = 24.0

# 分组多的时候循环取用
MARKERS = ["o", "s", "^", "D", "v", "P", "X", "<", ">", "h"]
COLORS = ["#1f4e9c", "#c0392b", "#1e8449", "#8e44ad", "#d35400", "#16a085",
          "#7f8c8d", "#2c3e50", "#a04000", "#117a65"]

LABELS = {
    "zh": {
        "ylabel": "计数率 (s$^{-1}$)",
        "title": "$^{{146}}$Eu 衰变曲线（{peak}）",
        "resid": "标准化残差",
        "xlabel": "相对时间 (d)",
        "half_life": "$T_{1/2}$",
        "chi2": "$\\chi^2/\\mathrm{{ndf}}$",
        "serif": ["SimSun", "Times New Roman", "DejaVu Serif"],
    },
    "en": {
        "ylabel": "Count rate (s$^{-1}$)",
        "title": "$^{{146}}$Eu decay curve ({peak})",
        "resid": "Normalised residual",
        "xlabel": "Elapsed time (d)",
        "half_life": "$T_{1/2}$",
        "chi2": "$\\chi^2/\\mathrm{ndf}$",
        "serif": ["Times New Roman", "DejaVu Serif"],
    },
}


@contextmanager
def paper_style(lang: str = "en"):
    """临时切到论文风格，退出时还原。

    中文图必须把中文字体放在 serif 列表首位：matplotlib 的字体回退链在
    Times New Roman 不含汉字时会出现方块。
    """
    keys = (
        "font.family", "font.serif", "mathtext.fontset", "mathtext.default",
        "axes.grid", "axes.linewidth", "xtick.direction", "ytick.direction",
        "xtick.top", "ytick.right", "xtick.minor.visible", "ytick.minor.visible",
        "legend.frameon", "font.size", "axes.labelsize", "axes.titlesize",
        "xtick.labelsize", "ytick.labelsize", "legend.fontsize",
    )
    saved = {k: mpl.rcParams.get(k) for k in keys}
    labels = LABELS.get(lang, LABELS["en"])
    mpl.rcParams.update({
        "font.family": "serif",
        "font.serif": labels["serif"],
        "mathtext.fontset": "stix",
        "mathtext.default": "regular",
        "axes.grid": False,
        "axes.linewidth": 1.0,
        "xtick.direction": "in", "ytick.direction": "in",
        "xtick.top": True, "ytick.right": True,
        "xtick.minor.visible": True, "ytick.minor.visible": True,
        "legend.frameon": False,
        "font.size": 9, "axes.labelsize": 10, "axes.titlesize": 10,
        "xtick.labelsize": 9, "ytick.labelsize": 9, "legend.fontsize": 8,
    })
    try:
        yield labels
    finally:
        for k, v in saved.items():
            if v is not None:
                mpl.rcParams[k] = v


# --------------------------------------------------------------------------- 绘制

def draw_series(ax, axr, series: Dict, txt: Dict, idx: int,
                with_residuals: bool = True, legend: bool = True,
                show_chi2: bool = True) -> None:
    """在给定坐标轴上画一组数据 + 指数拟合（+ 残差）。"""
    d = series["decay"]
    mk = MARKERS[idx % len(MARKERS)]
    col = COLORS[idx % len(COLORS)]

    t_days = np.asarray(series["t"], dtype=float) / HOURS_PER_DAY
    label = f"{series['label']}: {txt['half_life']} = {d.halflife_days:.2f} $\\pm$ {d.halflife_error_days:.2f} d"
    if show_chi2:
        label += f"  ({txt['chi2']} = {d.chi2_per_ndf:.1f})"
    ax.errorbar(t_days, series["rate"], yerr=series["err"], fmt=mk, ms=3.2, lw=0.9,
                capsize=1.8, elinewidth=0.8, color=col, mec=col, mfc="white",
                label=label if legend else None)
    tt_days = np.linspace(t_days.min(), t_days.max(), 300)
    ax.plot(tt_days, d.model(tt_days * HOURS_PER_DAY), "-", color=col, lw=1.2)

    if with_residuals and axr is not None:
        sigma_ln = np.where(series["err"] > 0, series["err"] / series["rate"], 1.0)
        model = d.model(t_days * HOURS_PER_DAY)
        axr.errorbar(t_days, (np.log(series["rate"]) - np.log(model)) / sigma_ln,
                     yerr=1.0, fmt=mk, ms=2.6, lw=0.7, capsize=1.4, elinewidth=0.7,
                     color=col, mec=col, mfc="white")


def style_main_axis(ax, txt: Dict, peak: str, show_title: bool = True) -> None:
    ax.set_yscale("log")
    ax.set_ylabel(txt["ylabel"])
    if show_title:
        ax.set_title(txt["title"].format(peak=peak), pad=6)


def style_residual_axis(axr, txt: Dict, show_label: bool = True) -> None:
    axr.axhline(0, color="0.35", lw=0.8)
    for y in (1, -1):
        axr.axhline(y, color="0.65", lw=0.6, ls=":")
    axr.set_ylim(-4, 4)
    axr.set_yticks([-3, 0, 3])
    axr.set_xlabel(txt["xlabel"])
    if show_label:
        axr.set_ylabel(txt["resid"])


# --------------------------------------------------------------------------- 版式

def figure_combined(cfg, groups: List[Dict], peak: str, lang: str = "en",
                    name: Optional[str] = None) -> None:
    """所有分组叠在一张图上。"""
    import matplotlib.pyplot as plt

    if not groups:
        return
    # 分组多时图例很大，放在坐标区里必然压到数据；统一摆到图下方两列
    ncol = 2 if len(groups) > 3 else 1
    height = 4.6 + (0.34 * (len(groups) + 1) // ncol)
    with paper_style(lang) as txt:
        fig, (ax, axr) = plt.subplots(
            2, 1, figsize=(3.45, height), sharex=True,
            gridspec_kw={"height_ratios": [3, 1]})
        for i, g in enumerate(groups):
            draw_series(ax, axr, g, txt, i)
        style_main_axis(ax, txt, peak)
        style_residual_axis(axr, txt)
        handles, labels = ax.get_legend_handles_labels()
        fig.legend(handles, labels, loc="lower center", bbox_to_anchor=(0.5, 0.004),
                   ncol=ncol, fontsize=7, handletextpad=0.4, columnspacing=1.0,
                   labelspacing=0.3)
        fig.subplots_adjust(left=0.19, right=0.96, top=0.93, bottom=0.30, hspace=0.09)
        plotting.save_figure(fig, cfg, "halflife",
                             name or f"eu146_fit_allgroups_{peak}",
                             dpi_override=300)


def figure_one_each(cfg, groups: List[Dict], peak: str, lang: str = "en") -> List[str]:
    """每个分组单独一张图，返回文件名列表。"""
    import matplotlib.pyplot as plt

    made = []
    with paper_style(lang) as txt:
        for i, g in enumerate(groups):
            fig, (ax, axr) = plt.subplots(
                2, 1, figsize=(3.45, 4.4), sharex=True,
                gridspec_kw={"height_ratios": [3, 1]})
            draw_series(ax, axr, g, txt, i)
            style_main_axis(ax, txt, peak)
            ax.legend(loc="lower left", handletextpad=0.4, borderaxespad=0.3,
                      numpoints=1, fontsize=7.5)
            style_residual_axis(axr, txt)
            fig.subplots_adjust(left=0.19, right=0.96, top=0.92, bottom=0.11, hspace=0.10)
            key = _safe(g["label"])
            made.append(f"eu146_fit_{key}_{peak}")
            plotting.save_figure(fig, cfg, "halflife",
                                 f"eu146_fit_{key}_{peak}", dpi_override=300)
    return made


def figure_grid(cfg, groups: List[Dict], peak: str, lang: str = "en",
                ncols: int = 3) -> None:
    """所有分组拼成一张联络表：每格一个分组（主图 + 小残差），便于总览。"""
    import matplotlib.pyplot as plt

    n = len(groups)
    if n == 0:
        return
    ncols = max(1, min(ncols, n))
    nrows = int(np.ceil(n / ncols))

    with paper_style(lang) as txt:
        fig, axes = plt.subplots(nrows, ncols, figsize=(3.45 * ncols * 0.92, 2.9 * nrows),
                                 squeeze=False)
        for i, g in enumerate(groups):
            r, c = divmod(i, ncols)
            ax = axes[r][c]
            d = g["decay"]
            draw_series(ax, None, g, txt, i, with_residuals=False,
                        legend=False, show_chi2=False)
            ax.set_yscale("log")
            title = g["label"]
            rng = g.get("date_range")
            if rng:
                title += f"\n{rng[0]:%m-%d} → {rng[1]:%m-%d}"
            ax.set_title(title, fontsize=9.5)
            ax.set_xlabel(txt["xlabel"])
            if c == 0:
                ax.set_ylabel(txt["ylabel"])
            ax.text(0.03, 0.05,
                    f"{txt['half_life']} = {d.halflife_days:.2f} $\\pm$ {d.halflife_error_days:.2f} d\n"
                    f"{txt['chi2']} = {d.chi2_per_ndf:.1f}   (n = {len(g['points'])})",
                    transform=ax.transAxes, fontsize=7.5, va="bottom",
                    bbox=dict(boxstyle="round,pad=0.25", fc="white", ec="0.75", lw=0.6, alpha=0.85))
        for j in range(n, nrows * ncols):
            r, c = divmod(j, ncols)
            axes[r][c].axis("off")
        fig.suptitle(txt["title"].format(peak=peak), fontsize=11, y=0.995)
        fig.subplots_adjust(left=0.06, right=0.985, top=0.90, bottom=0.08,
                            wspace=0.28, hspace=0.38)
        plotting.save_figure(fig, cfg, "halflife", f"eu146_fit_grid_{peak}",
                             dpi_override=300)


def _safe(text: str) -> str:
    """把分组名变成可作文件名的形式。"""
    return "".join(ch if ch.isalnum() or ch in "-._" else "_" for ch in text)

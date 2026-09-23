"""统一绘图与图片归档。

图片规范（由 config/pipeline.json 控制）
---------------------------------------
* 目录：`figures/<类别>/`，类别取 {calibration, alpha, gamma, halflife, migration}
* 格式：每次同时输出矢量 `.pdf` 与预览 `.png`
* 文件名：`<类别>_<对象>_<内容>.pdf`，例如 `gamma_eu146_decay_747keV.pdf`

旧流程把 20 多个 png/pdf 直接丢在项目根目录，无法判断哪张图对应哪一步；
这里强制「先归类、再命名」。
"""

from pathlib import Path
from typing import Optional, Sequence

import matplotlib

matplotlib.use("Agg")  # 无窗口批处理；需要交互时在脚本里手动切回默认后端

import matplotlib.pyplot as plt  # noqa: E402

from .logutil import get_logger  # noqa: E402

log = get_logger()

_CJK_CANDIDATES = ["Microsoft YaHei", "SimHei", "Noto Sans CJK SC", "Source Han Sans SC",
                   "WenQuanYi Zen Hei", "DejaVu Sans"]

CATEGORIES = ("calibration", "alpha", "gamma", "halflife", "migration", "other")


def setup_style() -> str:
    """设置统一风格，并挑一个可用的中文字体；返回选中的字体名。"""
    available = {f.name for f in matplotlib.font_manager.fontManager.ttflist}
    chosen = next((name for name in _CJK_CANDIDATES if name in available), "DejaVu Sans")
    plt.rcParams.update({
        "font.sans-serif": [chosen, "DejaVu Sans"],
        "axes.unicode_minus": False,
        "figure.dpi": 110,
        "savefig.bbox": "tight",
        "axes.grid": True,
        "grid.alpha": 0.3,
        "grid.linestyle": "--",
        "axes.titlesize": 12,
        "axes.labelsize": 11,
        "legend.fontsize": 9,
        "xtick.labelsize": 10,
        "ytick.labelsize": 10,
        "figure.autolayout": False,
    })
    if chosen == "DejaVu Sans":
        log.warning("未找到中文字体，图中中文可能显示为方块（已回退到 DejaVu Sans）")
    return chosen


def figures_dir(cfg, category: str) -> Path:
    """返回并创建 `figures/<category>/`。"""
    if category not in CATEGORIES:
        raise ValueError(f"未知图片类别 {category!r}，合法值为 {CATEGORIES}")
    base = cfg.path("paths.figures")
    out = base / category
    out.mkdir(parents=True, exist_ok=True)
    return out


def save_figure(fig, cfg, category: str, name: str, close: bool = True,
                dpi_override: int = 0) -> Sequence[Path]:
    """按配置把图保存为 pdf + png，返回写出的路径列表。

    `dpi_override` 用于论文图（通常要 300 dpi 以上）。
    """
    formats = cfg.get("plot.formats", ["pdf", "png"])
    dpi = int(dpi_override or cfg.get("plot.png_dpi", 200))
    out_dir = figures_dir(cfg, category)
    written = []
    for fmt in formats:
        path = out_dir / f"{name}.{fmt}"
        fig.savefig(path, dpi=dpi if fmt != "pdf" else None)
        written.append(path)
    if close:
        plt.close(fig)
    log.info("图片已保存: %s", ", ".join(str(p.relative_to(cfg.path('paths.figures').parent)) for p in written))
    return written


def new_figure(width: float = 7.5, height: float = 4.6, nrows: int = 1, ncols: int = 1, **kwargs):
    """创建画布与坐标轴，返回 `(fig, axes)`；单子图时 axes 不是数组。"""
    fig, axes = plt.subplots(nrows, ncols, figsize=(width, height), **kwargs)
    return fig, axes


def annotate_fit(ax, text: str, loc: str = "upper right", fontsize: int = 9) -> None:
    """在图上角标注拟合结果。"""
    ax.text(0.98 if "right" in loc else 0.02, 0.97, text,
            transform=ax.transAxes, ha="right" if "right" in loc else "left",
            va="top", fontsize=fontsize,
            bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.75, edgecolor="0.7"))


def style_spectrum_axes(ax, xlabel: str = "能量 (keV)", ylabel: str = "计数",
                        title: Optional[str] = None, logy: bool = False) -> None:
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    if title:
        ax.set_title(title)
    if logy:
        ax.set_yscale("log")

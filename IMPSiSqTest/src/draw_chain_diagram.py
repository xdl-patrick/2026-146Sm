# -*- coding: utf-8 -*-
#!/usr/bin/env python3
# draw_chain_diagram.py — Detector measurement chain schematic
# Signal chain: 10x10mm Si detector -> SMA -> 40pin aviation (wall)
#              -> DB25 preamp -> Lemo/BNC -> DT5780 -> USB -> PC/CoMPASS

import matplotlib.pyplot as plt
import matplotlib.font_manager as fm

# Use Noto Sans CJK if available, fallback to DejaVu
_cjk_fonts = [f.name for f in fm.fontManager.ttflist
              if 'Noto Sans CJK' in f.name or 'WenQuanYi' in f.name]
FONT_MAIN = _cjk_fonts[0] if _cjk_fonts else 'DejaVu Sans'
print(f'Using font: {FONT_MAIN}')

plt.rcParams['font.sans-serif'] = [FONT_MAIN, 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False
import matplotlib.patches as mpatches
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch, Arc
from matplotlib.path import Path
import matplotlib.patheffects as pe

# ---------------------------------------------------------------------------
# Figure setup
# ---------------------------------------------------------------------------
fig, ax = plt.subplots(figsize=(16, 9))
ax.set_xlim(0, 16)
ax.set_ylim(0, 9)
ax.axis('off')
ax.set_facecolor('white')
fig.patch.set_facecolor('white')

# Colour palette — muted scientific
C_DETECTOR  = '#DDEEFF'   # light blue for detector/chamber interior
C_CHAMBER   = '#EEF5EE'   # very light green
C_CABLE     = '#555555'   # dark grey for cables
C_DT5780    = '#FFE8CC'   # light orange for instrument
C_PC        = '#E8F4E8'   # light green for PC
C_ACCENT    = '#1A6BAA'   # blue for arrows
C_BORDER    = '#333333'   # near-black border

FW = 0.6   # box fill-width
LW = 1.8   # border linewidth


def draw_box(ax, x, y, w, h, label, sublabel=None, fillcolor='white',
             edgecolor=C_BORDER, fontsize=9, radius=0.15):
    """Rounded rectangle with centred text."""
    box = FancyBboxPatch((x - w/2, y - h/2), w, h,
                         boxstyle=f"round,pad=0.04,rounding_size={radius}",
                         facecolor=fillcolor, edgecolor=edgecolor,
                         linewidth=LW, zorder=3)
    ax.add_patch(box)
    if sublabel:
        ax.text(x, y + 0.12, label, ha='center', va='center',
                fontsize=fontsize, fontweight='bold', color=edgecolor, zorder=4)
        ax.text(x, y - 0.22, sublabel, ha='center', va='center',
                fontsize=fontsize - 1.5, color='#555555', zorder=4,
                style='italic')
    else:
        ax.text(x, y, label, ha='center', va='center',
                fontsize=fontsize, fontweight='bold', color=edgecolor,
                multialignment='center', zorder=4)


def draw_arrow(ax, x0, y0, x1, y1, label='', color=C_ACCENT, lw=2.0):
    """Horizontal or vertical arrow, with optional label above."""
    if abs(x1 - x0) > abs(y1 - y0):
        # horizontal
        ax.annotate('', xy=(x1, y0), xytext=(x0, y0),
                    arrowprops=dict(arrowstyle='->', color=color,
                                   lw=lw, shrinkA=3, shrinkB=3),
                    zorder=2)
        if label:
            ax.text((x0 + x1) / 2, y0 + 0.22, label, ha='center',
                    fontsize=7.5, color='#555555', zorder=5)
    else:
        # vertical
        ax.annotate('', xy=(x0, y1), xytext=(x0, y0),
                    arrowprops=dict(arrowstyle='->', color=color,
                                   lw=lw, shrinkA=3, shrinkB=3),
                    zorder=2)
        if label:
            ax.text(x0 + 0.22, (y0 + y1) / 2, label, ha='left',
                    fontsize=7.5, color='#555555', zorder=5)


# ---------------------------------------------------------------------------
# 1. Measurement chamber outline (dashed box)
# ---------------------------------------------------------------------------
chamber = mpatches.Rectangle((0.3, 1.2), 7.2, 6.8,
                              fill=False, edgecolor='#888888',
                              linewidth=1.5, linestyle='--', zorder=1,
                              label='Measurement chamber')
ax.add_patch(chamber)
ax.text(0.55, 7.85, '靶室  Measurement chamber', fontsize=8,
        color='#888888', style='italic')


# ---------------------------------------------------------------------------
# 2. Silicon detector (inside chamber)
# ---------------------------------------------------------------------------
# Draw the square detector symbol
det_x, det_y = 1.5, 4.5
det_w, det_h = 1.2, 1.2
det_rect = mpatches.Rectangle((det_x - det_w/2, det_y - det_h/2),
                               det_w, det_h,
                               facecolor=C_DETECTOR, edgecolor=C_BORDER,
                               linewidth=LW, zorder=3)
ax.add_patch(det_rect)
ax.text(det_x, det_y + 0.05, 'Si', ha='center', va='center',
        fontsize=14, fontweight='bold', color=C_BORDER, zorder=4)
ax.text(det_x, det_y - 0.30, '10×10 mm', ha='center', va='center',
        fontsize=7.5, color='#555555', zorder=4)

# SMA connector nub on the right side of detector
sma_x = det_x + det_w/2
ax.plot([sma_x, sma_x + 0.25], [det_y, det_y],
        color=C_BORDER, lw=LW, zorder=4)
ax.add_patch(mpatches.Circle((sma_x + 0.25, det_y), 0.13,
                               facecolor='white', edgecolor=C_BORDER, lw=1.5, zorder=4))
ax.text(det_x, det_y - 0.65, 'SMA 接口', ha='center', fontsize=7.5,
        color='#555555', zorder=4)

# ---------------------------------------------------------------------------
# 3. SMA-to-DuPont cable (inside chamber)
# ---------------------------------------------------------------------------
cable1_x = sma_x + 0.38
draw_arrow(ax, cable1_x, det_y, cable1_x + 1.6, det_y,
           label='SMA–DuPont 线', color=C_CABLE)

# DuPont connector box
dp_x = cable1_x + 2.2
draw_box(ax, dp_x, det_y, 1.2, 0.75, 'DuPont 公头', '杜邦线缆接头',
          fillcolor='#F5F5F5', fontsize=8)

draw_arrow(ax, dp_x + 0.6, det_y, dp_x + 1.5, det_y,
           label='连接线', color=C_CABLE)

# ---------------------------------------------------------------------------
# 4. 40-pin aviation connector (wall feedthrough)
# ---------------------------------------------------------------------------
av_x = dp_x + 2.9
draw_box(ax, av_x, det_y, 1.35, 1.0,
         '40-pin 航空接头', '40-pin aviation conn.',
         fillcolor='#FFF0E0', fontsize=8, radius=0.1)

# Label on the left side (inside chamber)
ax.text(av_x - 0.82, det_y + 0.32, '舱内 →', ha='right',
        fontsize=7, color='#555555')

# Label on the right side (outside chamber)
ax.text(av_x + 0.82, det_y + 0.32, '→ 舱外', ha='left',
        fontsize=7, color='#555555')

draw_arrow(ax, av_x + 0.68, det_y, av_x + 1.7, det_y,
           label='穿墙线缆', color=C_CABLE)

# ---------------------------------------------------------------------------
# 5. DB25 cable label
# ---------------------------------------------------------------------------
db25_x = av_x + 2.9
draw_box(ax, db25_x, det_y, 1.1, 0.75, 'DB25 线缆', 'preamp connection',
         fillcolor='#F5F5F5', fontsize=8)
ax.text(db25_x, det_y - 0.32, 'DB25 接口', ha='center', fontsize=7,
        color='#555555')

draw_arrow(ax, db25_x + 0.55, det_y, db25_x + 1.5, det_y,
           label='信号线', color=C_CABLE)

# ---------------------------------------------------------------------------
# 6. Charge-sensitive preamplifier (outside chamber)
# ---------------------------------------------------------------------------
preamp_x = db25_x + 2.55
preamp_y = det_y - 0.6   # slightly below main axis to make layout cleaner
draw_box(ax, preamp_x, preamp_y, 2.0, 1.1,
         '电荷灵敏前放', 'Charge-sensitive preamplifier',
         fillcolor='#E8F0FF', fontsize=8)

# Arrow up to main chain level
draw_arrow(ax, preamp_x, preamp_y + 0.55, preamp_x, det_y,
           color=C_CABLE, lw=1.5)
ax.plot([preamp_x - 0.3, preamp_x + 0.3], [preamp_y + 0.55, preamp_y + 0.55],
        color=C_CABLE, lw=1.5, zorder=2)

# Arrow to next stage
draw_arrow(ax, preamp_x + 1.0, preamp_y, preamp_x + 2.5, preamp_y,
           label='LEMO / BNC 线', color=C_CABLE)
ax.plot([preamp_x + 1.0, preamp_x + 1.0], [preamp_y, det_y],
        color='white', lw=3, zorder=2)   # cover the up-arrow stub

# ---------------------------------------------------------------------------
# 7. Lemo / BNC adapter label
# ---------------------------------------------------------------------------
adp_x = preamp_x + 4.0
draw_box(ax, adp_x, preamp_y, 1.5, 0.75,
         'LEMO–BNC 适配器', 'LEMO to BNC adapter',
         fillcolor='#F5F5F5', fontsize=8)

draw_arrow(ax, adp_x + 0.75, preamp_y, adp_x + 2.1, preamp_y,
           label='BNC 线', color=C_CABLE)

# ---------------------------------------------------------------------------
# 8. DT5780 CAEN N957
# ---------------------------------------------------------------------------
dt_x = adp_x + 3.7
dt_y = preamp_y
draw_box(ax, dt_x, dt_y, 2.2, 1.3,
         'DT5780', 'CAEN N957 Desktop Spectrometer',
         fillcolor=C_DT5780, fontsize=9)
ax.text(dt_x, dt_y - 0.35, 'CAEN N957', ha='center', fontsize=7.5,
        color='#555555')
ax.text(dt_x, dt_y - 0.65, '桌面能谱仪', ha='center', fontsize=7,
        color='#777777')

draw_arrow(ax, dt_x + 1.1, dt_y, dt_x + 1.1, dt_y + 1.5,
           label='USB', color=C_CABLE, lw=2.0)
ax.plot([dt_x + 0.3, dt_x + 1.9], [dt_y + 1.5, dt_y + 1.5],
        color=C_CABLE, lw=2.0, zorder=2)

# ---------------------------------------------------------------------------
# 9. PC + CoMPASS
# ---------------------------------------------------------------------------
pc_x = dt_x + 1.1
pc_y = dt_y + 2.8
draw_box(ax, pc_x, pc_y, 2.4, 1.2,
         'PC + CoMPASS', 'Data acquisition & analysis',
         fillcolor=C_PC, fontsize=9)

draw_arrow(ax, pc_x, pc_y - 0.6, pc_x, dt_y + 1.5,
           label='', color=C_CABLE, lw=1.5)

# ---------------------------------------------------------------------------
# Title
# ---------------------------------------------------------------------------
ax.text(8.0, 8.55, '$^{241}$Am Alpha Spectroscopy — Measurement Chain',
        ha='center', va='center', fontsize=13, fontweight='bold',
        color='#1A1A1A', zorder=6)
ax.text(8.0, 8.15, 'Signal path: Si detector → SMA → 40-pin aviation → DB25 → Preamp → DT5780 → USB → PC',
        ha='center', va='center', fontsize=8, color='#555555', zorder=6)

# ---------------------------------------------------------------------------
# Legend entries for cable types
# ---------------------------------------------------------------------------
legend_elements = [
    mpatches.Patch(facecolor='white', edgecolor=C_BORDER,
                   linewidth=LW, label='Component'),
    mpatches.Patch(facecolor=C_DETECTOR, edgecolor=C_BORDER,
                   linewidth=LW, label='Silicon detector'),
    mpatches.Patch(facecolor=C_DT5780, edgecolor=C_BORDER,
                   linewidth=LW, label='DT5780 instrument'),
    mpatches.Patch(facecolor=C_CHAMBER, edgecolor='#888888',
                   linewidth=1.5, linestyle='--', label='Measurement chamber'),
]
ax.legend(handles=legend_elements, loc='lower right',
          fontsize=8, framealpha=0.9, edgecolor=C_BORDER,
          fancybox=True, frameon=True)

# ---------------------------------------------------------------------------
# Save
# ---------------------------------------------------------------------------
plt.tight_layout(pad=0.5)
plt.savefig('plot/detector_chain_diagram.pdf', bbox_inches='tight',
            dpi=150, facecolor='white')
plt.savefig('plot/detector_chain_diagram.png', bbox_inches='tight',
            dpi=150, facecolor='white')
print('Saved: plot/detector_chain_diagram.pdf  plot/detector_chain_diagram.png')

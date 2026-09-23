"""SPE 能谱文件解析、能量刻度与峰面积积分。

对应旧脚本
----------
* `SpeToRoot.C` / `BatchSpeToRoot.C` / `BatchSpeToRoot2024.C` —— 只做「跳过 12 行然后读数」，
  把测量时间、死时间都丢掉了；这里全部解析出来。
* `AnalyzeSPEandROOTFiles.C` / `runFullAnalysis.C` —— 747 / 630 keV 峰的线性本底积分，
  逻辑原样保留，但用 NumPy 取代逐 bin 的 for 循环。
"""

from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Dict, Optional, Tuple

import numpy as np

from .logutil import get_logger

log = get_logger()

# SPE `$DATE_MEA` 行的常见格式，按顺序尝试
_DATE_FORMATS = ("%m/%d/%Y %H:%M:%S", "%m/%d/%Y %H:%M", "%Y-%m-%d %H:%M:%S", "%d/%m/%Y %H:%M:%S")


@dataclass
class SpeSpectrum:
    """一个 Maestro SPE 谱文件。"""

    path: Path
    counts: np.ndarray
    first_channel: int = 0
    spec_id: str = ""
    detector: str = ""
    acquired_at: Optional[datetime] = None
    live_time: float = 0.0
    real_time: float = 0.0
    header: Dict[str, str] = field(default_factory=dict)

    # ---------- 派生量 ----------

    @property
    def n_channels(self) -> int:
        return int(self.counts.size)

    @property
    def dead_time_percent(self) -> float:
        if self.real_time <= 0:
            return 0.0
        return (self.real_time - self.live_time) / self.real_time * 100.0

    @property
    def total_counts(self) -> float:
        return float(self.counts.sum())

    def energy_axis(self, slope_kev_per_channel: float, intercept_kev: float) -> np.ndarray:
        """由道数得到能量轴（keV），返回 bin 中心。"""
        channels = np.arange(self.first_channel, self.first_channel + self.n_channels, dtype=float)
        return slope_kev_per_channel * channels + intercept_kev

    def calibrated_hist(self, slope_kev_per_channel: float, intercept_kev: float,
                        bin_width_kev: float = 0.5) -> Tuple[np.ndarray, np.ndarray]:
        """把原始道谱重分箱到等宽的能量轴，返回 `(counts, edges)`。

        旧脚本 `BatchSpeToRoot.C` 直接沿用 16384 个 bin、把横轴从道数线性映射到能量，
        每个 bin 的宽度因此等于 `slope`（0.2009 keV），在低能端偏粗。这里改成按固定
        keV 宽度重分箱，峰位与峰面积不受影响，但作图更直观。
        """
        emin = slope_kev_per_channel * self.first_channel + intercept_kev
        emax = slope_kev_per_channel * (self.first_channel + self.n_channels) + intercept_kev
        n_bins = max(1, int(round((emax - emin) / bin_width_kev)))
        edges = np.linspace(emin, emax, n_bins + 1)
        centers = 0.5 * (edges[:-1] + edges[1:])
        raw_centers = self.energy_axis(slope_kev_per_channel, intercept_kev)
        # 原始道中心 -> 目标 bin 索引，按 bin 宽度把计数累加进去
        idx = np.clip(np.searchsorted(edges, raw_centers) - 1, 0, n_bins - 1)
        counts = np.bincount(idx, weights=self.counts, minlength=n_bins)[:n_bins]
        return counts, edges

    def measurement_hours(self, reference: Optional[datetime] = None) -> Optional[float]:
        """相对参考时刻的小时数（用于衰变拟合的横轴）。"""
        if self.acquired_at is None or reference is None:
            return None
        return (self.acquired_at - reference).total_seconds() / 3600.0


# --------------------------------------------------------------------------- 解析

def _parse_measure_date(text: str) -> Optional[datetime]:
    text = text.strip()
    for fmt in _DATE_FORMATS:
        try:
            return datetime.strptime(text, fmt)
        except ValueError:
            continue
    log.warning("无法解析测量时间: %r", text)
    return None


def read_spe(path: Path) -> SpeSpectrum:
    """读取一个 SPE 文件（含 `$DATE_MEA` / `$MEAS_TIM` 头信息）。"""
    path = Path(path)
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    header: Dict[str, str] = {}
    date_mea: Optional[datetime] = None
    live_time = real_time = 0.0
    spec_id = detector = ""
    first_channel = 0
    data_start = None

    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if line.startswith("$"):
            key = line.lstrip("$").rstrip(":")
            nxt = lines[i + 1].strip() if i + 1 < len(lines) else ""
            if key == "SPEC_ID":
                spec_id = nxt
                header[key] = nxt
                i += 2
                continue
            if key in ("DATE_MEA", "MEAS_TIM", "DATA", "DETECTOR"):
                header[key] = nxt
                if key == "DATE_MEA":
                    date_mea = _parse_measure_date(nxt)
                elif key == "MEAS_TIM":
                    parts = nxt.split()
                    if len(parts) >= 2:
                        live_time, real_time = float(parts[0]), float(parts[1])
                elif key == "DATA":
                    parts = nxt.split()
                    if parts:
                        first_channel = int(parts[0])
                    data_start = i + 2
                    break
                i += 2
                continue
            i += 1
            continue
        if "DETDESC#" in line:
            detector = line.split("DETDESC#", 1)[1].strip()
        i += 1

    if data_start is None:
        raise ValueError(f"{path.name}: 找不到 $DATA: 段，可能不是 SPE 文件")

    # 数据段之后还可能跟着 `$ROI:` / `$ENER_FIT:` 等小节，遇到 `$` 开头的行即停止
    values: list = []
    for tok in lines[data_start:]:
        stripped = tok.strip()
        if not stripped:
            continue
        if stripped.startswith("$"):
            break
        values.append(float(stripped))
    counts = np.asarray(values, dtype=float)
    if counts.size == 0:
        raise ValueError(f"{path.name}: $DATA: 段为空")

    return SpeSpectrum(
        path=path,
        counts=counts,
        first_channel=first_channel,
        spec_id=spec_id,
        detector=detector,
        acquired_at=date_mea,
        live_time=live_time,
        real_time=real_time,
        header=header,
    )


# --------------------------------------------------------------------------- 峰面积

@dataclass
class PeakArea:
    """一次峰面积积分的结果。"""

    net_counts: float
    net_error: float
    gross_counts: float
    background_counts: float
    n_bins: int
    window: Tuple[float, float]
    live_time: float

    @property
    def count_rate(self) -> float:
        return self.net_counts / self.live_time if self.live_time > 0 else float("nan")

    @property
    def count_rate_error(self) -> float:
        return self.net_error / self.live_time if self.live_time > 0 else float("nan")

    @property
    def valid(self) -> bool:
        return np.isfinite(self.net_counts) and np.isfinite(self.net_error) and self.net_counts > 0


def _bin_index(edges: np.ndarray, energy: float) -> int:
    """等价于 ROOT `TH1::FindBin`：返回 1-based bin 序号（与 edges 的 bin 对齐）。"""
    idx = int(np.searchsorted(edges, energy, side="right"))
    return min(max(idx, 1), edges.size - 1)


def peak_area_linear_bg(centers: np.ndarray, counts: np.ndarray,
                        window: Tuple[float, float], bg_width: float = 4.0,
                        bg_gap: float = 2.0, live_time: float = 0.0) -> PeakArea:
    """线性本底扣除的峰面积积分（沿用 `AnalyzeSPEandROOTFiles.C` 的定义）。

    峰区   : [lo, hi]
    左本底 : [lo - bg_gap - bg_width, lo - bg_gap]
    右本底 : [hi + bg_gap, hi + bg_gap + bg_width]
    本底按左右两侧「每 bin 平均计数」的平均值，乘峰区 bin 数扣除。
    误差取 sqrt(峰区总计数 + 本底计数)，与旧代码一致。
    """
    centers = np.asarray(centers, dtype=float)
    counts = np.asarray(counts, dtype=float)
    if centers.size != counts.size:
        raise ValueError("能量轴与计数长度不一致")

    lo, hi = window
    in_peak = (centers >= lo) & (centers <= hi)
    n_bins = int(in_peak.sum())
    if n_bins == 0:
        raise ValueError(f"峰窗口 {window} 内没有 bin，请检查能量刻度")

    left = (centers >= lo - bg_gap - bg_width) & (centers <= lo - bg_gap)
    right = (centers >= hi + bg_gap) & (centers <= hi + bg_gap + bg_width)

    left_rate = counts[left].mean() if left.any() else 0.0
    right_rate = counts[right].mean() if right.any() else 0.0
    bg_per_bin = 0.5 * (left_rate + right_rate)

    gross = float(counts[in_peak].sum())
    background = bg_per_bin * n_bins
    net = gross - background
    error = float(np.sqrt(max(gross, 0.0) + max(background, 0.0)))

    return PeakArea(
        net_counts=net,
        net_error=error,
        gross_counts=gross,
        background_counts=background,
        n_bins=n_bins,
        window=(float(lo), float(hi)),
        live_time=float(live_time),
    )

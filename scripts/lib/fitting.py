"""拟合模型与工具。

对应旧脚本
----------
* `Cal2026.C` / `cbFit.cc` / `cbFitLinked*.cc` —— Crystal Ball / 高斯峰形拟合
* `fit_correction.C` / `CalculateCalibrationCoefficients.C` —— 线性拟合
* 半衰期步骤 —— `ln(rate) = ln(A0) - λ t`，由斜率得到 `t½ = ln2 / λ`
"""

from dataclasses import dataclass, field
from typing import Callable, Dict, Optional, Sequence, Tuple

import numpy as np
from scipy.optimize import curve_fit

from .logutil import get_logger

log = get_logger()

LN2 = float(np.log(2.0))


@dataclass
class FitResult:
    """一次拟合的结果。"""

    ok: bool
    params: np.ndarray = field(default_factory=lambda: np.array([]))
    errors: np.ndarray = field(default_factory=lambda: np.array([]))
    chi2: float = float("nan")
    ndf: int = 0
    pvalue: float = float("nan")
    model: Optional[Callable] = None
    message: str = ""

    @property
    def chi2_per_ndf(self) -> float:
        return self.chi2 / self.ndf if self.ndf > 0 else float("nan")

    def summary(self, names: Sequence[str]) -> str:
        if not self.ok:
            return f"拟合失败: {self.message}"
        parts = [f"{n} = {v:.6g} ± {e:.3g}" for n, v, e in zip(names, self.params, self.errors)]
        return (f"chi2/ndf = {self.chi2:.2f}/{self.ndf} = {self.chi2_per_ndf:.3f}"
                f"  (p = {self.pvalue:.4f})  |  " + ", ".join(parts))


# --------------------------------------------------------------------------- 峰形

def gaussian(x: np.ndarray, amp: float, mu: float, sigma: float, bg: float = 0.0) -> np.ndarray:
    """高斯 + 常数本底。"""
    return amp * np.exp(-0.5 * ((x - mu) / sigma) ** 2) + bg


def double_gaussian(x: np.ndarray, amp1: float, mu1: float, amp2: float, mu2: float,
                    sigma: float, bg: float = 0.0) -> np.ndarray:
    """双高斯，共享 sigma（对应旧脚本的 `dg` 模型）。"""
    return (amp1 * np.exp(-0.5 * ((x - mu1) / sigma) ** 2)
            + amp2 * np.exp(-0.5 * ((x - mu2) / sigma) ** 2) + bg)


def crystal_ball(x: np.ndarray, amp: float, mu: float, sigma: float,
                 alpha: float, n: float, bg: float = 0.0) -> np.ndarray:
    """Crystal Ball 函数（ROOT `crystalball_function` 同一约定）。

    |t| < alpha 一侧为高斯核，tail 一侧为幂律 `A·(B - t)^(-n)`。
    """
    x = np.asarray(x, dtype=float)
    t = (x - mu) / sigma
    abs_alpha = abs(alpha)
    if abs_alpha == 0 or n == 0:
        return amp * np.exp(-0.5 * t ** 2) + bg
    a = (n / abs_alpha) ** n * np.exp(-0.5 * abs_alpha ** 2)
    b = n / abs_alpha - abs_alpha
    core = np.exp(-0.5 * t ** 2)
    tail = a * np.power(np.clip(b - t, 1e-9, None), -n)
    return amp * np.where(t > -abs_alpha, core, tail) + bg


def double_crystal_ball(x: np.ndarray, amp1: float, mu1: float, amp2: float, mu2: float,
                        sigma: float, alpha: float, n: float, bg: float = 0.0) -> np.ndarray:
    """双 Crystal Ball，共享 sigma/alpha/n（旧脚本的 `dcb` 模型）。"""
    return (crystal_ball(x, amp1, mu1, sigma, alpha, n, 0.0)
            + crystal_ball(x, amp2, mu2, sigma, alpha, n, 0.0) + bg)


# --------------------------------------------------------------------------- 通用拟合

def _run_fit(model: Callable, x: np.ndarray, y: np.ndarray, p0: Sequence[float],
             sigma: Optional[np.ndarray] = None,
             bounds: Optional[Tuple[Sequence[float], Sequence[float]]] = None,
             maxfev: int = 20000) -> FitResult:
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    yerr = None if sigma is None else np.clip(np.asarray(sigma, dtype=float), 1e-12, None)
    try:
        popt, pcov = curve_fit(model, x, y, p0=list(p0), sigma=yerr,
                               bounds=bounds or (-np.inf, np.inf), maxfev=maxfev)
    except Exception as exc:  # noqa: BLE001 - 拟合失败是常态，返回结构化结果
        return FitResult(ok=False, model=model, message=str(exc))

    perr = np.sqrt(np.clip(np.diag(pcov), 0.0, None))
    residual = y - model(x, *popt)
    weights = 1.0 / yerr ** 2 if yerr is not None else np.ones_like(y)
    chi2 = float(np.sum((residual * np.sqrt(weights)) ** 2))
    ndf = max(len(x) - len(popt), 0)
    from scipy.stats import chi2 as chi2_dist
    pvalue = float(chi2_dist.sf(chi2, ndf)) if ndf > 0 else float("nan")

    return FitResult(ok=True, params=np.asarray(popt), errors=perr,
                     chi2=chi2, ndf=ndf, pvalue=pvalue, model=model)


def fit_gaussian(x, y, p0=None, sigma=None, bg: bool = True) -> FitResult:
    """单高斯拟合。p0 = [amp, mu, sigma(, bg)]。"""
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    if p0 is None:
        amp = float(np.max(y)) if y.size else 1.0
        mu = float(x[int(np.argmax(y))]) if y.size else 0.0
        sigma0 = max((x[-1] - x[0]) / 10.0, 1e-6)
        p0 = [amp, mu, sigma0] + ([0.0] if bg else [])
    model = gaussian if bg else (lambda xx, a, m, s: gaussian(xx, a, m, s, 0.0))
    lower = [-np.inf, x.min(), 1e-9] + ([-np.inf] if bg else [])
    upper = [np.inf, x.max(), (x[-1] - x[0])] + ([np.inf] if bg else [])
    return _run_fit(model, x, y, p0, sigma=sigma, bounds=(lower, upper))


def fit_crystal_ball(x, y, p0=None, sigma=None, bg: bool = True) -> FitResult:
    """单 Crystal Ball 拟合。p0 = [amp, mu, sigma, alpha, n(, bg)]。"""
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    if p0 is None:
        amp = float(np.max(y)) if y.size else 1.0
        mu = float(x[int(np.argmax(y))]) if y.size else 0.0
        sigma0 = max((x[-1] - x[0]) / 10.0, 1e-6)
        p0 = [amp, mu, sigma0, 1.0, 1.0] + ([0.0] if bg else [])
    model = crystal_ball if bg else (lambda xx, a, m, s, al, n: crystal_ball(xx, a, m, s, al, n, 0.0))
    lower = [-np.inf, x.min(), 1e-9, 1e-3, 1e-3] + ([0.0] if bg else [])
    upper = [np.inf, x.max(), (x[-1] - x[0]), 100.0, 100.0] + ([np.inf] if bg else [])
    return _run_fit(model, x, y, p0, sigma=sigma, bounds=(lower, upper))


def fit_double_gaussian(x, y, p0, sigma=None, fixed_sigma: Optional[float] = None) -> FitResult:
    """双高斯（共享 sigma）。`fixed_sigma` 给定时 sigma 固定不拟合。"""
    if fixed_sigma is not None:
        model = (lambda xx, a1, m1, a2, m2, bg:
                 double_gaussian(xx, a1, m1, a2, m2, fixed_sigma, bg))
        lower = [-np.inf, x.min(), -np.inf, x.min(), 0.0]
        upper = [np.inf, x.max(), np.inf, x.max(), np.inf]
    else:
        model = double_gaussian
        lower = [-np.inf, x.min(), -np.inf, x.min(), 1e-9, 0.0]
        upper = [np.inf, x.max(), np.inf, x.max(), (x[-1] - x[0]), np.inf]
    return _run_fit(model, x, y, p0, sigma=sigma, bounds=(lower, upper))


# --------------------------------------------------------------------------- 线性 / 衰变

@dataclass
class LinearResult:
    """直线拟合 `y = slope·x + intercept`。"""

    slope: float
    slope_error: float
    intercept: float
    intercept_error: float
    chi2: float
    ndf: int
    covariance: np.ndarray
    model: Optional[Callable] = None


def fit_linear(x, y, yerr=None) -> LinearResult:
    """加权最小二乘直线拟合。"""
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    sigma = None if yerr is None else np.clip(np.asarray(yerr, dtype=float), 1e-12, None)
    popt, pcov = curve_fit(lambda xx, a, b: a * xx + b, x, y, p0=[0.0, float(np.mean(y))],
                           sigma=sigma, maxfev=10000)
    if sigma is not None:
        residual = (y - (popt[0] * x + popt[1])) / sigma
    else:
        residual = y - (popt[0] * x + popt[1])
    chi2 = float(np.sum(residual ** 2))
    return LinearResult(
        slope=float(popt[0]), slope_error=float(np.sqrt(pcov[0, 0])),
        intercept=float(popt[1]), intercept_error=float(np.sqrt(pcov[1, 1])),
        chi2=chi2, ndf=max(len(x) - 2, 0), covariance=pcov,
        model=lambda xx, a=popt[0], b=popt[1]: a * xx + b,
    )


@dataclass
class DecayResult:
    """衰变常数 λ 与半衰期 t½。"""

    lam: float              # 每小时衰变常数
    lam_error: float
    halflife_hours: float
    halflife_error_hours: float
    a0: float
    chi2: float
    ndf: int
    model: Optional[Callable] = None

    @property
    def halflife_days(self) -> float:
        return self.halflife_hours / 24.0

    @property
    def halflife_error_days(self) -> float:
        return self.halflife_error_hours / 24.0

    @property
    def chi2_per_ndf(self) -> float:
        return self.chi2 / self.ndf if self.ndf > 0 else float("nan")

    @property
    def pvalue(self) -> float:
        """拟合优度的 p 值（χ² 分布尾概率）。"""
        from scipy.stats import chi2 as chi2_dist
        return float(chi2_dist.sf(self.chi2, self.ndf)) if self.ndf > 0 else float("nan")


def fit_decay(t_hours, rate, rate_error=None) -> DecayResult:
    """由计数率随时间的变化拟合半衰期。

    拟合 `ln(rate) = ln(A0) - λ·t`，于是 `λ = -slope`，`t½ = ln2/λ`。
    相对误差传递：`σ(t½)/t½ = σ(λ)/λ`。
    """
    t = np.asarray(t_hours, dtype=float)
    r = np.asarray(rate, dtype=float)
    mask = np.isfinite(t) & np.isfinite(r) & (r > 0)
    if mask.sum() < 3:
        raise ValueError(f"有效数据点只有 {int(mask.sum())} 个，无法拟合衰变曲线")
    if mask.sum() != t.size:
        log.warning("剔除了 %d 个无效数据点（计数率 <= 0 或非有限）", int(t.size - mask.sum()))

    t_use, r_use = t[mask], r[mask]
    if rate_error is None:
        yerr = 1.0 / np.sqrt(r_use)                      # Poisson: σ(ln N) = 1/√N
    else:
        yerr = np.clip(np.asarray(rate_error, dtype=float)[mask] / r_use, 1e-9, None)

    linear = fit_linear(t_use, np.log(r_use), yerr)
    lam = -linear.slope
    lam_err = linear.slope_error
    if lam <= 0:
        raise ValueError(f"拟合得到的衰变常数为负（λ = {lam:.4g} /h），数据不呈下降趋势")

    return DecayResult(
        lam=lam,
        lam_error=lam_err,
        halflife_hours=LN2 / lam,
        halflife_error_hours=LN2 / lam ** 2 * lam_err,
        a0=float(np.exp(linear.intercept)),
        chi2=linear.chi2,
        ndf=linear.ndf,
        model=lambda xx, A=np.exp(linear.intercept), L=lam: A * np.exp(-L * xx),
    )


def weighted_mean(values, errors) -> Tuple[float, float]:
    """加权平均及其误差（用于把多块靶/多天的结果合并）。"""
    values = np.asarray(values, dtype=float)
    errors = np.clip(np.asarray(errors, dtype=float), 1e-12, None)
    w = 1.0 / errors ** 2
    mean = float(np.sum(w * values) / np.sum(w))
    return mean, float(np.sqrt(1.0 / np.sum(w)))


def chi2_consistency(values, errors, mean: Optional[float] = None) -> Dict[str, float]:
    """加权平均的一致性检验（χ²/ndf 与 p 值），用来判断能否合并多组结果。"""
    from scipy.stats import chi2 as chi2_dist
    values = np.asarray(values, dtype=float)
    errors = np.clip(np.asarray(errors, dtype=float), 1e-12, None)
    if mean is None:
        mean, _ = weighted_mean(values, errors)
    chi2 = float(np.sum(((values - mean) / errors) ** 2))
    ndf = max(values.size - 1, 0)
    return {"chi2": chi2, "ndf": ndf,
            "chi2_per_ndf": chi2 / ndf if ndf else float("nan"),
            "pvalue": float(chi2_dist.sf(chi2, ndf)) if ndf else float("nan"),
            "mean": mean}

#!/usr/bin/env python3
"""import_legacy_config.py — 从旧 ROOT 宏中抽取硬编码常量，生成配置文件。

背景
----
旧工作流把 4x32 的 ADC 刻度系数和逐道阈值直接写在 `Offline_146Sm2026_*.C` 里，
每次改一个数就要复制一份新的 .C 文件（共 11 个副本）。这里把它们一次性抽出来，
变成 JSON 配置，之后所有脚本都读 JSON，不再重复硬编码。

用法
----
    python scripts/tools/import_legacy_config.py \
        legacy/macros_2026/Offline_146Sm2026_06.C

输出
----
    config/alpha_calibration_2026.json   {slope[4][32], intercept[4][32]}
    config/alpha_thresholds_2026.json    {threshold[4][32], overrides}
"""

import argparse
import json
import re
import sys
from pathlib import Path

# --- 路径基准：本文件位于 <root>/scripts/tools/ 之下 ---
ROOT = Path(__file__).resolve().parents[2]
CONFIG_DIR = ROOT / "config"


def _strip_comments(text: str) -> str:
    """去掉 C/C++ 注释，避免注释里的数字干扰抽取。"""
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    return text


def _find_array(text: str, name: str) -> list:
    """抽取形如 `Float_t Calk[4][32] = { ... };` 的初始化列表。"""
    m = re.search(rf"\b{name}\s*\[[^]]*\]\s*\[[^]]*\]\s*=\s*\{{(.*?)\}}\s*;", text, flags=re.S)
    if not m:
        raise ValueError(f"在源码中找不到数组 {name}")
    body = m.group(1)
    return [float(v) for v in re.findall(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?", body)]


def _reshape(flat: list, n_outer: int, n_inner: int, name: str) -> list:
    expect = n_outer * n_inner
    if len(flat) < expect:
        raise ValueError(f"{name}: 只解析出 {len(flat)} 个数，期望 {expect}")
    if len(flat) > expect:
        # 旧宏里偶尔多写几项，截断并提示，不静默丢弃信息
        print(f"[warn] {name}: 解析出 {len(flat)} 个数，多于 {expect}，取前 {expect} 个",
              file=sys.stderr)
        flat = flat[:expect]
    return [flat[i * n_inner:(i + 1) * n_inner] for i in range(n_outer)]


def _find_threshold_overrides(text: str) -> dict:
    """抽取形如 `thArray[0][2] = 160;` 的逐道阈值覆盖项。"""
    overrides = {}
    for m in re.finditer(r"thArray\s*\[\s*(\d+)\s*\]\s*\[\s*(\d+)\s*\]\s*=\s*(\d+)\s*;", text):
        adc, ch, val = (int(x) for x in m.groups())
        overrides[f"{adc},{ch}"] = val
    return overrides


def main() -> int:
    ap = argparse.ArgumentParser(description="从旧 ROOT 宏抽取刻度系数与阈值")
    ap.add_argument("macro", help="例如 legacy/macros_2026/Offline_146Sm2026_06.C")
    ap.add_argument("--n-adc", type=int, default=4)
    ap.add_argument("--n-ch", type=int, default=32)
    ap.add_argument("--calib-out", default="alpha_calibration_2026.json")
    ap.add_argument("--thresh-out", default="alpha_thresholds_2026.json")
    args = ap.parse_args()

    src = Path(args.macro)
    if not src.is_absolute():
        src = ROOT / src
    if not src.is_file():
        print(f"[error] 找不到宏文件: {src}", file=sys.stderr)
        return 1

    text = _strip_comments(src.read_text(encoding="utf-8", errors="replace"))

    slope = _reshape(_find_array(text, "Calk"), args.n_adc, args.n_ch, "Calk")
    intercept = _reshape(_find_array(text, "Calb"), args.n_adc, args.n_ch, "Calb")
    overrides = _find_threshold_overrides(text)

    # 默认阈值：从 `thArray[i][j] = <val>; // 默认阈值` 那条循环里取
    m = re.search(r"thArray\s*\[\s*i\s*\]\s*\[\s*j\s*\]\s*=\s*(\d+)", text)
    default_th = int(m.group(1)) if m else 180
    thresholds = [[overrides.get(f"{a},{c}", default_th) for c in range(args.n_ch)]
                  for a in range(args.n_adc)]

    # 0 系数的通道说明该道未连接 / 未刻度
    dead = [[a, c] for a in range(args.n_adc) for c in range(args.n_ch)
            if slope[a][c] == 0.0 or intercept[a][c] == 0.0]

    CONFIG_DIR.mkdir(parents=True, exist_ok=True)
    calib_path = CONFIG_DIR / args.calib_out
    thresh_path = CONFIG_DIR / args.thresh_out

    calib_path.write_text(json.dumps({
        "_source": str(src.relative_to(ROOT)) if src.is_relative_to(ROOT) else str(src),
        "_comment": "E[MeV] = slope * ADC + intercept。slope/intercept 为 0 表示该道未刻度。",
        "n_adc": args.n_adc,
        "n_ch": args.n_ch,
        "dead_channels": dead,
        "slope": slope,
        "intercept": intercept,
    }, indent=2) + "\n", encoding="utf-8")

    thresh_path.write_text(json.dumps({
        "_source": str(src.relative_to(ROOT)) if src.is_relative_to(ROOT) else str(src),
        "_comment": "ADC 阈值为原始道数，逐道可覆盖。",
        "n_adc": args.n_adc,
        "n_ch": args.n_ch,
        "default": default_th,
        "threshold": thresholds,
        "overrides": overrides,
    }, indent=2) + "\n", encoding="utf-8")

    print(f"[ok] 刻度系数 -> {calib_path.relative_to(ROOT)}")
    print(f"[ok] 阈值     -> {thresh_path.relative_to(ROOT)}")
    print(f"     未刻度通道数 = {len(dead)} / {args.n_adc * args.n_ch}")
    print(f"     阈值覆盖项   = {len(overrides)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

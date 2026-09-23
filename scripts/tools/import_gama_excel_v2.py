#!/usr/bin/env python3
"""把《gama 谱数据_v2.xlsx》（作者 2026-01 版，7 个工作表）转成论文可引用的规范表。

源文件
------
`data/reference/gama 谱数据_v2.xlsx`
（原路径 `F:\\360MoveData\\Users\\13474\\OneDrive\\桌面\\146Sm伽马谱数据\\gama 谱数据.xlsx`，
2026-01-14，MD5 `48D52B54BC6E6BF697A2F9B8509D2D5E`）。

与 v1（2025-12-25，4 个工作表）相比，v2 的结构信息更全：

| 工作表 | 规模 | 内容 |
|---|---|---|
| `原始数据` | 324×8 | **逐次测量**：`序号 \\| 日期 \\| 靶室号 \\| 文件名 \\| 时间/h \\| 747 计数率 \\| 633/634 计数率 \\| 阶段标记` |
| `原始数据修正后` | 85×12 | **4 个靶室并排**，每块 3 列：`Time/h \\| ln(747 计数率) \\| ln(633/634 计数率)` |
| `修正后伽马谱 (1)…(5)` | 64×30 等 | 每个阶段一份：`Time/h \\| 747 原始 \\| 747 误差 \\| 633 原始 \\| 633 误差 \\| 747ln修正 \\| … \\| 747修正 \\| 633修正` |

关键结构（v1 里看不出来，v2 才明确）
------------------------------------
* `原始数据` 第 3 列是**靶室号 1/2/3/4**（各 80~82 行）——所以 `原始数据修正后` 里
  并排的 4 组就是这 4 个靶室。
* 文件名里 `with` 之后的标记按时间依次是
  `Pb → Cu → 12 → 0 → 0_ → 1_`，即 6 个**连续测量阶段**（吸收片/间隔/距离）。

输出（都在 `data/reference/gama_v2/` 下）
-----------------------------------------
```
sheets/                        7 个工作表的忠实导出，保留 Excel 原始行号
原始数据_tidy.csv              逐次测量长表（日期/靶室/阶段/时间/两个峰的计数率）
原始数据修正后_tidy.csv        4 靶室 × 2 峰的长表（time_h, ln_count_rate）—— 拟合就用它
修正后伽马谱_tidy.csv          5 个阶段表的公共列合并（按表头名匹配，适应不同版式）
tables/原始数据_按阶段汇总.tex   按阶段汇总的 LaTeX 表
```

用法
----
    python scripts/tools/import_gama_excel_v2.py
    python scripts/tools/import_gama_excel_v2.py --xlsx "data/reference/gama 谱数据_v2.xlsx"
"""

import argparse
import csv
import re
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from lib import config as cfgmod
from lib import xlsx
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()

DEFAULT_XLSX = "data/reference/gama 谱数据_v2.xlsx"
OUT_SUBDIR = "gama_v2"

# 原始数据修正后 的 4 个并排列区：每块 3 列
SERIES_BLOCKS = [
    {"series": "chamber1", "start": 0},
    {"series": "chamber2", "start": 3},
    {"series": "chamber3", "start": 6},
    {"series": "chamber4", "start": 9},
]
SERIES_PEAKS = [("747keV", 1), ("633keV", 2)]


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="gama 谱数据_v2.xlsx -> 论文可引用的规范表")
    ap.add_argument("--config", default=None)
    ap.add_argument("--xlsx", default=None, help=f"源表格（默认 {DEFAULT_XLSX}）")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    banner("导入 gama 谱数据_v2.xlsx")

    src = Path(args.xlsx) if args.xlsx else cfgmod.resolve_path(DEFAULT_XLSX)
    if not src.is_file():
        log.error("找不到源表格: %s", src)
        return 1
    log.info("源文件: %s  (%.1f KB)", src, src.stat().st_size / 1024)

    out_dir = cfgmod.ensure_dir(cfgmod.PROJECT_ROOT / "data" / "reference" / OUT_SUBDIR)
    sheet_dir = cfgmod.ensure_dir(out_dir / "sheets")

    sheets = {name: xlsx.read_sheet_indexed(src, name) for name in xlsx.sheet_names(src)}
    log.info("工作表: %s", ", ".join(sheets))

    # ---------- 0) 忠实导出（保留 Excel 行号） ----------
    for name, rows in sheets.items():
        safe = re.sub(r"[^\w\-\.]", "_", name)
        with (sheet_dir / f"{safe}.csv").open("w", encoding="utf-8-sig", newline="") as fh:
            w = csv.writer(fh)
            for row_no, cells in rows:
                w.writerow([row_no] + cells)
    log.info("忠实导出 %d 个工作表 -> %s/", len(sheets), sheet_dir.name)

    # ---------- 1) 原始数据 -> 逐次测量长表 ----------
    raw = _find_sheet(sheets, "原始数据", exact=True)
    if raw is None:
        log.error("缺少『原始数据』工作表")
        return 1
    measurements = _parse_raw(raw, _known_stems(cfg))
    _write_csv(out_dir / "原始数据_tidy.csv", measurements)
    log.info("逐次测量长表 -> 原始数据_tidy.csv (%d 行)", len(measurements))
    _write_campaign_tex(out_dir / "tables" / "原始数据_按阶段汇总.tex", measurements)

    # ---------- 2) 原始数据修正后 -> 4 靶室 × 2 峰 长表 ----------
    corrected = _find_sheet(sheets, "原始数据修正后", exact=True)
    if corrected is None:
        log.error("缺少『原始数据修正后』工作表")
        return 1
    series = _parse_corrected(corrected)
    _write_csv(out_dir / "原始数据修正后_tidy.csv", series)
    per_series = {}
    for r in series:
        per_series[(r["series"], r["peak"])] = per_series.get((r["series"], r["peak"]), 0) + 1
    log.info("修正后长表 -> 原始数据修正后_tidy.csv (%d 行)", len(series))
    log.info("  分布: %s", ", ".join(f"{k[0]}/{k[1]}={v}" for k, v in sorted(per_series.items())))

    # ---------- 3) 修正后伽马谱 (N) -> 公共列合并 ----------
    variants = [(n, rows) for n, rows in sheets.items() if "修正后伽马谱" in n]
    merged = []
    for name, rows in variants:
        merged.extend(_parse_variant(name, rows))
    if merged:
        _write_csv(out_dir / "修正后伽马谱_tidy.csv", merged)
        log.info("阶段表合并 -> 修正后伽马谱_tidy.csv (%d 行, %d 张表)", len(merged), len(variants))
    else:
        log.warning("修正后伽马谱 (N) 没有解析出可用列（版式与预期不符），只保留忠实导出")

    log.info("全部输出在 %s", out_dir.relative_to(cfgmod.PROJECT_ROOT))
    return 0


# --------------------------------------------------------------------------- 解析

def _find_sheet(sheets, keyword: str, exact: bool = False):
    if exact:
        return sheets.get(keyword)
    for name, rows in sheets.items():
        if keyword in name:
            return rows
    return None


def _to_float(text) -> Optional[float]:
    try:
        value = float(str(text).strip())
    except (TypeError, ValueError):
        return None
    return value


def _campaign_of(tag: str) -> str:
    """把文件名标记归到测量阶段（吸收片/间隔/距离）。"""
    tag = tag.strip()
    for key in ("Pb", "Cu"):
        if tag.startswith(key):
            return key
    if tag.startswith("12"):
        return "12.5cm 有机玻璃"
    if tag.startswith("0_"):
        return "0_0.5"
    if tag.startswith("1_"):
        return "1_0.5"
    if tag.startswith("0"):
        return "0"
    return tag or "unknown"


def _known_stems(cfg) -> List[str]:
    """真实原始谱文件名（不带扩展名），用于还原表里被截断的文件名。

    Excel 的『原始数据』表把文件名截断成 `10.18_1_with Pb...`，单看这张表分不清
    `0` 与 `0_0.5`、`1_0.5` 与 `12.5`。用 `data/raw/gamma_eu146_2025/` 里的真实文件名
    做前缀匹配（最长的那个），可以准确还原。
    """
    raw_dir = cfg.path("paths.raw_gamma_eu146")
    if not raw_dir.is_dir():
        log.warning("找不到 %s，无法还原被截断的文件名", raw_dir)
        return []
    return sorted((p.stem for p in raw_dir.glob("*.Spe")), key=len, reverse=True)


def _resolve_name(truncated: str, known: List[str]) -> str:
    """把截断的文件名还原成真实名字（取最长的前缀匹配）。"""
    prefix = truncated.rstrip(".").strip()
    for stem in known:
        if stem.startswith(prefix):
            return stem
    return prefix


def _parse_raw(rows: List[Tuple[int, List[str]]], known: Optional[List[str]] = None) -> List[Dict]:
    """`原始数据` -> 逐次测量记录。

    列布局（`cells`，不含本工具另外写出的 Excel 行号）：
    `日期 | 靶室号 | 文件名 | 时间/h | 747 计数率 | 633/634 计数率 | [阶段标记]`

    表里的文件名是**截断**的（`10.18_1_with Pb...`），先尝试用 `known`（真实文件名）还原，
    还原不了就用截断文本本身。
    """
    known = known or []
    out: List[Dict] = []
    for row_no, cells in rows:
        if len(cells) < 6:
            continue
        shown = cells[2].strip()
        # 文件名被截断，不能用扩展名判断，只认「以日期开头」这一特征
        if not re.match(r"^\d{1,2}\.\d{1,2}", shown):
            continue
        time_h = _to_float(cells[3])
        if time_h is None:
            continue
        full = _resolve_name(shown, known)
        tag = ""
        m = re.search(r"with\s+(.+)$", full)
        if m:
            tag = m.group(1).strip()
        out.append({
            "date": cells[0].strip(),
            "chamber": cells[1].strip(),
            "file": full,
            "file_as_shown": shown,
            "time_h": time_h,
            "rate_747_cps": _to_float(cells[4]),
            "rate_633_cps": _to_float(cells[5]),
            "tag_raw": tag,
            "campaign": _campaign_of(tag),
            "source_row": row_no,
        })
    return out


def _parse_corrected(rows: List[Tuple[int, List[str]]]) -> List[Dict]:
    """`原始数据修正后` -> 4 靶室 × 2 峰的长表。

    表头在第 1-3 行，数据从第 4 行开始；每块 3 列 = (Time/h, ln747, ln633)。
    """
    header_row = None
    data_rows = []
    for row_no, cells in rows:
        if header_row is None:
            if any("Time" in str(c) for c in cells):
                header_row = (row_no, cells)
            continue
        data_rows.append((row_no, cells))
    if header_row is None:
        raise ValueError("原始数据修正后 里找不到表头行")

    out: List[Dict] = []
    for row_no, cells in data_rows:
        for block in SERIES_BLOCKS:
            start = block["start"]
            if len(cells) <= start:
                continue
            time_h = _to_float(cells[start])
            if time_h is None:
                continue
            for peak, offset in SERIES_PEAKS:
                value = _to_float(cells[start + offset]) if len(cells) > start + offset else None
                if value is None:
                    continue
                out.append({
                    "series": block["series"],
                    "peak": peak,
                    "time_h": time_h,
                    "ln_count_rate": value,
                    "source_row": row_no,
                })
    return out


# 修正后伽马谱 (N) 里关心的量：表头关键字 -> 输出名
_VARIANT_FIELDS = [
    ("747原始", "rate_747_cps"),
    ("747误差", "rate_747_err"),
    ("633原始", "rate_633_cps"),
    ("633误差", "rate_633_err"),
    ("747ln修正", "ln_747_corrected"),
    ("633ln修正", "ln_633_corrected"),
    ("747ln", "ln_747_corrected"),
    ("633ln", "ln_633_corrected"),
    ("747修正", "counts_747_corrected"),
    ("633修正", "counts_633_corrected"),
]


def _parse_variant(sheet_name: str, rows: List[Tuple[int, List[str]]]) -> List[Dict]:
    """`修正后伽马谱 (N)` -> 长表。按表头文字定位列，兼容各表不同的列数。"""
    # 表头行：第一行（含 "Time"）
    header_cells = None
    for row_no, cells in rows:
        if any("Time" in str(c) for c in cells):
            header_cells = cells
            break
    if header_cells is None:
        return []

    # 找出所有 "Time/h" 列的起点；取最后一处作为有效数据块（这些表里左块是空的）
    time_cols = [i for i, c in enumerate(header_cells) if str(c).strip() in ("Time/h", "Time")]
    if not time_cols:
        return []
    block_start = time_cols[-1]
    block_end = len(header_cells)

    # 在最后一块里按表头名匹配
    mapping: Dict[int, str] = {}
    for idx in range(block_start, block_end):
        head = str(header_cells[idx]).strip()
        if not head:
            continue
        for key, field in _VARIANT_FIELDS:
            if key in head:
                mapping.setdefault(idx, field)
                break

    if not mapping:
        return []

    out: List[Dict] = []
    for row_no, cells in rows:
        if row_no <= 0:
            continue
        if len(cells) <= block_start:
            continue
        time_h = _to_float(cells[block_start])
        if time_h is None:
            continue
        rec = {"sheet": sheet_name, "time_h": time_h, "source_row": row_no}
        for idx, field in mapping.items():
            rec[field] = _to_float(cells[idx]) if len(cells) > idx else None
        out.append(rec)
    return out


# --------------------------------------------------------------------------- 输出

def _write_csv(path: Path, rows: List[Dict]) -> None:
    if not rows:
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    # 取所有出现过的键，保持首次出现的顺序，缺失值补空
    fields: List[str] = []
    for r in rows:
        for k in r:
            if k not in fields:
                fields.append(k)
    with path.open("w", newline="", encoding="utf-8-sig") as fh:
        w = csv.DictWriter(fh, fieldnames=fields)
        w.writeheader()
        for r in rows:
            w.writerow({k: ("" if r.get(k) is None else r.get(k)) for k in fields})


def _write_campaign_tex(path: Path, measurements: List[Dict]) -> None:
    """按测量阶段汇总的 LaTeX 表（论文里可直接放一张概览表）。"""
    tally: Dict[Tuple[str, str], List[float]] = {}
    for m in measurements:
        tally.setdefault((m["campaign"], m["chamber"]), []).append(m["time_h"])
        tally.setdefault((m["campaign"], "ALL"), []).append(m["time_h"])

    lines = [
        "% 由 scripts/tools/import_gama_excel_v2.py 自动生成",
        "% 数据来源: data/reference/gama 谱数据_v2.xlsx -> 原始数据",
        "\\begin{table}[htbp]",
        "  \\centering",
        "  \\caption{Summary of the $^{146}$Eu HPGe measurement campaigns.}",
        "  \\label{tab:eu146-campaigns}",
        "  \\begin{tabular}{llrrr}",
        "    \\hline",
        "    Campaign & Chamber & Points & $t_{\\min}$ (h) & $t_{\\max}$ (h) \\\\",
        "    \\hline",
    ]
    seen = set()
    for m in measurements:
        key = (m["campaign"], m["chamber"])
        if key in seen:
            continue
        seen.add(key)
        ts = tally[key]
        lines.append(f"    {m['campaign']} & {m['chamber']} & {len(ts)} & "
                     f"{min(ts):.1f} & {max(ts):.1f} \\\\")
    lines += ["    \\hline", "  \\end{tabular}", "\\end{table}"]
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    log.info("按阶段汇总 LaTeX 表 -> %s", path.name)


if __name__ == "__main__":
    raise SystemExit(main())

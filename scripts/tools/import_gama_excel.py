#!/usr/bin/env python3
"""把人工整理的《gama 谱数据.xlsx》转成流程可直接使用的规范表格。

源文件
------
`data/reference/gama 谱数据.xlsx`（原路径
`C:\\Users\\13474\\OneDrive\\xwechat_files\\...\\msg\\file\\2025-12\\gama 谱数据.xlsx`，
2025-12-25）。四个工作表：

| 工作表 | 内容 |
|---|---|
| `原始数据` | **逐次测量**：序号、文件名、747 keV 计数率、633/634 keV 计数率，并附每日平均值行 |
| `原始数据修正后` | 用于 ln 拟合的时间序列：`时间/h, 747 计数, 633 计数` |
| `修正前伽马谱` | 两段（左=铅砖数据、右=铜砖数据）随时间的谱计数率，含图例 |
| `修正后伽马谱` | 同上，另含 ln 修正值与衰变修正后的计数 |

输出
----
* `data/reference/gama_excel_sheets/*.csv` —— 每个工作表的忠实导出（便于人工核对）
* `data/reference/eu146_measurements.csv` —— **逐次测量**的规范表（含测量时刻、活时间、
  统计误差、分组），可直接做散点图与衰变拟合
* `data/reference/eu146_series.csv` —— Excel `修正后` 的时间序列（保留原样）

用法
----
    python scripts/tools/import_gama_excel.py
    python scripts/tools/import_gama_excel.py --xlsx "data/reference/gama 谱数据.xlsx"
"""

import argparse
import csv
import math
import re
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from lib import config as cfgmod
from lib import xlsx
from lib.logutil import add_verbosity_argument, apply_verbosity, banner, get_logger

log = get_logger()

DEFAULT_XLSX = "data/reference/gama 谱数据.xlsx"

# 文件名里 "with" 之后的标记，去掉尾部靶号/左右/时长这类索引
#   12.5_1_L      -> 12.5
#   12.5_9_L_13h  -> 12.5
#   1_0.5         -> 1_0.5   （第二段不是纯数字索引，整体保留）
_TRAILING_INDEX = re.compile(r"^(?P<base>[^_]+)(?:_\d{1,2}(?:_[LR])?(?:_\d+h)?)$")


def normalise_tag(tag: str) -> str:
    """把文件名标记归一到「测量条件」。"""
    tag = tag.strip()
    if not tag:
        return "unknown"
    if tag.replace("_", "").isalpha():
        return tag                      # Pb / Cu 这类材料名
    m = _TRAILING_INDEX.match(tag)
    return m.group("base") if m else tag


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="gama 谱数据.xlsx -> 规范 CSV")
    ap.add_argument("--config", default=None)
    ap.add_argument("--xlsx", default=None, help=f"源表格（默认 {DEFAULT_XLSX}）")
    ap.add_argument("--measurements-csv", default=None,
                    help="逐次测量的峰面积表（默认 data/results/gamma_peak_areas.csv），"
                         "用来补上测量时刻、活时间与统计误差")
    add_verbosity_argument(ap)
    return ap.parse_args()


def main() -> int:
    args = parse_args()
    apply_verbosity(args)
    cfg = cfgmod.load(args.config)
    banner("导入 gama 谱数据.xlsx")

    src = Path(args.xlsx) if args.xlsx else cfgmod.resolve_path(DEFAULT_XLSX)
    if not src.is_file():
        log.error("找不到源表格: %s", src)
        return 1
    log.info("源文件: %s  (%.1f KB, MD5 见 data/reference/README.md)", src, src.stat().st_size / 1024)

    out_dir = cfgmod.ensure_dir(cfgmod.PROJECT_ROOT / "data" / "reference")

    # 1) 忠实导出每个工作表
    sheets = xlsx.read_all(src)
    sheet_dir = cfgmod.ensure_dir(out_dir / "gama_excel_sheets")
    for name, rows in sheets:
        safe = re.sub(r"[^\w\-\.]", "_", name)
        fp = sheet_dir / f"{safe}.csv"
        with fp.open("w", encoding="utf-8-sig", newline="") as fh:
            csv.writer(fh).writerows(rows)
        log.info("工作表 %-12s %4d 行 -> %s", name, len(rows), fp.relative_to(cfgmod.PROJECT_ROOT))

    # 2) `原始数据` -> 逐次测量规范表
    raw_sheet = _find_sheet(sheets, "原始数据")
    if raw_sheet is None:
        log.error("源表格里没有『原始数据』工作表")
        return 1
    probes = _parse_raw_sheet(raw_sheet)
    log.info("从『原始数据』解析出 %d 条逐次测量记录", len(probes))

    aux = _load_aux(args.measurements_csv, cfg)
    log.info("从峰面积表补到 %d 个文件的测量时刻/活时间", len(aux))

    n_written = 0
    n_missing = 0
    rows_out: List[Dict] = []
    for p in probes:
        spe = Path(p["file"]).with_suffix(".Spe").name
        info = aux.get(spe) or aux.get(p["file"])
        if info is None:
            n_missing += 1
        rate747 = p["rate_747"]
        rate633 = p["rate_633"]
        live = info["live_time"] if info else float("nan")
        rows_out.append({
            "day_index": p["day_index"],
            "file": p["file"],
            "condition": normalise_tag(p["tag"]),
            "tag_raw": p["tag"],
            "acquired_at": info["acquired_at"] if info else "",
            "live_time_s": f"{live:.1f}" if live == live else "",
            "rate_747_cps": f"{rate747:.6f}" if rate747 is not None else "",
            "rate_747_err": f"{math.sqrt(rate747 / live):.6f}" if (rate747 and live == live and live > 0) else "",
            "rate_633_cps": f"{rate633:.6f}" if rate633 is not None else "",
            "rate_633_err": f"{math.sqrt(rate633 / live):.6f}" if (rate633 and live == live and live > 0) else "",
        })
        n_written += 1

    meas_csv = out_dir / "eu146_measurements.csv"
    with meas_csv.open("w", encoding="utf-8-sig", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(rows_out[0].keys()))
        w.writeheader()
        w.writerows(rows_out)
    log.info("逐次测量表 -> %s (%d 行)", meas_csv.relative_to(cfgmod.PROJECT_ROOT), n_written)

    by_cond: Dict[str, int] = {}
    for r in rows_out:
        by_cond[r["condition"]] = by_cond.get(r["condition"], 0) + 1
    log.info("测量条件分布: %s", ", ".join(f"{k}({v})" for k, v in sorted(by_cond.items())))
    if n_missing:
        log.warning("%d 条记录在峰面积表里找不到对应文件，测量时刻与误差留空", n_missing)

    # 3) 时间序列（`原始数据修正后` 才是干净的 (t, 747, 633) 三列）
    series = _find_sheet(sheets, "原始数据修正后", exact=True)
    if series is None:
        series = _find_sheet(sheets, "修正后")
    if series is not None:
        blocks = _split_blocks(series)
        ser_csv = out_dir / "eu146_series.csv"
        n = 0
        with ser_csv.open("w", encoding="utf-8-sig", newline="") as fh:
            w = csv.writer(fh)
            w.writerow(["block", "time_h", "counts_747", "counts_633"])
            for b, block in enumerate(blocks, start=1):
                for trig in block:
                    w.writerow([b, *trig])
                    n += 1
        log.info("时间序列表 -> %s (%d 行, %d 段)", ser_csv.relative_to(cfgmod.PROJECT_ROOT), n, len(blocks))
    return 0


# --------------------------------------------------------------------------- 解析

def _find_sheet(sheets, keyword: str, exact: bool = False):
    """按表名查找工作表。`exact=True` 时要求完全同名。

    注意：「修正后伽马谱」也包含子串「修正后」，所以取干净的 (t, 747, 633)
    三列表时必须用 exact 指定 `原始数据修正后`。
    """
    if exact:
        for name, rows in sheets:
            if name == keyword:
                return rows
        return None
    for name, rows in sheets:
        if keyword in name:
            return rows
    return None


def _parse_raw_sheet(rows: List[List[str]]) -> List[Dict]:
    """『原始数据』表 -> `[{day_index, file, tag, rate_747, rate_633}, ...]`。

    表格结构：

    ```
    1           | 10.18_1_with Pb.root | 13.375 | 3.548     <- 序号在测量行上
    1           | 10.18_2_with Pb.root | 14.231 | 3.864
    ...
    1 平均值     |                      | 14.0157| 3.977     <- 每日平均值行，跳过
    2           | 10.20_1_with Pb.root | 9.833  | 3.220
    ```
    """
    out: List[Dict] = []
    day_index = ""
    for row in rows:
        if len(row) < 2:
            continue
        cell0 = row[0].strip()
        name = row[1].strip()

        if "平均值" in cell0:
            continue                              # 汇总行，不参与逐次测量
        if re.fullmatch(r"\d+", cell0):
            day_index = cell0                     # 序号写在测量行上
        if not name.endswith(".root"):
            continue

        m = re.search(r"with\s+(.+)$", Path(name).stem)
        out.append({
            "day_index": day_index,
            "file": name,
            "tag": m.group(1).strip() if m else "",
            "rate_747": _to_float(row[2] if len(row) > 2 else ""),
            "rate_633": _to_float(row[3] if len(row) > 3 else ""),
        })
    return out


def _to_float(text: str) -> Optional[float]:
    try:
        return float(str(text).strip())
    except (TypeError, ValueError):
        return None


def _split_blocks(rows: List[List[str]]) -> List[List[Tuple[float, float, float]]]:
    """把 `(time, 747, 633)` 序列按「时间不再单调递增」切成多段。"""
    points: List[Tuple[float, float, float]] = []
    for row in rows:
        if len(row) < 3:
            continue
        t, a, b = _to_float(row[0]), _to_float(row[1]), _to_float(row[2])
        if t is None or a is None or b is None:
            continue
        points.append((t, a, b))

    blocks: List[List[Tuple[float, float, float]]] = []
    current: List[Tuple[float, float, float]] = []
    prev = None
    for p in points:
        if prev is not None and p[0] <= prev:
            blocks.append(sorted(current))
            current = []
        current.append(p)
        prev = p[0]
    if current:
        blocks.append(sorted(current))
    return [b for b in blocks if len(b) >= 3]


def _load_aux(path: Optional[str], cfg) -> Dict[str, Dict]:
    """从峰面积表读 `文件名 -> {acquired_at, live_time}`。"""
    csv_path = Path(path) if path else cfg.path("paths.results") / "gamma_peak_areas.csv"
    if not csv_path.is_file():
        log.warning("找不到 %s，测量时刻与活时间将留空（可先运行 03_gamma_peak_area.py）", csv_path)
        return {}
    out: Dict[str, Dict] = {}
    with csv_path.open(encoding="utf-8-sig") as fh:
        for row in csv.DictReader(fh):
            try:
                live = float(row.get("live_time_s") or "nan")
            except ValueError:
                live = float("nan")
            out[row["file"]] = {"acquired_at": row.get("acquired_at", ""), "live_time": live}
    return out


if __name__ == "__main__":
    raise SystemExit(main())

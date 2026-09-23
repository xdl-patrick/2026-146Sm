"""最小化的 xlsx 读取（不依赖 openpyxl / pandas）。

为什么自己写？
--------------
本机 Python 3.9 里没有 openpyxl / xlrd，而 xlsx 本质上只是一个 ZIP + XML。
实验记录类的表格又必须被流程稳定读到（`.Spe` 数据、人工整理的结果表），
所以这里放一个 100 行左右的只读解析器，避免给项目增加安装依赖。

支持：sharedStrings、inlineStr、数值、公式缓存值（取的是 v，即上次计算结果）。
不支持：合并单元格的还原、单元格样式、日期格式转换（日期会以序列号返回）。
"""

import re
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path
from typing import Dict, List, Tuple

_MAIN = "{http://schemas.openxmlformats.org/spreadsheetml/2006/main}"
_NS = {"m": _MAIN[1:-1]}


def _col_index(cell_ref: str) -> int:
    """`C12` -> 2（0 基列号）。"""
    letters = re.match(r"([A-Z]+)", cell_ref).group(1)
    n = 0
    for ch in letters:
        n = n * 26 + (ord(ch) - 64)
    return n - 1


def sheet_names(path: Path) -> List[str]:
    """按工作簿中的顺序返回工作表名。"""
    with zipfile.ZipFile(path) as z:
        wb = ET.fromstring(z.read("xl/workbook.xml"))
    return [s.get("name") for s in wb.findall(".//m:sheet", _NS)]


def read_sheet(path: Path, sheet: str) -> List[List[str]]:
    """读取单个工作表，返回「行 -> 单元格字符串」的二维列表（空单元格为 ''）。"""
    with zipfile.ZipFile(path) as z:
        shared: List[str] = []
        if "xl/sharedStrings.xml" in z.namelist():
            root = ET.fromstring(z.read("xl/sharedStrings.xml"))
            for si in root.findall("m:si", _NS):
                shared.append("".join(t.text or "" for t in si.iter(_MAIN + "t")))

        wb = ET.fromstring(z.read("xl/workbook.xml"))
        names = [s.get("name") for s in wb.findall(".//m:sheet", _NS)]
        if sheet not in names:
            raise KeyError(f"{Path(path).name} 中没有工作表 {sheet!r}，可选: {names}")
        # sheetN.xml 的编号与工作簿顺序一致
        target = f"xl/worksheets/sheet{names.index(sheet) + 1}.xml"
        if target not in z.namelist():
            raise FileNotFoundError(f"{Path(path).name}: 缺少 {target}")
        root = ET.fromstring(z.read(target))

    rows: List[List[str]] = []
    for row in root.iter(_MAIN + "row"):
        cells: Dict[int, str] = {}
        for c in row.findall("m:c", _NS):
            ctype = c.get("t")
            if ctype == "inlineStr":
                node = c.find("m:is", _NS)
                value = "".join(t.text or "" for t in node.iter(_MAIN + "t")) if node is not None else ""
            else:
                v = c.find("m:v", _NS)
                if v is None or v.text is None:
                    continue
                value = shared[int(v.text)] if ctype == "s" else v.text
            cells[_col_index(c.get("r"))] = value
        if cells:
            rows.append([cells.get(j, "") for j in range(max(cells) + 1)])
    return rows


def read_all(path: Path) -> List[Tuple[str, List[List[str]]]]:
    """读取全部工作表，返回 `[(表名, 行列表), ...]`。"""
    return [(name, read_sheet(path, name)) for name in sheet_names(path)]


def read_sheet_indexed(path: Path, sheet: str) -> List[Tuple[int, List[str]]]:
    """读取单个工作表，**保留 Excel 里的真实行号**，返回 `[(行号, 单元格), ...]`。

    为什么需要这个？
    ----------------
    人工维护的表经常用「空行」分隔多段数据，或者把几条时间序列并排放在同一张表的不同
    列区（例如《gama 谱数据》里的 4 个靶室）。`read_sheet` 会跳过空行、丢掉行号，
    多段数据就会被拼成一条看起来乱序的序列。这个函数把行号带出来，便于：
    * 识别空行分隔的段；
    * 把并排的多个列区各自切开。
    """
    with zipfile.ZipFile(path) as z:
        shared: List[str] = []
        if "xl/sharedStrings.xml" in z.namelist():
            root = ET.fromstring(z.read("xl/sharedStrings.xml"))
            for si in root.findall("m:si", _NS):
                shared.append("".join(t.text or "" for t in si.iter(_MAIN + "t")))
        wb = ET.fromstring(z.read("xl/workbook.xml"))
        names = [s.get("name") for s in wb.findall(".//m:sheet", _NS)]
        if sheet not in names:
            raise KeyError(f"{Path(path).name} 中没有工作表 {sheet!r}，可选: {names}")
        target = f"xl/worksheets/sheet{names.index(sheet) + 1}.xml"
        root = ET.fromstring(z.read(target))

    rows: List[Tuple[int, List[str]]] = []
    for row in root.iter(_MAIN + "row"):
        row_no = int(row.get("r"))
        cells: Dict[int, str] = {}
        for c in row.findall("m:c", _NS):
            ctype = c.get("t")
            if ctype == "inlineStr":
                node = c.find("m:is", _NS)
                value = "".join(t.text or "" for t in node.iter(_MAIN + "t")) if node is not None else ""
            else:
                v = c.find("m:v", _NS)
                if v is None or v.text is None:
                    continue
                value = shared[int(v.text)] if ctype == "s" else v.text
            cells[_col_index(c.get("r"))] = value
        rows.append((row_no, [cells.get(j, "") for j in range(max(cells) + 1)] if cells else []))
    return rows


def read_sheet_with_rowmap(path: Path, sheet: str) -> Dict[int, List[str]]:
    """同上，但返回 `{行号: 单元格}`，便于按行号直接取。"""
    return dict(read_sheet_indexed(path, sheet))

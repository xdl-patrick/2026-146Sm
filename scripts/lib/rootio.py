"""ROOT 文件 I/O（基于 uproot）。

为什么用 uproot？
-----------------
旧流程依赖 WSL 里的 ROOT（`root -l -q 'macro.C(...)'`），每个步骤都要起一个
ROOT 进程、写一个 .C 文件。改用 uproot 后整条流程可以在一份 Python 里串起来，
`run_all.py` 一键跑通；ROOT 6.32 仍然可用（见 README），只是不再是必需依赖。

约定
----
- 只读用法：`with open_root(p) as f: ...`
- 写直方图：`write_hists(p, {"h": (values, edges)}, titles={...})`
"""

import re
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple

import numpy as np
import uproot

from .logutil import get_logger

log = get_logger()

HistArrays = Tuple[np.ndarray, np.ndarray]  # (bin contents, bin edges)


# --------------------------------------------------------------------------- 读

def open_root(path: Path, **kwargs):
    """打开 ROOT 文件（调用方负责关闭，推荐用 with 语句）。"""
    path = Path(path)
    if not path.is_file():
        raise FileNotFoundError(f"ROOT 文件不存在: {path}")
    return uproot.open(str(path), **kwargs)


def object_names(root_file, classname: Optional[str] = None) -> List[str]:
    """列出文件内对象名（去掉 `;1` 版本后缀）。找不到文件时不抛错，返回空列表。"""
    names = []
    classnames = root_file.classnames()
    for key, klass in classnames.items():
        if classname and not klass.startswith(classname):
            continue
        names.append(key.split(";")[0])
    return names


def read_hist(root_file, name: str) -> Optional[HistArrays]:
    """读取 TH1，返回 `(counts, edges)`；对象不存在返回 None。"""
    try:
        hist = root_file[name]
    except KeyError:
        return None
    axis = hist.axis()
    return np.asarray(hist.values(), dtype=float), np.asarray(axis.edges(), dtype=float)


def read_hist_with_errors(root_file, name: str) -> Optional[Tuple[np.ndarray, np.ndarray, np.ndarray]]:
    """读取 TH1，返回 `(counts, errors, edges)`。"""
    try:
        hist = root_file[name]
    except KeyError:
        return None
    values = np.asarray(hist.values(), dtype=float)
    variances = np.asarray(hist.variances(), dtype=float)
    edges = np.asarray(hist.axis().edges(), dtype=float)
    # 未 fill 过 error 的直方图 variances 可能为 None
    errors = np.sqrt(np.clip(variances, 0.0, None)) if variances is not None else np.sqrt(np.abs(values))
    return values, errors, edges


def tree_branches(root_file, tree: str) -> List[str]:
    """列出 TTree 的分支名；树不存在时返回空列表。"""
    for key in root_file.keys():
        if key.split(";")[0] == tree:
            return list(root_file[key].keys())
    return []


def read_tree(root_file, tree: str, branches: Optional[Sequence[str]] = None,
              entry_stop: Optional[int] = None,
              missing: str = "zero") -> Dict[str, np.ndarray]:
    """把 TTree 的分支读成 `{分支名: np.ndarray}`。

    旧脚本用 `SetBranchAddress` + `GetEntry` 逐条循环，128 个分支要写 128 行；
    这里一次读入整棵树的数组，后续用 NumPy 向量化处理。

    `missing="zero"` 时，请求但文件中不存在的分支会用全 0 数组补齐——不同 run 的
    通道数并不一致（128 / 144 / 36 / 4），这样缺通道的 run 也能跑，只是那些面没有计数。
    """
    present = tree_branches(root_file, tree)
    if not present:
        raise KeyError(f"文件中没有 TTree '{tree}'")
    wanted = list(branches) if branches else present
    available = [b for b in wanted if b in present]
    if not available:
        raise KeyError(f"TTree '{tree}' 里没有任何请求的分支（文件里有 {len(present)} 个）")

    obj = root_file[tree]
    arrays = obj.arrays(available, library="np", entry_stop=entry_stop)
    if not isinstance(arrays, dict):
        arrays = {name: np.asarray(arrays[name]) for name in available}
    out = {k: np.asarray(v) for k, v in arrays.items()}

    absent = [b for b in wanted if b not in present]
    if absent:
        if missing != "zero":
            raise KeyError(f"TTree '{tree}' 缺少分支: {absent[:8]}{' ...' if len(absent) > 8 else ''}")
        n = len(next(iter(out.values()))) if out else 0
        for name in absent:
            out[name] = np.zeros(n, dtype=np.uint32)
        log.debug("TTree %s: %d 个分支缺失，已用 0 补齐", tree, len(absent))
    return {name: out[name] for name in wanted}


# --------------------------------------------------------------------------- 写

def write_hists(path: Path, hists: Dict[str, HistArrays]) -> Path:
    """把 `{名字: (counts, edges)}` 写成 TH1D 存到 ROOT 文件。

    等价于旧脚本的 `TH1F *h = new TH1F(...); ... h->Write();`，但不必为每个通道
    复制一段代码。uproot 的写入接口只接受 `(values, edges)` 元组，不带标题——
    标题/轴标签统一由绘图层（`plotting.py` / `figures/`）负责。
    """
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with uproot.recreate(str(path)) as out:
        for name, (values, edges) in hists.items():
            values = np.asarray(values, dtype=float)
            edges = np.asarray(edges, dtype=float)
            if values.size != edges.size - 1:
                raise ValueError(
                    f"直方图 {name}: counts 长度 {values.size} 与 edges 长度 {edges.size} 不匹配"
                )
            out[name] = (values, edges)
    return path


# --------------------------------------------------------------------------- 目录

def find_run_files(folder: Path, pattern: str, max_files: int = 0) -> List[Path]:
    """按正则（在文件名上去掉扩展名后匹配）查找 run 文件，按 run 号自然排序。

    `pattern` 例：`^2026_146Sm(\\d+)$`。捕获组 1 若不是数字则退回按文件名排序。
    """
    folder = Path(folder)
    if not folder.is_dir():
        log.warning("目录不存在，跳过: %s", folder)
        return []
    rx = re.compile(pattern)
    hits: List[Tuple[int, str, Path]] = []
    for p in sorted(folder.glob("*.root")):
        m = rx.match(p.stem)
        if not m:
            continue
        try:
            key = int(m.group(1))
        except (IndexError, ValueError):
            key = 0
        hits.append((key, p.name, p))
    hits.sort(key=lambda t: (t[0], t[1]))
    files = [h[2] for h in hits]
    if max_files and max_files > 0:
        files = files[:max_files]
    return files


def run_number(path: Path, pattern: str = r"(\d+)") -> Optional[int]:
    """从文件名里抽出 run 号。"""
    matches = re.findall(pattern, Path(path).stem)
    return int(matches[-1]) if matches else None


def safe_close(obj) -> None:
    """安静关闭 uproot 文件对象。"""
    try:
        obj.close()
    except Exception:  # noqa: BLE001 - 关闭失败不应影响主流程
        pass


def iter_channel_names(n_adc: int = 4, n_ch: int = 32) -> Iterable[str]:
    """生成 `adc0ch0 ... adc3ch31` 这种通道名序列。"""
    for adc in range(n_adc):
        for ch in range(n_ch):
            yield f"adc{adc}ch{ch}"

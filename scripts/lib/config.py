"""统一配置读取。

所有脚本不再硬编码路径 / 系数 / 阈值，一律通过这里加载 `config/pipeline.json`。
配置里的相对路径统一按「项目根目录」解析，因此无论在哪个目录下运行脚本都一致。
"""

import json
from pathlib import Path
from typing import Any, Dict, Optional

# scripts/lib/config.py -> parents[2] == 项目根目录
PROJECT_ROOT: Path = Path(__file__).resolve().parents[2]
CONFIG_DIR: Path = PROJECT_ROOT / "config"
DEFAULT_CONFIG_FILE: Path = CONFIG_DIR / "pipeline.json"


class Config:
    """`pipeline.json` 的轻量包装，支持点号路径取值与相对路径解析。"""

    def __init__(self, data: Dict[str, Any], source: Path) -> None:
        self._data = data
        self.source = source

    # ---------- 取值 ----------

    def get(self, dotted: str, default: Any = None) -> Any:
        """按 `a.b.c` 取值；中间缺失即返回 default。"""
        node: Any = self._data
        for key in dotted.split("."):
            if not isinstance(node, dict) or key not in node:
                return default
            node = node[key]
        return node

    def require(self, dotted: str) -> Any:
        """取值，缺失即抛错（配置写错时尽早暴露，而不是静默用默认值）。"""
        sentinel = object()
        value = self.get(dotted, sentinel)
        if value is sentinel:
            raise KeyError(f"配置 {self.source.name} 缺少必需项: {dotted}")
        return value

    def section(self, name: str) -> Dict[str, Any]:
        value = self.get(name, {})
        if not isinstance(value, dict):
            raise TypeError(f"配置项 {name} 应为对象，实际为 {type(value).__name__}")
        return value

    # ---------- 路径 ----------

    def path(self, dotted: str, default: Optional[str] = None) -> Path:
        """取配置中的相对路径并解析为绝对路径（不从磁盘校验存在性）。"""
        raw = self.get(dotted, default)
        if raw is None:
            raise KeyError(f"配置 {self.source.name} 缺少路径项: {dotted}")
        return resolve_path(raw)

    def __getitem__(self, key: str) -> Any:
        return self._data[key]

    def __repr__(self) -> str:  # pragma: no cover - 仅用于调试
        return f"<Config {self.source.name} keys={list(self._data)}>"


def resolve_path(raw: str) -> Path:
    """把配置里的相对路径解析成绝对路径；已是绝对路径则原样返回。"""
    p = Path(raw)
    return p if p.is_absolute() else (PROJECT_ROOT / p)


def load(config_file: Optional[Path] = None) -> Config:
    """加载主配置。"""
    path = Path(config_file) if config_file else DEFAULT_CONFIG_FILE
    if not path.is_absolute():
        path = PROJECT_ROOT / path
    if not path.is_file():
        raise FileNotFoundError(f"找不到配置文件: {path}")
    with path.open(encoding="utf-8") as fh:
        return Config(json.load(fh), path)


def load_json(name: str) -> Dict[str, Any]:
    """加载 `config/` 下的任意 JSON 文件（按文件名或相对路径）。"""
    path = Path(name)
    if not path.is_absolute():
        path = (path if path.parent != Path(".") else CONFIG_DIR / path)
    if not path.is_file():
        raise FileNotFoundError(f"找不到配置: {path}")
    with path.open(encoding="utf-8") as fh:
        return json.load(fh)


def save_json(name: str, data: Dict[str, Any]) -> Path:
    """把结果写回 `config/`（例如刻度系数、阈值）。"""
    path = Path(name)
    if not path.is_absolute():
        path = path if path.parent != Path(".") else CONFIG_DIR / path
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as fh:
        json.dump(data, fh, indent=2, ensure_ascii=False)
        fh.write("\n")
    return path


def ensure_dir(path: Path) -> Path:
    """确保目录存在并返回它。"""
    path.mkdir(parents=True, exist_ok=True)
    return path

"""统一日志。

旧脚本用 `cout << "=== xxx ==="` 混杂输出，无法过滤也无法重定向。
这里统一成 `[HH:MM:SS] LEVEL  message`，并支持 `--quiet` / `--verbose`。
"""

import logging
import sys

_FORMAT = "[%(asctime)s] %(levelname)-7s %(message)s"
_DATEFMT = "%H:%M:%S"


def _harden_streams() -> None:
    """让 stdout/stderr 不会因为个别字符编码失败而中断整个脚本。

    Windows 控制台默认是 GBK(cp936)，能很好地显示中文，但没有 ¹⁴⁶ 这类上标字符。
    这里的原则是**尽量保留控制台原编码**（这样终端里中文不乱码），只是加上
    `errors="replace"` 兜底；只有当原编码完全不可用时才切到 UTF-8。
    另外，日志与图片文字里也应一律使用 ASCII 的数字写法（146Sm / 146Eu / T1/2），
    否则在这些地方会显示成 `?` 或方块。
    """
    probe = "中文 ± λ α σ …"
    for stream_name in ("stdout", "stderr"):
        stream = getattr(sys, stream_name, None)
        if stream is None or not hasattr(stream, "reconfigure"):
            continue
        encoding = getattr(stream, "encoding", None) or "utf-8"
        try:
            probe.encode(encoding)
        except (UnicodeEncodeError, LookupError):
            try:
                stream.reconfigure(encoding="utf-8", errors="replace")
                continue
            except (ValueError, OSError):
                continue
        try:
            stream.reconfigure(errors="replace")
        except (ValueError, OSError):  # 已被重定向到不支持 reconfigure 的对象
            pass


_harden_streams()


def get_logger(name: str = "pipeline") -> logging.Logger:
    """返回配置好的 logger（重复调用不会重复添加 handler）。"""
    logger = logging.getLogger(name)
    if not logger.handlers:
        handler = logging.StreamHandler(sys.stdout)
        handler.setFormatter(logging.Formatter(_FORMAT, datefmt=_DATEFMT))
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
        logger.propagate = False
    return logger


def set_level(level: str) -> None:
    """`quiet` / `normal` / `verbose` -> WARNING / INFO / DEBUG。"""
    mapping = {"quiet": logging.WARNING, "normal": logging.INFO, "verbose": logging.DEBUG}
    for logger in [logging.getLogger("pipeline")] + [
        logging.getLogger(n) for n in list(logging.root.manager.loggerDict)
    ]:
        logger.setLevel(mapping.get(level, logging.INFO))


def add_verbosity_argument(parser) -> None:
    """给 argparse 加统一的输出级别开关。"""
    parser.add_argument("--quiet", action="store_true", help="只输出警告与错误")
    parser.add_argument("--verbose", action="store_true", help="输出调试信息")


def apply_verbosity(args) -> None:
    if getattr(args, "quiet", False):
        set_level("quiet")
    elif getattr(args, "verbose", False):
        set_level("verbose")
    else:
        set_level("normal")


def banner(title: str, char: str = "=", width: int = 60) -> None:
    """打印分节标题，便于在长日志里定位步骤。"""
    line = char * width
    log = get_logger()
    log.info("")
    log.info(line)
    log.info("  %s", title)
    log.info(line)

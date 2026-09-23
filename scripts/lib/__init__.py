"""公共库：146Sm / 146Eu 半衰期测量全流程共用工具。

模块划分
--------
config    —— 统一读取 config/pipeline.json 与其它 JSON 配置
logutil   —— 统一日志格式（时间戳 + 级别）
rootio    —— ROOT 文件读写（基于 uproot）
spectrum  —— SPE 谱文件解析、能量刻度、峰面积积分
fitting   —— 高斯 / Crystal Ball 拟合、指数衰变拟合
plotting  —— 统一绘图风格与输出（figures/<类别>/xxx.pdf+.png）
"""

__all__ = ["config", "logutil", "rootio", "spectrum", "fitting", "plotting"]

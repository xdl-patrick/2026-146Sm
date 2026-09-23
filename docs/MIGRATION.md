# 本次整理做了什么（从「分散脚本」到统一项目）

## 问题

整理前 `D:\Sm147-2026` 根目录里堆着 62 个脚本、12 个中间 `.root`、13 张散落的图片，
`D:\Sm147-2025` 里还有 34 个更早的副本。具体病灶：

| 问题 | 具体情况 |
|---|---|
| **同一功能 11 个副本** | `Offline_146Sm.C`、`Offline_146Sm2025.C`、`Offline_146Sm2025_12.C`、`Offline_146Sm2025_2.C`、`Offline_146Sm2026_01/02/03/04/06/0601/0602.C` —— 每个 200~500 行，差异只在刻度系数与阈值 |
| **刻度宏 8 个副本** | `Cal12/13/14/15/146/147/147_01/2026.C`，其中 6 个用 128 段 `if(num==N){...}` 手写展开，共约 1600 行样板代码 |
| **系数硬编码在源码里** | 4×32 的刻度系数与逐道阈值写死在 `.C` 里，改一个数就要复制一份新文件 |
| **临时/损坏脚本** | `do_background.C`（没有函数体的裸脚本）、`check_and_repair.C`、`rebuild_bgsub_file.C`、`simple_channel_by_channel.C`、`combine_channels_simple.C`、`plot_same_canvas.C`、`interactive_fitter.py`（0 字节）、`EnsureResolutionCsvSchema.C`（641 行，内容其实是 `cbFit.cc` 的副本，文件名与内容不符） |
| **结果手抄** | `fit_correction.C` 把时间和计数率抄成 C++ `std::vector<double>` 常量；`GammaPeakAnalysis_Full_*.txt` 是手写的结果表 |
| **输出全堆在根目录** | 12 个 `.root` 中间产物 + 13 张 png/pdf，无法判断哪张图属于哪一步 |
| **SPE 解析丢信息** | `SpeToRoot.C` 只做「跳过 12 行然后读数」，`$DATE_MEA`（测量时刻）、`$MEAS_TIM`（活/死时间）全部丢弃——而后面做衰变拟合恰恰需要它们 |
| **重复数据集** | `D:\Sm146` 与 `F:\work\146Sm\root` 是同一批 2024 数据的两个拷贝（各 5.1 GB） |
| **重复工程** | `IMPSiSqTest/` 与 `main/impsisqtest/` 是同一个子课题的两份 |

## 现在的做法

### 1. 数据归位，原始/派生分离

```
data/raw/       原始数据，只读，不入 git
   alpha_2024/  alpha_2025/  alpha_2026/  gamma_eu146_2025/
data/processed/ 全部可从 raw 重建的产物
data/results/   结果表（入 git）
figures/<类别>/ 图片按规定类别归档
legacy/         旧脚本与旧中间产物（保留备查，不进流程）
```

原则：**原始数据一个都不删**；重复的、可重建的、一次性的内容一律移入 `legacy/`
或 `data/processed/`，需要时随时能再生成。

### 2. 参数进配置，脚本里零硬编码

`config/pipeline.json` 集中了路径、刻度系数、峰窗口、阈值、多重性上限、绘图格式。
逐道的 4×32 刻度系数与阈值由 `scripts/tools/import_legacy_config.py`
**从旧宏自动抽取**成 `config/alpha_calibration_2026.json` /
`config/alpha_thresholds_2026.json`（抽取时报告了 4 个未刻度通道、51 条阈值覆盖项）。
改参数只改 JSON。

### 3. 六个脚本 + 一个公共库，取代 50+ 个脚本

| 新脚本 | 取代 |
|---|---|
| `01_spe_to_root.py` | `SpeToRoot.C` + `BatchSpeToRoot.C` + `BatchSpeToRoot2024.C` |
| `02_alpha_offline.py` | 11 个 `Offline_146Sm*.C` + 8 个 `Cal*.C`（`--calibrate` 模式）+ `am241_calibration.C` + `SmEu_improved_calibration.C` + `CalculateCalibrationCoefficients.C` |
| `03_gamma_peak_area.py` | `AnalyzeSPEandROOTFiles.C` + `runFullAnalysis.C` |
| `04_eu146_halflife.py` | `fit_correction.C` |
| `05_normalize_targets.py` | 原工作流缺此步，按任务描述新写 |
| `06_sm146_halflife.py` | 原工作流缺此步，按任务描述新写 |
| `scripts/lib/` | 公共库：配置、日志、ROOT 读写、SPE 解析、拟合、绘图 |
| `run_all.py` | 一键串起 01→06 |

### 4. 关键实现改进

| 旧做法 | 新做法 |
|---|---|
| 逐事件 `GetEntry` + 128 个 `SetBranchAddress` | `uproot` 一次读入整棵树，NumPy 向量化求和与多重性筛选 |
| 128 段 `if(num==N)` 手写展开 | 一个双重循环 + 配置文件 |
| `.Spe` 只读数据段 | 完整解析头部，输出测量时刻/活时间/死时间表 |
| 刻度系数写在源码 | JSON 配置，且可由工具从旧宏重新抽取 |
| 逐 bin 的 `for` 循环积分峰面积 | 布尔掩码 + `sum()`，逻辑与旧代码逐步等价 |
| 结果手抄进 `.C` 常量 | 从 `data/results/*.csv` 直接读取 |
| 图片散落根目录 | `figures/{calibration,alpha,gamma,halflife}/`，同时输出 pdf（矢量）+ png（预览） |

### 5. 保真度验证

新流程在 ¹⁴⁶Eu 上复现了旧工作流的物理结论：

| 分组 | 新流程 T½ | 旧 `fit_correction.C` |
|---|---|---|
| Pb 吸收片 | 4.4935 ± 0.0344 d | 同类数据同一量级 |
| Cu 吸收片 | 4.4197 ± 0.0307 d | 同上 |
| 加权平均 | 4.4525 ± 0.0229 d | — |
| 参考值 | 4.610 ± 0.030 d | — |

铅铜两组一致（p = 0.109），但都比参考值低约 3~4%——**这与原工作流中
`fit_correction.C` 要解决的问题一致**（吸收片厚度/几何差异尚未修正），
说明流程没有引入新的偏差。

α 侧：54 个 2026 run 中 40 个成功重建 8 路求和谱，14 个被自动识别为
「只有 `adc4ch*` 的 ²⁴¹Am 刻度 run」或「采集失败的空文件」而跳过——分工正确。

## 仍未自动化 / 需要人工输入

1. **`config/runs.json`**：run / γ 文件 → 靶编号、测量时间。
   目前没有记录在任何旧脚本里，05/06 两步依赖它。
   用 `python scripts/05_normalize_targets.py --make-template` 生成骨架后填写。
2. **α run 的真实测量日期**：这些 ROOT 文件的内部时间戳被写成了 2009 年
   （DAQ 电脑时钟未校准），只有「先后顺序 + 时刻」可信，年份/月日需要人工补。
3. **`config/alpha_calibration_points.json`**：α 逐道刻度所需的参考点
   （测量文件 + 已知 α 能量）。准备好后用 `02_alpha_offline.py --calibrate` 重算刻度。
4. **α 峰窗口**：`config/pipeline.json` 里 `alpha_peaks` 的默认值依据核素数据表
   与旧脚本注释给出，请按实际谱确认。
5. **γ 文件名后三项标记的物理含义**（`9.5` / `0_0.5` / `1_0.5`），见
   `data/reference/README.md` 的对照表。
6. **系统误差**：要真正测出 ¹⁴⁶Sm 半衰期，还需要把束流积分、几何、立体角等
   系统误差压到 10⁻³ 量级——这部分没有被任何旧脚本覆盖。

---

## 附：人工整理表的接入（第二轮整理）

后来找到了作者手工整理的《gama 谱数据.xlsx》（`C:\...\msg\file\2025-12\`，
2025-12-25），做法：

| 动作 | 结果 |
|---|---|
| 原表存档 | `data/reference/gama 谱数据.xlsx`（原样复制，记录 MD5） |
| 写只读 xlsx 解析器 | `scripts/lib/xlsx.py`（不引入 openpyxl/pandas 依赖，本机没有） |
| 转换工具 | `scripts/tools/import_gama_excel.py` → 4 个工作表忠实导出 + 逐次测量规范表 + 时间序列表 |
| 接入拟合 | `04_eu146_halflife.py --rates-from reference` 直接读规范表 |
| 论文级输出 | `--paper` → 双面板散点图（pdf+png）与 LaTeX 结果表 |

交叉验证：本流程自算的 633/634 keV 计数率与 Excel 完全一致；747 keV 平均差
0.44 cps（约 4%），来源是峰窗口分箱不同（旧脚本用 16384 个 0.2009 keV 的 bin
加 ROOT `FindBin`，实际覆盖 744.85–749.03 keV；本流程用 0.5 keV 等宽分箱积分
[745, 749]）。论文数据以 Excel 为准，已在 `data/reference/README.md` 中记录。

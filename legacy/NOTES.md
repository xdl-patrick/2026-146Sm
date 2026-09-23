# legacy/ —— 旧脚本与旧中间产物归档

这里**不是**分析流程的一部分，只是保留备查。新流程见 `../README.md`。

## 内容

| 子目录 | 来源 | 说明 |
|---|---|---|
| `macros_2026/` | `D:\Sm147-2026\` 根目录 | 62 个 `.C` / `.cc` / `.py` / `.txt` / `.h` / `.so` / `.d` / `.pcm`，2025-09 ~ 2026-06 期间逐步累积 |
| `workspace_2025/` | `D:\Sm147-2025\` 根目录 | 34 个更早的脚本副本（其中 28 个与 `macros_2026/` 逐字节相同，7 个是旧版本） |
| `workspace_2025_extra/` | `D:\Sm146\`（2024 数据目录） | `Offline_146Sm.C`、`Offline_146Sm2025.C` —— **与 `macros_2026/` 里的同名文件内容不同**（是 2024 数据那一版的遗留副本），故单独保留 |
| `workspace_2025/Eu146-2025_scripts/` | `D:\Sm147-2025\Eu146-2025\` | ¹⁴⁶Eu γ 分析脚本（`runFullAnalysis.C`、`AnalyzeSPEandROOTFiles.C`、`fit_correction.C`）与手写结果表 |
| `intermediate_root/2026/`、`intermediate_root/2025/` | 两个工作目录根 | 旧的中间产物 `.root`（`sadd`、`sm146a`、`smadd`、`sum_all_calibrated`、`total_sum`、`test_sum`、`combined_result`、`debug_results`、`check_results`、`calibration_results`、`SmEu_calibration_results`） |
| `figures_2026/2026/`、`figures_2026/2025/` | 两个工作目录根 + `plot/` | 旧流程散落的 png / pdf |
| `workspace_2026_04_dup/` | 预留 | `D:\Sm147-2026\main\` 的去重目标位置（当前因被 VS Code 占用而未移动，见 `../docs/DATA_SOURCES.md`） |

## 归档脚本与新脚本的对应关系

| 旧脚本 | 被谁取代 |
|---|---|
| `SpeToRoot.C`、`BatchSpeToRoot.C`、`BatchSpeToRoot2024.C` | `../scripts/01_spe_to_root.py` |
| `Offline_146Sm*.C`（11 个） | `../scripts/02_alpha_offline.py` |
| `Cal12/13/14/15/146/147/147_01/2026.C`（8 个） | `../scripts/02_alpha_offline.py --calibrate` |
| `am241_calibration.C`、`SmEu_improved_calibration.C`、`CalculateCalibrationCoefficients.C` | `../scripts/02_alpha_offline.py --calibrate` + `../scripts/tools/import_legacy_config.py` |
| `AnalyzeSPEandROOTFiles.C`、`runFullAnalysis.C` | `../scripts/03_gamma_peak_area.py` |
| `fit_correction.C` | `../scripts/04_eu146_halflife.py` |
| `SimpleSumAll.C`、`combine_channels_simple.C`、`CheckFilteredData.C`、`check_and_repair.C`、`rebuild_bgsub_file.C`、`simple_channel_by_channel.C`、`do_background.C`、`plot_same_canvas.C`、`CompareTwoSpectra.C`、`Compare_146Sm_Data.C`、`Plot_E3_Compare*.C`、`draw_rounds.C`、`Filter1.C`、`ReadDetectorData.C`、`Sm.C`、`si_4.C`、`si_4.py`、`int.py`、`intGEANT4.py`、`draw same.py`、`interactive_fitter.py`（空文件） | 功能已被 `02`/`03` 覆盖，或被判定为一次性调试脚本；如需保留某个特殊功能请告知 |
| `cbFit.cc`、`cbFitLinked.cc`、`cbFitLinkedcaen.cc`、`plotResolutionVsFluence.cc`、`plotCB_alpha_compare.cc`、`draw_chain_diagram.py`、`EnsureResolutionCsvSchema.C` | 属于 **²⁴¹Am 分辨率子课题**，仍在 `../IMPSiSqTest/` 维护（`EnsureResolutionCsvSchema.C` 是 `cbFit.cc` 的错误命名副本，已丢弃） |
| `config.txt` | CAEN 在线 DAQ 配置，作为采集参数留档 |

## 注意事项

* 这些脚本依赖 WSL 里的 ROOT（`/mnt/d/root/root/bin/root`）和绝对路径
  （如 `/mnt/d/Sm147-2026/...`），移动数据目录后**不能再直接运行**。
* `EnsureResolutionCsvSchema.C` 与 `cbFit.cc` 内容重复、文件名错误，仅作归档。
* `interactive_fitter.py` 是 0 字节的空文件。

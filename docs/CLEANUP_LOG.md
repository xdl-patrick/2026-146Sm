# 清理记录

## 2026-09-23 — 删除 legacy/ 中的重复副本

**原则**：只删除与另一处**逐字节完全相同（MD5 一致）**的文件，
每个被删文件都有一份内容相同的副本保留在仓库里。
因内容有副本，未删除任何独有的数据或脚本。

- 删除文件数：**45**
- 释放空间：**519.58 MB**
- 保留策略：`macros_2026/` > `intermediate_root/2025/` > `figures_2026/2025/`
  （即由 `workspace_2025/`、`intermediate_root/2026/`、`figures_2026/2026/` 中删去重复项）

| 已删除 | 保留的副本 | 大小 | MD5 |
|---|---|---|---|
| `legacy/figures_2026/2026/Cal_Run480_Num2.png` | `legacy/figures_2026/2025/Cal_Run480_Num2.png` | 0.03 MB | `d533aa09c811f16898a9f75b4a240394` |
| `legacy/figures_2026/2026/comparison_plot.png` | `legacy/figures_2026/2025/comparison_plot.png` | 0.02 MB | `a94cd837ba1487c6e49c18d994363558` |
| `legacy/figures_2026/2026/comparison_plot_log.png` | `legacy/figures_2026/2025/comparison_plot_log.png` | 0.02 MB | `388774152792ed974f53555a279f968c` |
| `legacy/intermediate_root/2026/SmEu_calibration_results.root` | `legacy/intermediate_root/2025/SmEu_calibration_results.root` | 0.00 MB | `59e55af0001838bd4ce50e54e3204c68` |
| `legacy/intermediate_root/2026/calibration_results.root` | `legacy/intermediate_root/2025/calibration_results.root` | 0.56 MB | `26a71adef4a94ffbcbdf55dce2270d7f` |
| `legacy/intermediate_root/2026/check_results.root` | `legacy/intermediate_root/2025/check_results.root` | 0.01 MB | `7e266a632e92f7ff48991ece2fd07005` |
| `legacy/intermediate_root/2026/combined_result.root` | `legacy/intermediate_root/2025/combined_result.root` | 0.26 MB | `e7e83d8189ad2ad8aedc8b668f87de06` |
| `legacy/intermediate_root/2026/debug_results.root` | `legacy/intermediate_root/2025/debug_results.root` | 0.10 MB | `deb6aab29a37cdc7e175260f58a08e0c` |
| `legacy/intermediate_root/2026/sadd.root` | `legacy/intermediate_root/2025/sadd.root` | 193.98 MB | `506cb6fef11cd274a08f762c97965fee` |
| `legacy/intermediate_root/2026/sm146a.root` | `legacy/intermediate_root/2025/sm146a.root` | 96.08 MB | `513a2c88a205bf015be61d5d225a62b4` |
| `legacy/intermediate_root/2026/smadd.root` | `legacy/intermediate_root/2025/smadd.root` | 3.77 MB | `6665fc273508cb41cfe3aed33bcf5e34` |
| `legacy/intermediate_root/2026/sum_all_calibrated.root` | `legacy/intermediate_root/2025/sum_all_calibrated.root` | 207.89 MB | `6bd5cae7100b1d39d197ac325470b2c3` |
| `legacy/intermediate_root/2026/test_sum.root` | `legacy/intermediate_root/2025/test_sum.root` | 10.92 MB | `7bc6765cb1ef357331a4b31a85095321` |
| `legacy/intermediate_root/2026/total_sum.root` | `legacy/intermediate_root/2025/total_sum.root` | 5.68 MB | `e469cd79263659b9d78e3d030da5beca` |
| `legacy/workspace_2025/BatchSpeToRoot.C` | `legacy/macros_2026/BatchSpeToRoot.C` | 0.00 MB | `f7a2468a337103fd9ec0d6b68bb6c714` |
| `legacy/workspace_2025/BatchSpeToRoot2024.C` | `legacy/macros_2026/BatchSpeToRoot2024.C` | 0.01 MB | `e2770dbae09a39e0b99bc1b8d63f53f3` |
| `legacy/workspace_2025/Cal12.C` | `legacy/macros_2026/Cal12.C` | 0.02 MB | `70a6f657c0871c092914444449930432` |
| `legacy/workspace_2025/Cal13.C` | `legacy/macros_2026/Cal13.C` | 0.01 MB | `18ce0b7221ce47e39311bfdef9e3b988` |
| `legacy/workspace_2025/CalculateCalibrationCoefficients.C` | `legacy/macros_2026/CalculateCalibrationCoefficients.C` | 0.01 MB | `532e5cc7d1a7b474befb22b6a1508a4a` |
| `legacy/workspace_2025/CheckFilteredData.C` | `legacy/macros_2026/CheckFilteredData.C` | 0.00 MB | `17d9a6da743e4342af6bb60a49e327dd` |
| `legacy/workspace_2025/CompareTwoSpectra.C` | `legacy/macros_2026/CompareTwoSpectra.C` | 0.00 MB | `311908a14ca773c4f9c359f726ae81b2` |
| `legacy/workspace_2025/Compare_146Sm_Data.C` | `legacy/macros_2026/Compare_146Sm_Data.C` | 0.02 MB | `7f98e12e32ba7d1179d37f4eb7dc923f` |
| `legacy/workspace_2025/Filter1.C` | `legacy/macros_2026/Filter1.C` | 0.01 MB | `e1613dd68ccad4cca2662a47cd59831a` |
| `legacy/workspace_2025/Offline_146Sm.C` | `legacy/macros_2026/Offline_146Sm.C` | 0.01 MB | `7ec9d258d745d4f3298560d93213a785` |
| `legacy/workspace_2025/Offline_146Sm2026_01.C` | `legacy/macros_2026/Offline_146Sm2026_01.C` | 0.01 MB | `ac3ec92d50b2ed86f10ba2c89daef30a` |
| `legacy/workspace_2025/Offline_146Sm2026_03.C` | `legacy/macros_2026/Offline_146Sm2026_03.C` | 0.01 MB | `0e95998060243645cb82bc21438fce9d` |
| `legacy/workspace_2025/ReadDetectorData.C` | `legacy/macros_2026/ReadDetectorData.C` | 0.00 MB | `abef12b9a414fcdbd55dc9e93386413d` |
| `legacy/workspace_2025/SimpleSumAll.C` | `legacy/macros_2026/SimpleSumAll.C` | 0.01 MB | `fd4886fa1538abbba522b27f796b5d03` |
| `legacy/workspace_2025/SimpleSumAll_C.d` | `legacy/macros_2026/SimpleSumAll_C.d` | 0.01 MB | `fefea244d89f8b1850358bd9852cf98a` |
| `legacy/workspace_2025/SimpleSumAll_C.so` | `legacy/macros_2026/SimpleSumAll_C.so` | 0.04 MB | `97f2e906c8f208ffb8120522ae562768` |
| `legacy/workspace_2025/SimpleSumAll_C_ACLiC_dict_rdict.pcm` | `legacy/macros_2026/SimpleSumAll_C_ACLiC_dict_rdict.pcm` | 0.00 MB | `7cca7a3301729b4741cf3dd05de8ca33` |
| `legacy/workspace_2025/Sm.C` | `legacy/macros_2026/Sm.C` | 0.00 MB | `0a83ba48c9c566c07bc01eae8daa1ea3` |
| `legacy/workspace_2025/SmEu_improved_calibration.C` | `legacy/macros_2026/SmEu_improved_calibration.C` | 0.01 MB | `2e8eacadf091b359353d28d4888fb9c7` |
| `legacy/workspace_2025/SpeToRoot.C` | `legacy/macros_2026/SpeToRoot.C` | 0.00 MB | `b60285de5732ac9a16c2b56b8268acc6` |
| `legacy/workspace_2025/am241_calibration.C` | `legacy/macros_2026/am241_calibration.C` | 0.01 MB | `726c38d3853758fcd374ed506e5329ed` |
| `legacy/workspace_2025/calibration_coefficients.h` | `legacy/macros_2026/calibration_coefficients.h` | 0.00 MB | `6b2764886a23010b93c58647fbeedcc8` |
| `legacy/workspace_2025/check_and_repair.C` | `legacy/macros_2026/check_and_repair.C` | 0.00 MB | `81d9ff014fb22294cab9e20fda13bc33` |
| `legacy/workspace_2025/combine_channels_simple.C` | `legacy/macros_2026/combine_channels_simple.C` | 0.00 MB | `6c475270428cf79f60e69aaa92c54670` |
| `legacy/workspace_2025/data.txt` | `legacy/macros_2026/data.txt` | 0.00 MB | `42d39bc38f43cec53ee60a0918a9a282` |
| `legacy/workspace_2025/do_background.C` | `legacy/macros_2026/do_background.C` | 0.00 MB | `29e5e57bfd7a8326a972618ff2c886c8` |
| `legacy/workspace_2025/fit_means_output.txt` | `legacy/macros_2026/fit_means_output.txt` | 0.00 MB | `49558c9a8969360f4c19ac93f9525194` |
| `legacy/workspace_2025/interactive_fitter.py` | `legacy/macros_2026/interactive_fitter.py` | 0.00 MB | `d41d8cd98f00b204e9800998ecf8427e` |
| `legacy/workspace_2025/rebuild_bgsub_file.C` | `legacy/macros_2026/rebuild_bgsub_file.C` | 0.01 MB | `c733697c4ede23888f67106c325f2aec` |
| `legacy/workspace_2025/simple_calibration_parameters.txt` | `legacy/macros_2026/simple_calibration_parameters.txt` | 0.00 MB | `d39983706d09ce26cfa1cd877f162cc6` |
| `legacy/workspace_2025/simple_channel_by_channel.C` | `legacy/macros_2026/simple_channel_by_channel.C` | 0.00 MB | `7b14aade66d43ac746bfab40b8efc054` |

> 如需恢复：被删文件的内容与「保留的副本」完全相同，直接复制并改名即可。
>
> 其中 **28 个**（`legacy/workspace_2025/` 下的 `.C` / `.h` / `.txt` / `.py`）是 git 跟踪文件，
> 可由历史恢复：`git checkout bd916bd -- <路径>`（`bd916bd` = 本次清理前的 `main`）。
> 另 **17 个**（`legacy/figures_2026/2026/` 3 个、`legacy/intermediate_root/2026/` 11 个、
> 以及被 `.gitignore` 排除的 3 个 ROOT ACLiC 编译产物）不在 git 中，
> 但内容与上表「保留的副本」完全一致。

### 同时移除的空目录

| 目录 | 原因 |
|---|---|
| `legacy/intermediate_root/2026/` | 11 个文件全部是重复副本，删除后目录为空 |
| `legacy/workspace_2026_04_dup/` | 清理前即为空目录（0 文件 0 字节） |

### 未处理（按「其他不动」要求保留）

- `.git/lfs/` 1140.8 MB 孤立 LFS 对象、`.git/objects` 1126 MB 旧历史（C 级）
- `data/processed/` 260 MB（可重建）、`data/reference/146Sm伽马谱数据/Sm147-2025/` 56 MB（副本，B 级）
- `legacy/` 其余内容：`NOTES.md`、`macros_2026/`（62）、`workspace_2025/`（18）、
  `workspace_2025_extra/`（2）、`figures_2026/`（12）、`intermediate_root/2025/`（13）
- `legacy/macros_2026/EnsureResolutionCsvSchema.C`（错命名，但与 `cbFit.cc` **并非**逐字节相同，
  不满足「有副本」条件，故保留）

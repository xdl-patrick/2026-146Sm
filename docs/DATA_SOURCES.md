# 相关数据文件夹清点（本机）

扫描范围：`C:` / `D:` / `E:` / `F:` 所有驱动器（目录名匹配 `146|147|HPGe|BGO|Eu|Sm`）。
下面把「与本课题相关」和「无关」分开列出，并说明处理方式。

---

## 1. 已纳入本项目

| 原路径 | 内容 | 规模 | 现位置 |
|---|---|---|---|
| `F:\360MoveData\Users\13474\OneDrive\桌面\146Sm伽马谱数据\gama 谱数据.xlsx` | **人工整理表 v2**（2026-01-14，7 个工作表，含靶室号与误差；推荐使用） | 102 KB | `data/reference/gama 谱数据_v2.xlsx` |
| `C:\Users\13474\OneDrive\xwechat_files\...\2025-12\gama 谱数据.xlsx` | **人工整理表 v1**（2025-12-25，4 个工作表） | 57 KB | `data/reference/gama 谱数据.xlsx`（+ 转出的 `eu146_measurements.csv` 等） |
| `D:\Sm147-2026\2026_146Sm*.root` | 2026 年硅条 α run（Tree1：128/144/36/4 通道） | 54 个 / 1.2 GB | `data/raw/alpha_2026/` |
| `D:\Sm147-2025\2025_146Sm*.root` | 2025 年 α run（128 通道） | 141 个 | `data/raw/alpha_2025/` |
| `D:\Sm146\*.root` | **2024 年束流实验原始数据**（Evd2Root 转换，128 通道，2024-03） | 240 个 / 5.14 GB | `data/raw/alpha_2024/` |
| `D:\Sm146\Offline_146Sm.C`、`Offline_146Sm2025.C` | 2024 数据那一版的遗留脚本（与 2026 版不同） | 2 个 | `legacy/workspace_2025_extra/` |
| `D:\Sm147-2025\Eu146-2025\*.Spe` | **¹⁴⁶Eu 的 HPGe 原始谱**（2025-10-11 ~ 2025-12-20） | 357 个 / 70 MB | `data/raw/gamma_eu146_2025/` |
| `D:\Sm147-2025\Eu146-2025\*.root` 与无扩展名的 42 个 | 由 `.Spe` 转换的 ROOT（可重建） | 397 个 | `data/processed/gamma_eu146_2025_root/` |
| `D:\Sm147-2026\2025_146Sm100.root` | 与 `D:\Sm147-2025\2025_146Sm100.root` **逐字节相同**（MD5 一致）的副本 | 1 个 | `legacy/intermediate_root/2025/` |

### 重复数据集（已确认，只需保留一份）

| 路径 | 规模 | 说明 |
|---|---|---|
| `D:\Sm146` | 240 文件 / 5136.8 MB | **已整体移入** `data/raw/alpha_2024/`；目录内已留 `README.md` 说明 |
| `F:\work\146Sm\root` | 240 文件 / 5386322878 B | 同一批 2024 数据；`F:\work\146Sm\log\` 里有 497 个 `Evd2Root` 转换日志 |

> **处理方式**：保留一份规范位置 `data/raw/alpha_2024/`。
> `F:\work\146Sm` **保持原样未动**——它是同一批数据的另一份拷贝，
> 但转换日志（`F:\work\146Sm\log\`）只在这里有，删掉会丢失溯源信息。
> 确认 `data/raw/alpha_2024/` 可用后，可以自行删除 F 盘那份以释放 5.1 GB：
>
> ```powershell
> # 核对无误后手动执行
> Remove-Item 'F:\work\146Sm\root' -Recurse
> # 日志建议保留或先复制到 docs/ 下
> Copy-Item 'F:\work\146Sm\log' 'D:\Sm147-2026\docs\evd2root_logs' -Recurse
> ```

### 仍留在原处、仅供登记的数据

| 路径 | 内容 | 规模 | 为什么没动 |
|---|---|---|---|
| `D:\In-el\data\` | CoMPASS 的 HPGe 测试原始数据（`Run-HPGe-Test_23/32/34/35`、`20250331`）、CAEN 配置 | ~10 GB | 属于探测器调试，不是本课题的靶数据；如需一并分析请告知 |
| `D:\Sm147-2025\gama-2024\147Sm*\` | 上一年（2024-04 ~ 2024-07）的 ¹⁴⁷Sm γ 测量，43 个日期文件夹 | 40 MB | 上一年度的课题，本次未纳入 |
| `D:\Sm147-2025\Eu146-2025\` 下剩余的 `.git` | 旧工作副本的版本库 | — | 保留以便追溯 |
| `D:\Sm147-2025\` | 移走数据与脚本后只剩 `gama-2024/` 与 `.git/` | — | 见上 |

## 2. 与本课题无关

| 路径 | 内容 |
|---|---|
| `D:\BGO_20260606\run01\`（及工作区里的 `g:\BGO_20260606`） | BGO 探测器的一组测试 run（`run01`，CH13/CH14，2026-06-06），12 MB |
| `E:\BGOData\` | `twoBGO_C1_*.root` 波形数据，219 个 / 3.8 GB |

> 工作区里配置的 `g:\BGO_20260606` 盘符当前不存在（应为 `D:\BGO_20260606`）。

## 3. 旧脚本归档

| 原位置 | 数量 | 现位置 |
|---|---|---|
| `D:\Sm147-2026\*.C / *.cc / *.py / *.txt / *.h / *.so / *.d / *.pcm` | 62 | `legacy/macros_2026/` |
| `D:\Sm147-2025\*.C / *.py / *.txt / *.h / *.csv` | 34 | `legacy/workspace_2025/` |
| `D:\Sm147-2025\Eu146-2025\` 的分析脚本与结果文本 | 10 | `legacy/workspace_2025/Eu146-2025_scripts/` |
| 旧中间产物 `.root`（`sadd/sm146a/smadd/sum_all_calibrated/total_sum/test_sum/combined_result/...`） | 11 + 13 | `legacy/intermediate_root/2026/`、`legacy/intermediate_root/2025/` |
| 散落在根目录的 png/pdf | 5 + 8 | `legacy/figures_2026/2026/`、`legacy/figures_2026/2025/` |
| `D:\Sm147-2026\plot\`（4 张 cbFit 图） | 4 | `legacy/figures_2026/2026/` |
| `D:\Sm147-2026\main\impsisqtest\` | 重复的早期副本（比 `IMPSiSqTest/` 旧，且各源文件都更小） | **未移动**（被 VS Code 作为工作区根占用而锁定）；见下 |

> `D:\Sm147-2026\main\` 与 `D:\Sm147-2026\IMPSiSqTest\` 内容重叠。
> `IMPSiSqTest/` 是较新且较完整的一份（`cbFit.cc` 33216 B vs 32491 B）。
> 因为 `main` 仍是 VS Code 的一个工作区根目录，文件被占用无法移动。
> 若要清理：先从工作区移除该文件夹，再执行
> `Move-Item 'D:\Sm147-2026\main' 'D:\Sm147-2026\legacy\workspace_2026_04_dup\'`。

## 4. 数据格式速查

| 数据 | 文件 | 树/对象 | 关键分支/字段 |
|---|---|---|---|
| α run（2024/2025） | `146SmN.root`、`2025_146SmN.root` | `Tree1` | `adc0ch0`..`adc3ch31`（uint32，原始道数）+ 128 个 TH1I |
| α run（2026，8 块靶） | `2026_146SmN.root` | `Tree1` | `adc0ch0`..`adc3ch31`（不同 run 通道数不同：128/144/36） |
| α 刻度 run（²⁴¹Am） | `2026_146Sm62/71/72/75...` | `Tree1` | 只有 `adc4ch12`..`adc4ch15` |
| HPGe γ 原始谱 | `*.Spe` | 文本 | 头 `$DATE_MEA` / `$MEAS_TIM`(活时间 实时间) / `$DETECTOR`；`$DATA:` 后 16384 个计数 |
| HPGe γ 转换后 | `*.root` | `h` / `spectrum` | `h` = 能量刻度后（keV），`spectrum` = 原始道谱 |

* 2024/2025 数据由 `Evd2Root` 从 `.evd` 转换而来；转换日志见 `F:\work\146Sm\log\`。
* 2026 数据由 CAEN 在线 DAQ 产生，配置文件为 `legacy/macros_2026/config.txt`
  （4×V785 32 通道 + 1×V785N 16 通道 + 1×V560）。

# 146Sm / 146Eu 半衰期测量数据分析

用 4 块双面硅条 α 探测器测量 8 块靶中 ¹⁴⁶Sm 的特征 α 峰，同时用高纯锗（HPGe）
探测器测量同一批靶中 ¹⁴⁶Eu 的特征 γ 峰（2025-10 ~ 2026-01）。

分析链路分两个目的：

1. **验证 HPGe 系统**：由 ¹⁴⁶Eu 计数率随时间的下降拟合出它的半衰期，与已知值
   4.61 d 对比；
2. **给出 ¹⁴⁶Sm 的半衰期**：先用做过绝对活度测量的 2 块靶、在测量质量最好的一天
   对 8 块靶的 ¹⁴⁶Eu 活度做归一（消掉靶间几何/自吸收差异），再用归一后的 α 计数率
   随时间的变化拟合 ¹⁴⁶Sm 半衰期。

---

## 快速开始

```bash
# 依赖：Python >= 3.9，numpy / scipy / matplotlib / uproot
pip install numpy scipy matplotlib uproot

# 一键跑完全流程
python scripts/run_all.py

# 只跑前 3 步；只处理 5 个文件做自检
python scripts/run_all.py --steps 1,2,3
python scripts/run_all.py -- --limit 5

# 单步运行
python scripts/01_spe_to_root.py
python scripts/02_alpha_offline.py --runs 71,72,75
python scripts/03_gamma_peak_area.py --source root
python scripts/04_eu146_halflife.py --peak 747keV --group Pb
python scripts/05_normalize_targets.py --make-template   # 生成 config/runs.json 骨架
python scripts/06_sm146_halflife.py
```

CERN ROOT（本机在 WSL 里：`/mnt/d/root/root/bin/root`，6.32.02）**不再是必需依赖**，
只在需要交互式查看 ROOT 文件时使用。

---

## 目录结构

```
config/                     所有可调参数集中在这里（脚本里不再硬编码）
  pipeline.json             主配置：路径、刻度系数、峰窗口、阈值、绘图格式
  alpha_calibration_2026.json   4x32 逐道刻度系数（由旧宏抽取）
  alpha_thresholds_2026.json    4x32 逐道阈值
  runs.json                 run/文件 -> 靶编号、时间（需人工填写，见 05 步）
data/
  raw/                      原始数据（只读）
    alpha_2024/             2024 年束流实验原始数据（146SmN.root，128 通道）★ 不入 git
    alpha_2025/             2025 年 α run（2025_146SmN.root，128 通道）★ 不入 git
    alpha_2026/             2026 年 α run（2026_146SmN.root，4~144 通道）★ 不入 git
    gamma_eu146_2025/       146Eu 的 HPGe 原始谱（*.Spe，357 个）✔ 入 git（56 MB）
  reference/                人工整理的结果表（✔ 入 git，见其中 README.md）
    gama 谱数据.xlsx        ★ 作者手工整理的 146Eu 计数率与半衰期数据（v1，权威来源）
    gama 谱数据_v2.xlsx     ★ 2026-01 更新版（4 个靶室 × Pb/Cu/12.5cm/0/0_0.5/1_0.5 六阶段）
    eu146_measurements.csv  由 v1 表转出的逐次测量规范表（242 行）
    eu146_series.csv        用于 ln 拟合的时间序列
    gama_excel_sheets/      v1 表 4 个工作表的忠实导出，便于核对
    gama_v2/                v2 表 7 个工作表的转换产物（sheets/ + *_tidy.csv + tables/）
    146Sm伽马谱数据/        作者桌面的原始文件夹副本（xlsx + Origin 工程）
  processed/                可重建的派生产物 ★ 不入 git
    gamma_eu146_2025_root/  01 步产出的 ROOT
    alpha_spectra/          02 步产出的 8 个求和谱
  results/                  结果表 ✔ 入 git
scripts/
  lib/                      公共库（config/logutil/rootio/spectrum/fitting/plotting）
  01_spe_to_root.py         HPGe 原始谱 -> ROOT + 测量元数据
  02_alpha_offline.py       硅条 α：逐道求和 + 多重性筛选（含 --calibrate 刻度模式）
  03_gamma_peak_area.py     HPGe 峰面积与计数率
  04_eu146_halflife.py      ¹⁴⁶Eu 半衰期拟合
  05_normalize_targets.py   8 块靶 ¹⁴⁶Eu 活度归一
  06_sm146_halflife.py      ¹⁴⁶Sm 半衰期
  07_eu146_lnfit_reference.py  用 v2 表「修正后」数据做 ln(计数率) 线性拟合（论文用）
  run_all.py                一键全流程
  tools/
    import_legacy_config.py 从旧 ROOT 宏抽取刻度系数/阈值 -> config/*.json
    import_gama_excel.py    v1 Excel -> eu146_measurements.csv / eu146_series.csv
    import_gama_excel_v2.py v2 Excel（7 个工作表）-> data/reference/gama_v2/
    make_fit_report.py      汇总 docs/report_eu146_fits.html
figures/                    图片输出 ✔ 入 git（48 个文件，3.8 MB）
  calibration/  alpha/  gamma/  halflife/   migration/
docs/
  DATA_SOURCES.md           全部相关数据文件夹清点（含未纳入本项目的数据）
  MIGRATION.md              本次从"分散脚本"到统一项目做了什么
legacy/                     旧脚本归档 ✔ 入 git（仅 1.1 MB；
                            其中 intermediate_root/ 与 figures_2026/ 的大文件已忽略）
  macros_2026/              原 D:\Sm147-2026 根目录下的 62 个 .C/.cc/.py 等
  workspace_2025/           原 D:\Sm147-2025 的脚本 + Eu146-2025 的分析脚本
  intermediate_root/        旧的中间产物 .root（sadd/sm146a/sum_all_calibrated/...）
  figures_2026/             旧流程散落在根目录的 png/pdf
  NOTES.md                  归档清单 + 旧脚本与新脚本的对应关系
IMPSiSqTest/                另一个子课题：²⁴¹Am α 峰形与分辨率研究
                            ✔ 已由 git 子模块转为普通文件，随本仓库一起纳入
docs/DATA_SOURCES.md        本机相关数据文件夹清点
docs/MIGRATION.md           本次整理做了什么、保真度验证、仍需人工输入的部分
```

---

## 版本库内容说明

本仓库（`github.com/xdl-patrick/2026-146Sm`）的收录范围：

**已纳入**（共 596 个文件，约 63 MB）

| 内容 | 体积 | 说明 |
|---|---|---|
| `scripts/`、`config/`、`docs/`、`README.md` | ~0.2 MB | 全部分析代码与配置，`run_all.py` 一键复现 |
| `data/results/` | 0.2 MB | 结果表（csv + LaTeX） |
| `data/reference/` | 57.8 MB | 作者整理的 Excel、转换产物、Origin 工程 |
| `data/raw/gamma_eu146_2025/` | 56 MB | 357 个 γ 原始 `.Spe`（¹⁴⁶Eu 分析的全部输入） |
| `figures/` | 3.8 MB | 标定 / alpha / gamma / 半衰期论文图 |
| `legacy/`（非忽略部分） | 1.1 MB | 旧脚本归档 |
| `main/`、`IMPSiSqTest/` | 0.3 MB | ²⁴¹Am 峰形子项目源码 |

**未纳入**（见 `.gitignore`）

| 内容 | 体积 | 原因 |
|---|---|---|
| `data/raw/alpha_2024\|2025\|2026/` | 7.9 GB | 超出 GitHub 单文件 100 MB 硬上限（最大单文件 414 MB） |
| `data/processed/` | 260 MB | 可由 `python scripts/run_all.py` 重建 |
| `legacy/intermediate_root/`、`legacy/figures_2026/` | 1.0 GB | 旧流程中间产物，保留在本地备查 |
| `data/reference/146Sm伽马谱数据/Sm147-2025/` | 56 MB | 与 `data/raw/gamma_eu146_2025/` 逐字节相同的重复副本 |

> **注意**：α 原始数据未入库，因此单靠 clone 无法完整复现 α 分析；
> 需要把 `data/raw/alpha_2024`、`alpha_2025`、`alpha_2026` 放到对应位置。
> γ 部分（含 ¹⁴⁶Eu 半衰期拟合）则可以完全复现。

---

## 分析流程

### 01 SPE -> ROOT
扫描 `data/raw/gamma_eu146_2025/*.Spe`，解析头部（`$DATE_MEA`、`$MEAS_TIM`、
`$DETECTOR`），写两个直方图：`spectrum`（原始道谱）与 `h`（能量刻度后）。

输出：`data/results/gamma_measurements.csv`（每个文件的测量时间/活时间/死时间），
      `figures/gamma/gamma_eu146_qc_deadtime.*`

### 02 α 离线分析
每个 run 文件里 4 块双面硅条探测器对应 4 个 ADC 模块，每模块 32 道
（0-15 正面 front、16-31 背面 back）。逐事件：

1. 逐道 `E = slope × ADC + intercept`；
2. 只保留 `ADC > 阈值`（逐道阈值）；
3. 每块探测器每个面把所有条的能量求和 → `E1f/E1b/.../E4b`；
4. **多重性筛选**：某面有 ≥2 条同时触发时该面整体丢弃（默认阈值 `multiplicity_max=1`）；
5. 填 8 个一维谱 + 4 个 front-vs-back 二维谱。

输出：`data/processed/alpha_spectra/{source}_run{N}_spectra.root`、
      `data/results/alpha_run_summary.csv`、
      `figures/alpha/{source}_run{N}_spectra.*`

**多代数据源**：`config/pipeline.json` 的 `alpha_offline.sources` 里登记了三代数据，
默认只跑 `alpha_2026`（本次 8 块靶的数据）：

```bash
python scripts/02_alpha_offline.py --source alpha_2024   # 2024 束流实验（146SmN.root）
python scripts/02_alpha_offline.py --all-sources         # 三代一起处理
```

> 汇总表每次都会**重写**；用了 `--limit/--runs/--source/--input-dir` 时只包含本次
> 处理的 run，脚本会给出提醒。

> `2026_146Sm62/71/72/75...` 这类只有 `adc4ch12-15` 的 run 是 **²⁴¹Am α 刻度测量**，
> 不由本流程处理，属于 `IMPSiSqTest/`（`cbFit.cc` 等）。
> `2026_146Sm8/10/11...` 是采集失败留下的空文件，自动跳过。

### 03 HPGe 峰面积
对每个谱在 633 keV / 747 keV 窗口做线性本底积分（峰区两侧各取一段本底区），
计数率 = 净计数 / 活时间。

输出：`data/results/gamma_peak_areas.csv`、`figures/gamma/`

### 04 ¹⁴⁶Eu 半衰期
按吸收片分组（默认材料级：Pb / Cu），对 `ln(rate) = ln(A₀) − λt` 做加权线性拟合，
得到 `T½ = ln2/λ`，并与参考值 4.61 ± 0.03 d 比较。

输出：`data/results/halflife_eu146_{peak}.csv`、`figures/halflife/`

> 不同吸收片（铅砖/铜砖/不同厚度）会给出不同的表观半衰期，这正是原
> `fit_correction.C` 要处理的问题。铅铜两组之间的一致性检验（χ²/p 值）会自动打印。

#### 用人工整理的表做论文图（推荐）

```bash
python scripts/04_eu146_halflife.py --rates-from reference --group-by condition \
       --paper --paper-lang zh
```

* `--rates-from reference` 改用 `data/reference/eu146_measurements.csv`（由作者手工整理的
  `gama 谱数据.xlsx` 转出），这是论文数据的权威来源；
* `--paper` 输出论文级双面板图（上：计数率 vs 时间对数轴 + 指数拟合；下：标准化残差）
  与结果表：
  * `figures/halflife/eu146_halflife_747keV_paper.pdf` / `.png`
  * `data/results/eu146_halflife_747keV_table.csv` / `.tex`（可直接 `\input`）
* `--paper-lang zh` 用中文（宋体）标签，默认 `en` 用 Times 英文标签；
* `--paper-groups Pb,Cu` 选择进论文的分组（默认就是 Pb、Cu——它们是同一测量条件下的
  时间序列，χ²/ndf 才有意义；`0`/`0_0.5`/`1_0.5`/`12.5` 混了不同靶位与距离）。

#### 全部测量条件一起出图

```bash
python scripts/04_eu146_halflife.py --rates-from reference --group-by condition \
       --paper --paper-groups all --paper-per-group --paper-grid --paper-lang zh
python scripts/tools/make_fit_report.py     # 汇总成 docs/report_eu146_fits.html
```

| 产出 | 说明 |
|---|---|
| `figures/halflife/eu146_fit_grid_747keV.*` | 联络表：6 个条件各一格 |
| `figures/halflife/eu146_fit_allgroups_747keV.*` | 6 个条件叠加 |
| `figures/halflife/eu146_fit_{条件}_747keV.*` | 每个条件单独一张（双面板） |
| `docs/report_eu146_fits.html` | 全部图 + 结果表的汇总网页 |

#### 用 v2 表的「修正后」数据直接拟合（推荐用于论文）

```bash
python scripts/tools/import_gama_excel_v2.py     # 7 个工作表 -> 论文可引用的规范表
python scripts/07_eu146_lnfit_reference.py --lang zh
```

作者已把 Pb / Cu / 12.5cm 有机玻璃 / 0 / 0_0.5 / 1_0.5 六个阶段的**绝对计数率差异修正掉**，
所以 `原始数据修正后` 里 4 组（= 4 个靶室）的 `ln(rate)` 对时间各是一条直线：

| 数据组 | 峰 | n | T½ (d) | χ²/ndf |
|---|---|---|---|---|
| 1（靶室 1） | 633 / 747 | 75 | 4.473 ± 0.006 / 4.365 ± 0.010 | 73.0/73 |
| 2（靶室 2） | 633 / 747 | 80 | 4.482 ± 0.012 / 4.324 ± 0.018 | 78.0/78 |
| 3（靶室 3） | 633 / 747 | 82 | 4.444 ± 0.008 / 4.354 ± 0.014 | 80.0/80 |
| 4（靶室 4） | 633 / 747 | 82 | 4.486 ± 0.008 / 4.334 ± 0.013 | 80.0/80 |

**加权平均 T½ = 4.438 ± 0.003 d**（参考值 4.610 ± 0.030 d，偏低 3.8%）。
两个峰的 T½ 相差约 2.5%，是需要弄清楚的系统效应——详见 `data/reference/README.md`。

实测结果（`--rates-from reference`）：

| 分组 | 点数 | T½ (d) | χ²/ndf | 与参考值偏差 |
|---|---|---|---|---|
| Pb | 58 | 4.495 ± 0.040 | 680/56 | −2.3σ |
| Cu | 71 | 4.453 ± 0.035 | 946/69 | −3.4σ |

两组一致，但比参考值低 2~3%、χ²/ndf 远大于 1 —— 说明**吸收/几何修正尚未完成**，
这也正是原 `fit_correction.C` 试图解决的问题。

### 05 8 块靶 ¹⁴⁶Eu 活度归一
在读入 `config/runs.json`（run → 靶编号 → 时间的映射）后：

1. 把每块靶的 ¹⁴⁶Eu 计数率按 04 步的半衰期修正到统一参考日；
2. 用参考靶之间的一致性 + 统计误差给每个候选测量日打分，挑出「最好的一天」；
3. 计算归一因子 `k_j = A_ref / A_j`。

输出：`data/results/target_normalization.csv`、`figures/halflife/normalization_targets_*`

> **需要人工填写 `config/runs.json`。** 目前靶编号 -> run/文件 的对应关系没有
> 记录在任何脚本里，请用 `python scripts/05_normalize_targets.py --make-template`
> 生成骨架后补全（骨架里会列出所有可用的 run 与 γ 文件）。

### 06 ¹⁴⁶Sm 半衰期
用 05 步的归一因子修正每块靶的 α 计数率，按测量轮次加权平均，对
`R(t) = R₀·exp(−λt)` 做指数拟合。

输出：`data/results/halflife_sm146_{peak}.csv`、`figures/halflife/`

> **物理提示**：¹⁴⁶Sm 半衰期约 1.03×10⁸ 年，在几个月到几年的窗口内衰减 < 1%。
> 若只用 2025–2026 的数据，脚本会如实报告「未探测到显著衰变」，而不是给出一个
> 假的半衰期。要真正测出它，需要先用
> `python scripts/02_alpha_offline.py --source alpha_2024`（必要时再加 `--source alpha_2025`）
> 处理 2024 年的数据，把跨年的测量点一起纳入，并且把系统误差压到 10⁻³ 量级。

---

## 配置说明（`config/pipeline.json`）

| 配置项 | 含义 |
|---|---|
| `paths.*` | 各类数据的目录（相对项目根） |
| `alpha_offline.*` | α 离线分析：树名、ADC 模块数、阈值、多重性上限、直方图范围 |
| `alpha_peaks.*` | α 特征峰窗口（MeV） |
| `gamma.energy_calibration` | HPGe 的道->能量刻度系数 |
| `gamma.peaks.*` | γ 峰积分窗口与本底区宽度 |
| `eu146_halflife.reference_halflife_days` | ¹⁴⁶Eu 参考半衰期 |
| `sm146_halflife.reference_halflife_years` | ¹⁴⁶Sm 参考半衰期 |
| `plot.formats` / `plot.png_dpi` | 图片输出格式与分辨率 |

改参数请改 JSON，不要改脚本。

---

## 环境与约定

* **控制台编码**：Windows 默认 GBK，脚本会自动把 `stdout/stderr` 设为
  `errors="replace"` 以免个别符号把程序打断。日志与图片文字统一使用
  ASCII 写法（`146Sm`、`146Eu`、`T1/2`、`chi2`），中文照常显示。
* **ROOT**：仅用于人工查看；本流程用 `uproot` 读写。
* **单位**：α 能谱横轴 MeV，γ 能谱横轴 keV，时间轴小时/天。

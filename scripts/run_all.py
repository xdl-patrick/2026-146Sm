#!/usr/bin/env python3
"""run_all.py — 一键跑完整条 ¹⁴⁶Sm / ¹⁴⁶Eu 分析链。

流水线
------
    01_spe_to_root.py          HPGe 原始 .Spe -> ROOT + 测量元数据表
    02_alpha_offline.py        双面硅条 α：逐道求和 + 多重性筛选 -> 8 个能谱
    03_gamma_peak_area.py      HPGe 峰面积与计数率（633 / 747 keV）
    04_eu146_halflife.py       ¹⁴⁶Eu 半衰期拟合（验证 HPGe，参考值 4.61 d）
    05_normalize_targets.py    用参考靶/参考日归一 8 块靶的 ¹⁴⁶Eu 活度
    06_sm146_halflife.py       ¹⁴⁶Sm 半衰期（最终结果）

每一步也可以单独运行；本脚本只是按顺序调用它们，并汇总成败。

用法
----
    python scripts/run_all.py                     # 全部
    python scripts/run_all.py --steps 1,2,3       # 只跑到 03
    python scripts/run_all.py --from 3            # 从 03 开始
    python scripts/run_all.py --dry-run           # 只打印将要执行的命令
    python scripts/run_all.py -- --limit 5        # `--` 之后的参数透传给每个步骤
"""

import argparse
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
PROJECT_ROOT = HERE.parent

sys.path.insert(0, str(HERE))
import lib.logutil as logutil  # noqa: E402  仅为把 stdout/stderr 设为 errors="replace"

logutil.get_logger()

STEPS = [
    (1, "01_spe_to_root.py", "HPGe 原始谱 -> ROOT + 元数据"),
    (2, "02_alpha_offline.py", "双面硅条 α 离线求和谱"),
    (3, "03_gamma_peak_area.py", "HPGe 峰面积与计数率"),
    (4, "04_eu146_halflife.py", "146Eu 半衰期拟合"),
    (5, "05_normalize_targets.py", "8 块靶 146Eu 活度归一"),
    (6, "06_sm146_halflife.py", "146Sm 半衰期"),
]


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="一键运行全流程")
    ap.add_argument("--steps", default="", help="要运行的步骤号，逗号分隔，例如 1,2,3")
    ap.add_argument("--from", dest="from_step", type=int, default=1, help="从第几步开始")
    ap.add_argument("--to", dest="to_step", type=int, default=99, help="到第几步结束")
    ap.add_argument("--keep-going", action="store_true", help="某步失败也继续往下跑")
    ap.add_argument("--dry-run", action="store_true", help="只打印命令，不执行")
    ap.add_argument("extra", nargs=argparse.REMAINDER, help="`--` 之后透传给各步骤的参数")
    return ap.parse_args()


def select_steps(args) -> list:
    if args.steps:
        wanted = {int(x) for x in args.steps.split(",") if x.strip()}
        return [s for s in STEPS if s[0] in wanted]
    return [s for s in STEPS if args.from_step <= s[0] <= args.to_step]


def main() -> int:
    args = parse_args()
    steps = select_steps(args)
    extra = [a for a in args.extra if a != "--"]

    print(f"项目根目录: {PROJECT_ROOT}")
    print(f"将运行 {len(steps)} 个步骤:")
    for num, script, desc in steps:
        print(f"  {num}. {script:<28} {desc}")
    if extra:
        print(f"透传参数: {extra}")
    print()

    failed = []
    t_start = time.time()
    for num, script, desc in steps:
        cmd = [sys.executable, str(HERE / script)] + extra
        print("=" * 70)
        print(f"[{num}/6] {desc}")
        print(f"        $ {' '.join(cmd)}")
        print("=" * 70)
        if args.dry_run:
            continue
        t0 = time.time()
        result = subprocess.run(cmd, cwd=str(PROJECT_ROOT))
        elapsed = time.time() - t0
        if result.returncode != 0:
            failed.append((num, script, result.returncode))
            print(f"[{num}/6] 失败 (exit={result.returncode}, {elapsed:.1f}s)")
            if not args.keep_going:
                print("已中止。加 --keep-going 可忽略失败继续执行。")
                break
        else:
            print(f"[{num}/6] 完成 ({elapsed:.1f}s)")

    print()
    print(f"总耗时 {time.time() - t_start:.1f}s")
    if failed:
        print("失败步骤:")
        for num, script, code in failed:
            print(f"  {num}. {script} (exit={code})")
        return 1
    print("全部步骤完成。结果见 data/results/，图片见 figures/。")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

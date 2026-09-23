#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
五列数据交互式线性拟合程序 (Python版本)
支持手动选择数据点，进行加权线性拟合，计算半衰期
增加自动滑动窗口拟合功能
"""

import numpy as np
import matplotlib.pyplot as plt
import sys
import os
from dataclasses import dataclass
from typing import List, Tuple, Optional, Dict
import json
import pandas as pd

@dataclass
class DataPoint:
    """数据点类"""
    id: int
    x: float          # 第1列: X值
    y1: float         # 第2列: Y值1
    err1: float       # 第3列: 误差1
    y2: float         # 第4列: Y值2
    err2: float       # 第5列: 误差2
    selected: bool = False

@dataclass
class FitResult:
    """拟合结果类"""
    slope: float
    slope_err: float
    intercept: float
    intercept_err: float
    chi2: float
    r2: float
    selected_ids: List[int]
    y_column: int  # 使用的Y列 (2或4)
    half_life: Optional[float] = None
    half_life_err: Optional[float] = None

@dataclass
class WindowFitResult:
    """滑动窗口拟合结果类"""
    start_id: int
    end_id: int
    middle_x: float
    slope: float
    slope_err: float
    half_life: float
    half_life_err: float
    chi2: float
    r2: float

class InteractiveFitter:
    """交互式拟合器"""
    
    def __init__(self):
        self.data_points = []
        self.fit_history = []
        self.window_fit_history = []  # 存储滑动窗口拟合结果
        self.current_y_col = 2  # 默认使用第2列作为Y值
        
    def load_data(self, filename: str) -> bool:
        """加载数据文件"""
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                lines = f.readlines()
        except FileNotFoundError:
            print(f"错误: 文件 '{filename}' 未找到")
            return False
        except Exception as e:
            print(f"错误: 无法读取文件 '{filename}': {e}")
            return False
        
        print(f"\n=== 读取数据文件: {filename} ===")
        print("格式: 第1列(X), 第2列(Y1), 第3列(Err1), 第4列(Y2), 第5列(Err2)")
        
        data_count = 0
        for i, line in enumerate(lines, 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
                
            parts = line.split()
            if len(parts) >= 5:
                try:
                    point = DataPoint(
                        id=len(self.data_points),
                        x=float(parts[0]),
                        y1=float(parts[1]),
                        err1=float(parts[2]),
                        y2=float(parts[3]),
                        err2=float(parts[4]),
                        selected=False
                    )
                    self.data_points.append(point)
                    data_count += 1
                    
                    # 显示前几个数据点
                    if data_count <= 3:
                        print(f"点{point.id}: X={point.x:.1f}, "
                              f"Y1={point.y1:.6f}±{point.err1:.6f}, "
                              f"Y2={point.y2:.6f}±{point.err2:.6f}")
                except ValueError:
                    print(f"警告: 第{i}行格式错误: {line}")
        
        if not self.data_points:
            print("错误: 文件中没有有效数据")
            return False
        
        print(f"成功读取 {len(self.data_points)} 个数据点")
        print(f"默认使用: Y列={self.current_y_col}")
        return True
    
    def weighted_linear_fit(self, x: List[float], y: List[float], 
                           yerr: List[float]) -> Dict:
        """加权线性拟合，返回字典格式的结果"""
        n = len(x)
        if n < 2:
            raise ValueError("至少需要2个数据点进行拟合")
        
        # 转换为numpy数组
        x_arr = np.array(x)
        y_arr = np.array(y)
        yerr_arr = np.array(yerr)
        
        # 计算权重 (w = 1/σ²)
        w = 1.0 / (yerr_arr ** 2)
        
        # 计算加权统计量
        S = np.sum(w)
        Sx = np.sum(w * x_arr)
        Sy = np.sum(w * y_arr)
        Sxx = np.sum(w * x_arr * x_arr)
        Sxy = np.sum(w * x_arr * y_arr)
        
        # 计算拟合参数
        delta = S * Sxx - Sx * Sx
        intercept = (Sxx * Sy - Sx * Sxy) / delta
        slope = (S * Sxy - Sx * Sy) / delta
        intercept_err = np.sqrt(Sxx / delta)
        slope_err = np.sqrt(S / delta)
        
        # 计算卡方值
        y_fit = intercept + slope * x_arr
        chi2 = np.sum(((y_arr - y_fit) / yerr_arr) ** 2)
        
        # 计算R²
        y_mean = np.mean(y_arr)
        ss_total = np.sum((y_arr - y_mean) ** 2)
        ss_residual = np.sum((y_arr - y_fit) ** 2)
        r2 = 1.0 - ss_residual / ss_total
        
        # 计算半衰期 (如果Y是ln(计数率)且斜率为负)
        half_life = None
        half_life_err = None
        if  slope < 0:
            lambda_val = -slope  # 衰变常数
            lambda_err = slope_err
            half_life = np.log(2) / lambda_val
            half_life_err = half_life * (lambda_err / lambda_val)
        
        return {
            'slope': slope,
            'slope_err': slope_err,
            'intercept': intercept,
            'intercept_err': intercept_err,
            'chi2': chi2,
            'r2': r2,
            'n_points': n,
            'half_life': half_life,
            'half_life_err': half_life_err
        }
    
    def get_current_values(self, point: DataPoint) -> Tuple[float, float]:
        """获取当前使用的Y值和误差"""
        if self.current_y_col == 2:
            return point.y1, point.err1
        else:
            return point.y2, point.err2
    
    def perform_sliding_window_fits(self, window_size: int = 10):
        """执行滑动窗口拟合：从第一个点开始，每次选window_size个点进行拟合"""
        n_points = len(self.data_points)
        
        if n_points < window_size:
            print(f"错误: 数据点数量({n_points})少于窗口大小({window_size})")
            return
        
        print(f"\n=== 执行滑动窗口拟合 ===")
        print(f"窗口大小: {window_size} 个点")
        print(f"数据总量: {n_points} 个点")
        print(f"总拟合次数: {n_points - window_size + 1}")
        print("-" * 80)
        
        self.window_fit_history.clear()
        
        # 表头
        print(f"{'窗口':^8} {'起始ID':^8} {'结束ID':^8} {'中间X':^12} {'斜率':^15} {'斜率误差':^12} "
              f"{'半衰期(小时)':^15} {'χ²/NDF':^10} {'R²':^8}")
        print("-" * 100)
        
        for start_idx in range(n_points - window_size + 1):
            end_idx = start_idx + window_size - 1
            
            # 收集当前窗口的数据
            x_vals, y_vals, y_errs = [], [], []
            for i in range(start_idx, end_idx + 1):
                point = self.data_points[i]
                y_val, y_err = self.get_current_values(point)
                x_vals.append(point.x)
                y_vals.append(y_val)
                y_errs.append(y_err)
            
            # 执行拟合
            try:
                result = self.weighted_linear_fit(x_vals, y_vals, y_errs)
                
                # 计算中间点的X值（第5个点，如果是10个点的窗口）
                middle_idx = start_idx + window_size // 2
                middle_x = self.data_points[middle_idx].x
                
                # 创建窗口拟合结果
                window_result = WindowFitResult(
                    start_id=start_idx,
                    end_id=end_idx,
                    middle_x=middle_x,
                    slope=result['slope'],
                    slope_err=result['slope_err'],
                    half_life=result['half_life'] / 24.0 if result['half_life'] else 0,
                    half_life_err=result['half_life_err'] / 24.0 if result['half_life_err'] else 0,
                    chi2=result['chi2'],
                    r2=result['r2']
                )
                
                self.window_fit_history.append(window_result)
                
                # 输出结果
                half_life_str = f"{result['half_life']/24.0:.2f}±{result['half_life_err']/24.0:.2f}" if result['half_life'] else "N/A"
                chi2_ndf = f"{result['chi2']:.1f}/{window_size-2}"
                
                print(f"{start_idx+1:^8} {start_idx:^8} {end_idx:^8} {middle_x:^12.1f} "
                      f"{result['slope']:^15.6e} {result['slope_err']:^12.6e} "
                      f"{half_life_str:^15} {chi2_ndf:^10} {result['r2']:^8.4f}")
                
            except Exception as e:
                print(f"窗口 {start_idx}-{end_idx} 拟合失败: {e}")
        
        # 显示统计信息
        self.show_window_fit_statistics()
        
        # 询问是否保存结果
        save_choice = input("\n是否保存滑动窗口拟合结果? (y/n): ").strip().lower()
        if save_choice == 'y':
            self.save_window_fit_results()
    
    def show_window_fit_statistics(self):
        """显示滑动窗口拟合的统计信息"""
        if not self.window_fit_history:
            print("没有滑动窗口拟合结果")
            return
        
        print(f"\n=== 滑动窗口拟合统计 ===")
        
        # 提取半衰期数据
        half_lives = [r.half_life for r in self.window_fit_history if r.half_life > 0]
        
        if half_lives:
            avg_half_life = np.mean(half_lives)
            std_half_life = np.std(half_lives)
            min_half_life = np.min(half_lives)
            max_half_life = np.max(half_lives)
            
            print(f"半衰期统计 (基于 {len(half_lives)} 个有效拟合):")
            print(f"  平均值: {avg_half_life:.2f} 小时")
            print(f"  标准差: {std_half_life:.2f} 小时")
            print(f"  最小值: {min_half_life:.2f} 小时")
            print(f"  最大值: {max_half_life:.2f} 小时")
            print(f"  变化范围: {max_half_life - min_half_life:.2f} 小时")
        
        # 提取斜率数据
        slopes = [r.slope for r in self.window_fit_history]
        avg_slope = np.mean(slopes)
        std_slope = np.std(slopes)
        
        print(f"\n斜率统计:")
        print(f"  平均值: {avg_slope:.6e}")
        print(f"  标准差: {std_slope:.6e}")
    
    def save_window_fit_results(self, filename: str = "window_fit_results.txt"):
        """保存滑动窗口拟合结果"""
        if not self.window_fit_history:
            print("错误: 没有滑动窗口拟合结果")
            return
        
        try:
            with open(filename, 'w', encoding='utf-8') as f:
                f.write("# 滑动窗口拟合结果\n")
                f.write(f"# 窗口大小: {self.window_fit_history[0].end_id - self.window_fit_history[0].start_id + 1} 个点\n")
                f.write(f"# 使用的Y列: {self.current_y_col}\n")
                f.write(f"# 总拟合次数: {len(self.window_fit_history)}\n")
                f.write("#\n")
                f.write("# 窗口 起始ID 结束ID 中间X值 斜率 斜率误差 半衰期(小时) 半衰期误差 χ² NDF R²\n")
                f.write("#" * 100 + "\n")
                
                for i, result in enumerate(self.window_fit_history, 1):
                    ndf = result.end_id - result.start_id - 1
                    f.write(f"{i:4d} {result.start_id:6d} {result.end_id:6d} {result.middle_x:10.2f} "
                           f"{result.slope:12.6e} {result.slope_err:12.6e} "
                           f"{result.half_life:10.2f} {result.half_life_err:10.2f} "
                           f"{result.chi2:8.2f} {ndf:4d} {result.r2:8.4f}\n")
            
            print(f"滑动窗口拟合结果已保存到: {filename}")
            
            # 同时保存为CSV格式
            csv_filename = filename.replace('.txt', '.csv')
            self.save_window_fit_csv(csv_filename)
            
        except Exception as e:
            print(f"保存失败: {e}")
    
    def save_window_fit_csv(self, filename: str = "window_fit_results.csv"):
        """保存滑动窗口拟合结果为CSV格式"""
        if not self.window_fit_history:
            return
        
        try:
            data = []
            for i, result in enumerate(self.window_fit_history, 1):
                ndf = result.end_id - result.start_id - 1
                data.append({
                    'Window': i,
                    'Start_ID': result.start_id,
                    'End_ID': result.end_id,
                    'Middle_X': result.middle_x,
                    'Slope': result.slope,
                    'Slope_Error': result.slope_err,
                    'Half_Life_Hours': result.half_life,
                    'Half_Life_Error': result.half_life_err,
                    'Chi2': result.chi2,
                    'NDF': ndf,
                    'R2': result.r2
                })
            
            df = pd.DataFrame(data)
            df.to_csv(filename, index=False, encoding='utf-8')
            print(f"CSV格式结果已保存到: {filename}")
            
        except Exception as e:
            print(f"保存CSV失败: {e}")
    
    def plot_window_fit_results(self):
        """绘制滑动窗口拟合结果图"""
        if not self.window_fit_history:
            print("错误: 没有滑动窗口拟合结果")
            return
        
        # 创建图形
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        # 子图1: 半衰期随窗口位置的变化
        ax1 = axes[0, 0]
        middle_x = [r.middle_x for r in self.window_fit_history]
        half_lives = [r.half_life for r in self.window_fit_history]
        half_life_errs = [r.half_life_err for r in self.window_fit_history]
        
        ax1.errorbar(middle_x, half_lives, yerr=half_life_errs, fmt='o-', 
                     color='blue', ecolor='red', capsize=3, linewidth=1)
        ax1.set_xlabel('中间点X值', fontsize=12)
        ax1.set_ylabel('半衰期 (小时)', fontsize=12)
        ax1.set_title('半衰期随窗口位置的变化', fontsize=14)
        ax1.grid(True, alpha=0.3)
        
        # 计算并绘制平均线
        if half_lives:
            avg_half_life = np.mean(half_lives)
            ax1.axhline(y=avg_half_life, color='green', linestyle='--', 
                       linewidth=2, label=f'平均值: {avg_half_life:.1f} h')
            ax1.legend()
        
        # 子图2: 斜率随窗口位置的变化
        ax2 = axes[0, 1]
        slopes = [r.slope for r in self.window_fit_history]
        slope_errs = [r.slope_err for r in self.window_fit_history]
        
        ax2.errorbar(middle_x, slopes, yerr=slope_errs, fmt='s-', 
                     color='green', ecolor='orange', capsize=3, linewidth=1)
        ax2.set_xlabel('中间点X值', fontsize=12)
        ax2.set_ylabel('斜率', fontsize=12)
        ax2.set_title('斜率随窗口位置的变化', fontsize=14)
        ax2.grid(True, alpha=0.3)
        
        # 子图3: R²随窗口位置的变化
        ax3 = axes[1, 0]
        r2_values = [r.r2 for r in self.window_fit_history]
        
        ax3.plot(middle_x, r2_values, 'o-', color='purple', linewidth=2)
        ax3.set_xlabel('中间点X值', fontsize=12)
        ax3.set_ylabel('R²', fontsize=12)
        ax3.set_title('拟合质量 (R²) 随窗口位置的变化', fontsize=14)
        ax3.grid(True, alpha=0.3)
        ax3.set_ylim([0, 1.1])
        
        # 子图4: 半衰期分布直方图
        ax4 = axes[1, 1]
        if half_lives:
            ax4.hist(half_lives, bins=10, edgecolor='black', alpha=0.7)
            ax4.set_xlabel('半衰期 (小时)', fontsize=12)
            ax4.set_ylabel('频率', fontsize=12)
            ax4.set_title('半衰期分布直方图', fontsize=14)
            ax4.grid(True, alpha=0.3)
            
            # 添加统计信息
            stats_text = f'平均值: {np.mean(half_lives):.1f} h\n'
            stats_text += f'标准差: {np.std(half_lives):.1f} h\n'
            stats_text += f'最小值: {np.min(half_lives):.1f} h\n'
            stats_text += f'最大值: {np.max(half_lives):.1f} h'
            
            props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
            ax4.text(0.05, 0.95, stats_text, transform=ax4.transAxes,
                    fontsize=10, verticalalignment='top', bbox=props)
        
        plt.suptitle(f'滑动窗口拟合分析 (窗口大小: {self.window_fit_history[0].end_id - self.window_fit_history[0].start_id + 1} 个点)', 
                    fontsize=16)
        plt.tight_layout()
        
        # 保存图像
        plt.savefig('window_fit_analysis.png', dpi=150, bbox_inches='tight')
        plt.savefig('window_fit_analysis.pdf', bbox_inches='tight')
        plt.show()
        
        print(f"\n滑动窗口分析图像已保存为 window_fit_analysis.png 和 window_fit_analysis.pdf")
    
    # 以下是原有的方法，为了保持完整性都保留
    
    def show_all_data(self):
        """显示所有数据点"""
        print(f"\n=== 所有数据点 (使用Y列={self.current_y_col}) ===")
        print("ID\tX\t\tY\t\tErr\t\tY另一列\t\t选中")
        print("-" * 80)
        
        for point in self.data_points:
            y_curr, err_curr = self.get_current_values(point)
            y_other = point.y2 if self.current_y_col == 2 else point.y1
            err_other = point.err2 if self.current_y_col == 2 else point.err1
            
            selected = "[X]" if point.selected else "[ ]"
            
            print(f"{point.id:2d}\t{point.x:7.1f}\t"
                  f"{y_curr:10.6f}\t{err_curr:10.6f}\t"
                  f"{y_other:10.6f}\t{selected}")
    
    def show_selected_data(self):
        """显示选中的数据点"""
        selected_points = [p for p in self.data_points if p.selected]
        
        print(f"\n=== 当前选中的数据点 (使用Y列={self.current_y_col}) ===")
        
        if not selected_points:
            print("没有选中的数据点")
            return
        
        for point in selected_points:
            y_val, y_err = self.get_current_values(point)
            print(f"点{point.id}: x={point.x:7.1f}, "
                  f"y={y_val:10.6f} ± {y_err:10.6f}")
        
        print(f"共选中 {len(selected_points)} 个数据点")
    
    def toggle_point(self, point_id: int):
        """选择/取消选择数据点"""
        if 0 <= point_id < len(self.data_points):
            self.data_points[point_id].selected = not self.data_points[point_id].selected
            status = "已选中" if self.data_points[point_id].selected else "已取消"
            print(f"点{point_id} {status}")
        else:
            print(f"错误: ID {point_id} 无效 (有效范围: 0-{len(self.data_points)-1})")
    
    def select_range(self, start_id: int, end_id: int):
        """选择范围内的数据点"""
        if (start_id < 0 or end_id >= len(self.data_points) or 
            start_id > end_id):
            print(f"错误: 范围无效 (有效ID: 0-{len(self.data_points)-1})")
            return
        
        count = 0
        for i in range(start_id, end_id + 1):
            if not self.data_points[i].selected:
                self.data_points[i].selected = True
                count += 1
        
        print(f"已选中点 {start_id} 到 {end_id} (共{count}个点)")
    
    def select_by_x_range(self, x_min: float, x_max: float):
        """根据X值范围选择数据点"""
        count = 0
        for point in self.data_points:
            if x_min <= point.x <= x_max:
                if not point.selected:
                    point.selected = True
                    count += 1
        
        print(f"已选中X值在 [{x_min}, {x_max}] 范围内的 {count} 个数据点")
    
    def clear_selection(self):
        """清空所有选择"""
        for point in self.data_points:
            point.selected = False
        print("已清空所有选择")
    
    def perform_fit(self) -> Optional[FitResult]:
        """执行线性拟合"""
        # 收集选中的数据点
        x_vals, y_vals, y_errs = [], [], []
        selected_ids = []
        
        for point in self.data_points:
            if point.selected:
                y_val, y_err = self.get_current_values(point)
                x_vals.append(point.x)
                y_vals.append(y_val)
                y_errs.append(y_err)
                selected_ids.append(point.id)
        
        if len(x_vals) < 2:
            print("错误: 至少需要选择2个数据点进行拟合")
            return None
        
        print(f"\n=== 执行线性拟合 ===")
        print(f"使用 {len(x_vals)} 个数据点 (Y列={self.current_y_col})")
        
        try:
            result_dict = self.weighted_linear_fit(x_vals, y_vals, y_errs)
            
            # 转换为FitResult对象
            result = FitResult(
                slope=result_dict['slope'],
                slope_err=result_dict['slope_err'],
                intercept=result_dict['intercept'],
                intercept_err=result_dict['intercept_err'],
                chi2=result_dict['chi2'],
                r2=result_dict['r2'],
                selected_ids=selected_ids,
                y_column=self.current_y_col,
                half_life=result_dict['half_life'],
                half_life_err=result_dict['half_life_err']
            )
            
            # 显示拟合结果
            print(f"\n=== 拟合结果 ===")
            print(f"方程: y = a + b·x")
            print(f"截距 a = {result.intercept:.6e} ± {result.intercept_err:.6e}")
            print(f"斜率 b = {result.slope:.6e} ± {result.slope_err:.6e}")
            print(f"卡方 χ² = {result.chi2:.2f}")
            print(f"自由度 NDF = {len(x_vals) - 2}")
            print(f"χ²/NDF = {result.chi2/(len(x_vals)-2):.2f}")
            print(f"相关系数 R² = {result.r2:.6f}")
            
            # 如果Y是ln(计数率)，计算半衰期
            if self.current_y_col == 2 and result.slope < 0:
                lambda_val = -result.slope  # 衰变常数
                lambda_err = result.slope_err
                half_life = np.log(2) / lambda_val
                half_life_err = half_life * (lambda_err / lambda_val)
                
                print(f"\n=== 半衰期估计 ===")
                print(f"衰变常数 λ = {lambda_val:.6e} ± {lambda_err:.6e} /单位")
                print(f"半衰期 T_{{1/2}} = ln(2)/λ = {half_life:.2f} ± {half_life_err:.2f} 单位")
                print(f"转换为小时: {half_life/24.0:.2f} ± {half_life_err/24.0:.2f} 小时")
            
            # 保存到历史记录
            self.fit_history.append(result)
            
            return result
            
        except Exception as e:
            print(f"拟合失败: {e}")
            return None
    
    def plot_fit(self, result: FitResult):
        """绘制拟合图像"""
        if not result or not result.selected_ids:
            print("错误: 没有可绘制的拟合结果")
            return
        
        # 收集数据
        x_vals, y_vals, y_errs = [], [], []
        for point in self.data_points:
            if point.id in result.selected_ids:
                y_val, y_err = self.get_current_values(point)
                x_vals.append(point.x)
                y_vals.append(y_val)
                y_errs.append(y_err)
        
        # 计算拟合线
        x_min, x_max = min(x_vals), max(x_vals)
        x_range = x_max - x_min
        x_fit = np.linspace(x_min - 0.1*x_range, x_max + 0.1*x_range, 100)
        y_fit = result.intercept + result.slope * x_fit
        
        # 创建图形
        plt.figure(figsize=(10, 6))
        
        # 绘制数据点和误差条
        plt.errorbar(x_vals, y_vals, yerr=y_errs, fmt='o', 
                     color='blue', ecolor='red', elinewidth=1, 
                     capsize=3, label='数据点')
        
        # 绘制拟合线
        plt.plot(x_fit, y_fit, 'r-', linewidth=2, label='线性拟合')
        
        # 设置图形属性
        plt.xlabel('X (第1列)', fontsize=12)
        plt.ylabel(f'Y (第{self.current_y_col}列)', fontsize=12)
        plt.title(f'线性拟合结果 (Y列={self.current_y_col})', fontsize=14)
        plt.grid(True, alpha=0.3)
        
        # 添加图例
        plt.legend(fontsize=10)
        
        # 添加拟合结果文本框
        textstr = '\n'.join((
            f'斜率 = {result.slope:.6e} ± {result.slope_err:.6e}',
            f'截距 = {result.intercept:.6e} ± {result.intercept_err:.6e}',
            f'χ²/NDF = {result.chi2:.2f}/{len(x_vals)-2}',
            f'R² = {result.r2:.6f}'
        ))
        
        if result.half_life:
            textstr += f'\nT$_{{1/2}}$ = {result.half_life/24.0:.2f} 小时'
        
        props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
        plt.text(0.05, 0.95, textstr, transform=plt.gca().transAxes,
                fontsize=10, verticalalignment='top', bbox=props)
        
        # 调整布局并保存
        plt.tight_layout()
        plt.savefig('fit_result.png', dpi=300, bbox_inches='tight')
        plt.savefig('fit_result.pdf', bbox_inches='tight')
        plt.show()
        
        print(f"\n图像已保存为 fit_result.png 和 fit_result.pdf")
    
    def show_fit_history(self):
        """显示拟合历史"""
        print("\n=== 拟合历史记录 ===")
        
        if not self.fit_history:
            print("暂无拟合记录")
            return
        
        for i, result in enumerate(self.fit_history, 1):
            print(f"\n拟合 #{i}:")
            print(f"  使用的列: Y{result.y_column}")
            print(f"  数据点: {result.selected_ids} (共{len(result.selected_ids)}个)")
            print(f"  斜率 = {result.slope:.6e} ± {result.slope_err:.6e}")
            print(f"  截距 = {result.intercept:.6e} ± {result.intercept_err:.6e}")
            print(f"  R² = {result.r2:.6f}")
            print(f"  χ² = {result.chi2:.2f}")
    
    def save_results(self, filename: str = "fit_results.txt"):
        """保存拟合结果到文件"""
        if not self.fit_history:
            print("错误: 没有拟合结果可保存")
            return
        
        result = self.fit_history[-1]
        
        try:
            with open(filename, 'w', encoding='utf-8') as f:
                f.write("# 五列数据线性拟合结果 (Python版本)\n")
                f.write(f"# 生成时间: {np.datetime64('now')}\n")
                f.write(f"# 使用的列: Y列={result.y_column}\n")
                f.write("#\n")
                f.write(f"# 选中的数据点ID: {result.selected_ids}\n")
                f.write(f"# 数据点数量: {len(result.selected_ids)}\n")
                f.write("#\n")
                f.write(f"斜率 = {result.slope:.6e} ± {result.slope_err:.6e}\n")
                f.write(f"截距 = {result.intercept:.6e} ± {result.intercept_err:.6e}\n")
                f.write(f"卡方值 χ² = {result.chi2:.2f}\n")
                f.write(f"自由度 NDF = {len(result.selected_ids) - 2}\n")
                f.write(f"χ²/NDF = {result.chi2/(len(result.selected_ids)-2):.2f}\n")
                f.write(f"相关系数 R² = {result.r2:.6f}\n")
                f.write("#\n")
                f.write("# 数据点详细信息:\n")
                f.write(f"# ID\tX(1)\tY{result.y_column}\tErr\t"
                       f"Y{4 if result.y_column==2 else 2}\tErr\n")
                
                for point_id in result.selected_ids:
                    point = self.data_points[point_id]
                    y_val, y_err = (point.y1, point.err1) if result.y_column==2 else (point.y2, point.err2)
                    y_other, err_other = (point.y2, point.err2) if result.y_column==2 else (point.y1, point.err1)
                    
                    f.write(f"{point.id}\t{point.x}\t{y_val}\t{y_err}\t"
                           f"{y_other}\t{err_other}\n")
            
            print(f"\n拟合结果已保存到 {filename}")
            
        except Exception as e:
            print(f"保存文件失败: {e}")
    
    def save_to_json(self, filename: str = "fit_results.json"):
        """保存拟合结果到JSON文件"""
        if not self.fit_history:
            print("错误: 没有拟合结果可保存")
            return
        
        result = self.fit_history[-1]
        
        # 准备数据
        data = {
            "metadata": {
                "timestamp": str(np.datetime64('now')),
                "y_column_used": result.y_column,
                "n_points": len(result.selected_ids)
            },
            "fit_results": {
                "slope": result.slope,
                "slope_error": result.slope_err,
                "intercept": result.intercept,
                "intercept_error": result.intercept_err,
                "chi2": result.chi2,
                "r2": result.r2,
                "selected_point_ids": result.selected_ids
            },
            "data_points": []
        }
        
        # 添加数据点信息
        for point_id in result.selected_ids:
            point = self.data_points[point_id]
            y_val, y_err = (point.y1, point.err1) if result.y_column==2 else (point.y2, point.err2)
            y_other, err_other = (point.y2, point.err2) if result.y_column==2 else (point.y1, point.err1)
            
            data["data_points"].append({
                "id": point.id,
                "x": point.x,
                f"y{result.y_column}": y_val,
                f"err{result.y_column}": y_err,
                f"y{4 if result.y_column==2 else 2}": y_other,
                f"err{4 if result.y_column==2 else 2}": err_other
            })
        
        try:
            with open(filename, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2, ensure_ascii=False)
            print(f"结果已保存为JSON格式: {filename}")
        except Exception as e:
            print(f"保存JSON文件失败: {e}")
    
    def run_interactive(self):
        """运行交互式界面"""
        print("\n" + "="*60)
        print("      五列数据交互式线性拟合程序 (增强版)")
        print("      新增滑动窗口拟合功能")
        print("="*60)
        
        while True:
            print(f"\n当前配置: Y列={self.current_y_col}, 数据点={len(self.data_points)}")
            print("-" * 60)
            print("1. 显示所有数据点")
            print("2. 显示选中的数据点")
            print("3. 选择/取消选择数据点 (输入ID)")
            print("4. 选择数据点范围 (输入起始ID和结束ID)")
            print("5. 根据X值范围选择数据点")
            print("6. 清空所有选择")
            print("7. 切换数据列 (输入Y列: 2或4)")
            print("8. 执行线性拟合 (当前选择)")
            print("9. 绘制拟合图像")
            print("10. 显示拟合历史")
            print("11. 保存拟合结果 (文本)")
            print("12. 保存拟合结果 (JSON)")
            print("13. 执行滑动窗口拟合 (自动)")
            print("14. 显示滑动窗口拟合统计")
            print("15. 绘制滑动窗口分析图")
            print("0. 退出程序")
            print("-" * 60)
            
            try:
                choice = input("请输入选项 (0-15): ").strip()
                
                if choice == "0":
                    print("程序结束")
                    break
                
                elif choice == "1":
                    self.show_all_data()
                
                elif choice == "2":
                    self.show_selected_data()
                
                elif choice == "3":
                    try:
                        point_id = int(input("请输入数据点ID: "))
                        self.toggle_point(point_id)
                    except ValueError:
                        print("错误: 请输入有效的数字ID")
                
                elif choice == "4":
                    try:
                        start_id = int(input("请输入起始ID: "))
                        end_id = int(input("请输入结束ID: "))
                        self.select_range(start_id, end_id)
                    except ValueError:
                        print("错误: 请输入有效的数字")
                
                elif choice == "5":
                    try:
                        x_min = float(input("请输入X值最小值: "))
                        x_max = float(input("请输入X值最大值: "))
                        self.select_by_x_range(x_min, x_max)
                    except ValueError:
                        print("错误: 请输入有效的数字")
                
                elif choice == "6":
                    self.clear_selection()
                
                elif choice == "7":
                    try:
                        y_col = int(input("请输入Y列 (2或4): "))
                        if y_col in [2, 4]:
                            self.current_y_col = y_col
                            print(f"已切换到Y列: {y_col}")
                        else:
                            print("错误: Y列必须是2或4")
                    except ValueError:
                        print("错误: 请输入有效的数字")
                
                elif choice == "8":
                    result = self.perform_fit()
                    if result:
                        # 询问是否绘图
                        plot_choice = input("是否绘制拟合图像? (y/n): ").strip().lower()
                        if plot_choice == 'y':
                            self.plot_fit(result)
                
                elif choice == "9":
                    if self.fit_history:
                        self.plot_fit(self.fit_history[-1])
                    else:
                        print("错误: 请先执行拟合")
                
                elif choice == "10":
                    self.show_fit_history()
                
                elif choice == "11":
                    self.save_results()
                
                elif choice == "12":
                    self.save_to_json()
                
                elif choice == "13":
                    # 滑动窗口拟合
                    try:
                        window_size = input("请输入窗口大小 (默认10): ").strip()
                        window_size = int(window_size) if window_size else 10
                        if window_size < 2:
                            print("错误: 窗口大小至少为2")
                        else:
                            self.perform_sliding_window_fits(window_size)
                    except ValueError:
                        print("错误: 请输入有效的数字")
                
                elif choice == "14":
                    self.show_window_fit_statistics()
                
                elif choice == "15":
                    self.plot_window_fit_results()
                
                else:
                    print("无效选项，请重新输入")
                    
            except KeyboardInterrupt:
                print("\n\n程序被用户中断")
                break
            except Exception as e:
                print(f"发生错误: {e}")

def create_example_data(filename: str = "data.txt"):
    """创建示例数据文件"""
    example_data = """# 五列数据示例
# X Y1 Err1 Y2 Err2
54.6 8.85955827 0.01438399 8.694236779 0.035721529
90.8 8.599189404 0.016458143 8.472638529 0.040279711
114.3 8.464164344 0.016418253 8.325068688 0.040743729
138.2 8.299187482 0.018213779 8.185462594 0.044739616
162.2 8.119035162 0.019089099 8.035477798 0.046443269
186.2 7.987544056 0.020700608 7.876571888 0.051226095
210.4 7.856649849 0.015702288 7.63448533 0.042724413
234.7 7.694312912 0.015721713 7.554162143 0.039717774
258.9 7.533729105 0.015669516 7.41652257 0.038937409
283.0 7.371302516 0.015560343 7.230810994 0.03965385
307.1 7.206050004 0.018356396 7.077700413 0.046725393
332.6 7.036579714 0.0209339 6.857361555 0.056651814
354.6 6.896657648 0.020889451 6.751364299 0.054847841
"""
    
    try:
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(example_data)
        print(f"已创建示例数据文件: {filename}")
        print("前3行数据:")
        for i, line in enumerate(example_data.strip().split('\n')):
            if i < 4:
                print(f"  {line}")
    except Exception as e:
        print(f"创建示例数据文件失败: {e}")

def main():
    """主函数"""
    print("="*60)
    print("    五列数据交互式线性拟合程序 (增强版)")
    print("    新增滑动窗口自动拟合功能")
    print("="*60)
    
    fitter = InteractiveFitter()
    
    # 获取数据文件名
    default_file = "data.txt"
    filename = input(f"请输入数据文件名 (默认: {default_file}): ").strip()
    if not filename:
        filename = default_file
    
    # 检查文件是否存在，不存在则创建示例
    if not os.path.exists(filename):
        print(f"文件 '{filename}' 不存在")
        create = input("是否创建示例数据文件? (y/n): ").strip().lower()
        if create == 'y':
            create_example_data(filename)
        else:
            print("程序退出")
            return
    
    # 加载数据
    if not fitter.load_data(filename):
        print("数据加载失败，程序退出")
        return
    
    # 运行交互式界面
    fitter.run_interactive()

if __name__ == "__main__":
    # 检查必要的Python库
    required_libs = ['numpy', 'matplotlib']
    missing_libs = []
    
    for lib in required_libs:
        try:
            __import__(lib)
        except ImportError:
            missing_libs.append(lib)
    
    if missing_libs:
        print("错误: 缺少必要的Python库")
        print(f"请安装: pip install {' '.join(missing_libs)}")
        sys.exit(1)
    
    main()
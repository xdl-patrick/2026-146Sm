import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import uproot
import os
import warnings

# 忽略部分拟合警告，保持控制台整洁
warnings.filterwarnings('ignore')

# 1. 定义高斯函数模型: f(x) = A * exp(-(x - mu)^2 / (2 * sigma^2)) + B
def gaussian(x, A, mu, sigma, B):
    return A * np.exp(-(x - mu)**2 / (2 * sigma**2)) + B

# 2. 设置参数
file_pattern = "2026_146Sm{}.root"
channels = range(85, 91)  # 85 到 90
x_range = (0, 1000)       # 拟合的道址范围

# 用于收集结果和绘图
results = []
fig, ax = plt.subplots(figsize=(14, 8))
colors = plt.cm.tab10(np.linspace(0, 1, len(channels)))

# 3. 循环处理每个文件
for idx, ch in enumerate(channels):
    filename = file_pattern.format(ch)
    
    # 检查文件是否存在
    if not os.path.exists(filename):
        print(f"[Warning] File not found: {filename}")
        continue
        
    # 读取 ROOT 文件 (假设直方图名为 'h1'，请根据实际ROOT文件结构修改)
    try:
        with uproot.open(filename) as f:
            # 自动寻找第一个 TH1 对象，或者指定名称如 f['h1']
            hist_name = f.keys(filter_classname="TH1*")[0]
            hist = f[hist_name]
            counts = hist.values()
            edges = hist.axis().edges()
            x_data = (edges[:-1] + edges[1:]) / 2.0
    except Exception as e:
        print(f"[Error] Failed to read {filename}: {e}")
        continue

    # 截取 0-1000 道数据
    mask = (x_data >= x_range[0]) & (x_data <= x_range[1])
    x_fit = x_data[mask]
    y_fit = counts[mask]

    # 4. 初始参数猜测 (基于数据最大值和质心)
    max_idx = np.argmax(y_fit)
    p0 = [y_fit[max_idx], x_fit[max_idx], 10.0, np.min(y_fit)] 

    # 5. 执行高斯拟合
    try:
        popt, pcov = curve_fit(gaussian, x_fit, y_fit, p0=p0, maxfev=5000)
        A, mu, sigma, B = popt
        
        # 计算半高宽 FWHM = 2 * sqrt(2 * ln(2)) * sigma ≈ 2.3548 * sigma
        fwhm = 2.3548 * abs(sigma)
        
        results.append({"File": filename, "Peak_Pos": mu, "FWHM": fwhm})
        
        # 【核心修改】绘制原始数据与拟合曲线，完全使用原始坐标，不做任何对齐处理
        label = f"Sm{ch} (Pos={mu:.2f}, FWHM={fwhm:.2f})"
        ax.plot(x_fit, y_fit, 'o', markersize=2, color=colors[idx], alpha=0.6)
        ax.plot(x_fit, gaussian(x_fit, *popt), '-', color=colors[idx], linewidth=2, label=label)
        
    except Exception as e:
        print(f"[Warning] Fit failed for {filename}: {e}")

# 6. 打印拟合结果表格
print("\n" + "="*50)
print(f"{'File':<25} {'Peak Pos':<12} {'FWHM':<12}")
print("="*50)
for res in results:
    print(f"{res['File']:<25} {res['Peak_Pos']:<12.4f} {res['FWHM']:<12.4f}")
print("="*50 + "\n")

# 7. 完善图表并【保存为图片】
ax.set_xlabel("Channel", fontsize=14)
ax.set_ylabel("Counts", fontsize=14)
ax.set_title("Gaussian Fit for 2026_146Sm (Ch 85-90)", fontsize=16)
ax.legend(fontsize=9, loc='best')
ax.grid(True, linestyle='--', alpha=0.5)

# 限制X轴严格在0-1000道范围内，保持原始数据样貌
ax.set_xlim(0, 1000) 

plt.tight_layout()

# 保存图像到当前目录
save_path = "gaussian_fit_2026_146Sm_85to90.png"
plt.savefig(save_path, dpi=300)
print(f"[Success] Plot successfully saved to: {save_path}")

# 如果需要弹出窗口查看，可取消下面这行的注释：
plt.show() 
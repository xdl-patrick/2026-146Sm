import ROOT
import os

def fit_and_plot_adc_channels(root_file_path, channels):
    # 1. 检查文件是否存在
    if not os.path.exists(root_file_path):
        print(f"错误: 找不到文件 {root_file_path}")
        return

    # 2. 打开 ROOT 文件
    root_file = ROOT.TFile.Open(root_file_path, "READ")
    if not root_file or root_file.IsZombie():
        print(f"错误: 无法打开文件 {root_file_path}")
        return

    # 创建一个 2x2 的画布，用于同时展示4个通道的能谱
    canvas = ROOT.TCanvas("c_adc_channels", "ADC Channels 12-15 Spectra", 1200, 1000)
    canvas.Divide(2, 2)

    # 3. 遍历指定的四个通道
    for i, channel_name in enumerate(channels):
        # 获取直方图对象 (假设对象在文件根目录下，若在其他目录请修改路径)
        hist = root_file.Get(channel_name)
        
        if not hist:
            print(f"警告: 在文件中未找到对象 '{channel_name}'，跳过该通道。")
            continue
            
        # 克隆直方图以避免修改原文件中的对象
        plot_hist = hist.Clone(f"{channel_name}_clone")
        
        # 切换到当前画布子区域并绘制直方图
        canvas.cd(i + 1)
        plot_hist.SetTitle(f"{channel_name} Spectrum with Gaussian Fit")
        plot_hist.GetXaxis().SetTitle("ADC Channel / Energy")
        plot_hist.GetYaxis().SetTitle("Counts")
        plot_hist.Draw("HIST")

        # 4. 创建高斯拟合函数并进行拟合
        # 使用直方图的均值和RMS作为高斯函数的初始参数
        mean = plot_hist.GetMean()
        sigma = plot_hist.GetRMS()
        
        # 定义高斯函数，范围覆盖整个直方图
        fit_func = ROOT.TF1(f"gaus_fit_{channel_name}", "gaus", plot_hist.GetXaxis().GetXmin(), plot_hist.GetXaxis().GetXmax())
        fit_func.SetParameters(plot_hist.GetMaximum(), mean, sigma)
        
        # 执行拟合 ("R" 表示在函数定义范围内拟合, "+" 表示将拟合曲线画在当前图上)
        plot_hist.Fit(fit_func, "R+")
        
        # 更新画布
        ROOT.gPad.Update()

    # 5. 显示最终画布
    canvas.Draw()
    root_file.Close()

# ================= 主程序入口 =================
if __name__ == "__main__":
    # 指定 ROOT 文件路径
    file_name = "2026_146Sm46.root"
    
    # 指定需要绘制和拟合的四个通道名称
    target_channels = ["adc4ch12", "adc4ch13", "adc4ch14", "adc4ch15"]
    
    # 执行绘制与拟合
    fit_and_plot_adc_channels(file_name, target_channels)
    
    # 保持窗口打开，直到用户手动关闭
    input("按回车键退出程序...")
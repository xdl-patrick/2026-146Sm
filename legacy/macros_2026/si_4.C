#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TString.h"
#include <iostream>

void si_4() {
    // 1. 指定 ROOT 文件路径
    TString fileName = "2026_146Sm46.root";
    
    // 2. 打开 ROOT 文件
    TFile *file = TFile::Open(fileName, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "错误: 无法打开文件 " << fileName << std::endl;
        return;
    }

    // 3. 【修改】只保留最后两个通道名称
    const char* channels[] = {"adc4ch14", "adc4ch15"};
    int nChannels = 2;

    // 4. 【修改】创建一个 1x2 的画布（一行两列）
    TCanvas *c1 = new TCanvas("c_adc_channels", "ADC Channels 14-15 Spectra", 1200, 600);
    c1->Divide(2, 1); 

    // 5. 遍历通道进行绘制和拟合
    for (int i = 0; i < nChannels; ++i) {
        // 获取直方图对象
        TH1F *hist = (TH1F*)file->Get(channels[i]);
        
        if (!hist) {
            std::cerr << "警告: 在文件中未找到对象 '" << channels[i] << "'，跳过该通道。" << std::endl;
            continue;
        }

        // 切换到当前画布子区域
        c1->cd(i + 1);
        
        // 克隆直方图以避免修改原文件中的对象
        TH1F *plotHist = (TH1F*)hist->Clone(TString::Format("%s_clone", channels[i]));
        plotHist->SetTitle(TString::Format("%s Spectrum with Gaussian Fit", channels[i]));
        plotHist->GetXaxis()->SetTitle("ADC Channel / Energy");
        plotHist->GetYaxis()->SetTitle("Counts");
        plotHist->Draw("HIST");

        // 6. 创建高斯拟合函数并进行拟合
        double mean = plotHist->GetMean();
        double sigma = plotHist->GetRMS();
        double maxVal = plotHist->GetMaximum();

        // 定义高斯函数，范围覆盖整个直方图
        TF1 *fitFunc = new TF1(TString::Format("gaus_fit_%s", channels[i]), 
                               "gaus", 
                               plotHist->GetXaxis()->GetXmin(), 
                               plotHist->GetXaxis()->GetXmax());
        
        // 设置初始参数：常数项(峰值), 均值, 标准差
        fitFunc->SetParameters(maxVal, mean, sigma);
        
        // 执行拟合 ("R" 表示在函数定义范围内拟合, "+" 表示将拟合曲线画在当前图上)
        plotHist->Fit(fitFunc, "R+");
    }

    // 7. 更新并显示画布
    c1->Update();
    
    // 保持窗口打开，直到用户手动关闭
    std::cout << "绘图完成。按回车键或关闭窗口退出..." << std::endl;
    std::cin.get();
    
    file->Close();
}
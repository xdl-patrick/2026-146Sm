#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TLegend.h"
#include <iostream>
void Plot_same_canvas() {
    // 1. 打开两个 ROOT 文件
    TFile *file1 = new TFile("2025_146Sm100.root", "READ");
    TFile *file2 = new TFile("2026_146Sm71.root", "READ");

    // 检查文件是否成功打开
    if (!file1 || file1->IsZombie() || !file2 || file2->IsZombie()) {
        std::cerr << "Error: Cannot open one or both ROOT files!" << std::endl;
        return;
    }

    // 2. 直接获取名为 "adc4ch12" 的直方图 (请根据实际需要更改通道号)
    TH1I *h1 = nullptr;
    TH1I *h2 = nullptr;
    file1->GetObject("adc0ch12", h1); 
    file2->GetObject("adc4ch15", h2); 

    if (!h1 || !h2) {
        std::cerr << "Error: Cannot find the specified histogram in the files!" << std::endl;
        return;
    }

    // 3. 设置图形样式以便区分
    h1->SetLineColor(kRed);       
    h1->SetLineWidth(2);          
    h2->SetLineColor(kBlue);      
    h2->SetLineStyle(2);          

    // 4. 创建画布并绘制
    TCanvas *c1 = new TCanvas("c1", "Overlay Plot", 800, 600);
    h1->Draw("HIST");             // 先绘制第一个直方图
    h2->Draw("HIST SAME");        // 使用 "SAME" 叠加第二个
    
    // 5. 添加图例
    auto legend = new TLegend(0.7, 0.75, 0.9, 0.9);
    legend->AddEntry(h1, "2025_146Sm100 - adc0ch12", "l");
    legend->AddEntry(h2, "2026_146Sm71 - adc4ch15", "l");
    legend->Draw();

    c1->Update();
}
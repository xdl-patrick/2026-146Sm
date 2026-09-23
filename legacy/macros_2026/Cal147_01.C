#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TLegend.h" // 显式包含图例头文件

void Cal147_01(const int run, const int Cmin, const int Cmax, const int num) {
    // 1. 文件与树的读取
    TString fin = TString::Format("2026_146Sm%d.root", run);
    TFile* fRun = new TFile(fin);
    if (!fRun || fRun->IsZombie()) {
        printf("Error opening file %s\n", fin.Data());
        return;
    }
    TTree *t = (TTree*)fRun->Get("Tree1");
    if (!t) {
        printf("Error getting tree Tree1\n");
        return;
    }

    // 2. 定义画布
    TCanvas *c1 = new TCanvas("myc_1", "Overlayed Histograms", 0, 0, 1400, 800);
    TCanvas *cTmp = new TCanvas("c", "c");
   //  c1->SetGrid(); 

    // 3. 定义颜色数组 (8种颜色)
    Color_t colors[8] = {kBlack, kRed, kBlue, kGreen, 
                         kMagenta, kCyan, kOrange, kViolet};

    // 4. 修正后的通道逻辑判断 (覆盖了原代码中 num=1 到 16 的所有情况)
    TString adcName;
    int baseCh = 0;
    bool validNum = true;

    // --- 第一组 ADC0 (对应 num 1, 2) ---
    if (num == 1) { adcName = "adc0ch"; baseCh = 0; }
    else if (num == 2) { adcName = "adc0ch"; baseCh = 8; }
    
    // --- 第二组 ADC1 (对应 num 5, 6) ---
    else if (num == 5) { adcName = "adc1ch"; baseCh = 0; }
    else if (num == 6) { adcName = "adc1ch"; baseCh = 8; }
    
    // --- 第三组 ADC2 (对应 num 9, 10) ---
    else if (num == 9) { adcName = "adc2ch"; baseCh = 0; }
    else if (num == 10) { adcName = "adc2ch"; baseCh = 8; }
    
    // --- 第四组 ADC3 (对应 num 13, 14, 15, 16) ---
    else if (num == 13) { adcName = "adc3ch"; baseCh = 0; }
    else if (num == 14) { adcName = "adc3ch"; baseCh = 8; }
    else if (num == 15) { adcName = "adc3ch"; baseCh = 16; } // 你之前的报错就是因为这里没包含 15
    else if (num == 16) { adcName = "adc3ch"; baseCh = 24; }
    else {
        printf("Error: Invalid num parameter: %d\n", num);
        printf("Valid values are: 1,2,5,6,9,10,13,14,15,16\n");
        validNum = false;
    }

    if (!validNum) return;

    // 5. 循环绘制 8 个通道
    TH1F *hist[8];
    for (int i = 0; i < 8; i++) {
        int ch = baseCh + i;
        TString histName = TString::Format("hx_%d", ch); // 直方图名字包含通道号，防止重名覆盖
        TString expression = TString::Format("%s%d - (0)", adcName.Data(), ch);
        
        hist[i] = new TH1F(histName, "", (Cmax-Cmin)/5, Cmin, Cmax);
        
        // 填充直方图
        cTmp->cd();
        t->Draw(TString::Format("%s >> %s", expression.Data(), histName.Data()));
        hist[i]->SetDirectory(0);
        hist[i]->SetLineColor(colors[i]);
        hist[i]->SetLineWidth(2);
        
         c1->cd();
        if (i == 0) {
            hist[i]->Draw("HIST");
            hist[i]->SetTitle(TString::Format("Overlay: %s %d-%d", adcName.Data(), baseCh, baseCh+7));
            hist[i]->GetXaxis()->SetTitle("ADC Value");
            hist[i]->GetYaxis()->SetTitle("Counts");
        } else {
            hist[i]->Draw("HIST SAME");
        }
    }

    // 6. 添加图例
    hist[0]->GetYaxis()->SetRangeUser(0, 50);
    TLegend *legend = new TLegend(0.75, 0.6, 0.88, 0.88);
    for (int i = 0; i < 8; i++) {
        legend->AddEntry(hist[i], TString::Format("Ch %d", baseCh + i), "l");
    }
    legend->SetBorderSize(0);
    legend->Draw();

    cTmp->Close();
    c1->Update();
}
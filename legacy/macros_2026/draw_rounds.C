#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <vector>
#include <iostream>

// 支持命令行传参: root -l 'draw_rounds.C(90, 5000)'
// 若不传参，则使用下方的默认值
void draw_rounds(Int_t manual_ymax = 3000) {
    // --- 配置区域 ---
    TString file_prefix = "/mnt/d/Sm147-2026/2026_146Sm"; 
    TString hist_name = "adc4ch12"; 
    Int_t start_round = 106;
    Int_t end_round = 110;
    Double_t x_min = 0.0;
    Double_t x_max = 3500.0;
    // ----------------

    TCanvas *c1 = new TCanvas("c1", Form("Rounds %d-%d", start_round, end_round), 800, 600);
    c1->SetGrid();

    Int_t colors[] = {kBlue, kRed, kGreen+2, kOrange-3, kViolet, kCyan+2};
    std::vector<TH1I*> cloned_hists; 

    for (int run = start_round; run <= end_round; run++) {
        TString filename = Form("%s%d.root", file_prefix.Data(), run);
        
        TFile *file = TFile::Open(filename);
        if (!file || file->IsZombie()) { 
            std::cout << "警告: 无法打开文件 " << filename << std::endl; 
            continue; 
        }

        TH1I *hist = (TH1I*)file->Get(hist_name);
        if (!hist) { 
            std::cout << "警告: 未在 " << filename << " 中找到 " << hist_name << std::endl; 
            file->Close();
            continue; 
        }

        // 克隆直方图并脱离文件管理
        TH1I *h_clone = (TH1I*)hist->Clone(Form("h_clone_r%d", run));
        h_clone->SetDirectory(0); 
        
        // 固定横坐标范围
        h_clone->GetXaxis()->SetRangeUser(x_min, x_max);
        
        Int_t color_idx = (run - start_round) % 6;
        h_clone->SetLineColor(colors[color_idx]);
        h_clone->SetLineWidth(2);
        h_clone->SetTitle(Form("Round %d", run));

        // 绘制逻辑：统一使用手动指定的 Y 轴上限
        if (cloned_hists.empty()) {
            h_clone->GetYaxis()->SetRangeUser(0, manual_ymax);
            h_clone->Draw("HIST");
        } else {
            h_clone->Draw("HIST SAME");
        }

        cloned_hists.push_back(h_clone);
        file->Close(); 
        std::cout << "成功处理: " << filename << std::endl;
    }

    // 绘制图例
    if (!cloned_hists.empty()) {
        TLegend *leg = new TLegend(0.65, 0.80, 0.88, 0.92);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0); 
        for (auto* h : cloned_hists) {
            leg->AddEntry(h, h->GetTitle(), "l");
        }
        leg->Draw();
    }

    c1->Update();
    std::cout << "=== 绘制完成 (Y轴上限: " << manual_ymax << ") ===" << std::endl;
}
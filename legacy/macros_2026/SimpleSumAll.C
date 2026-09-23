#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TSystemDirectory.h"  // 添加这一行
#include "TSystemFile.h"       // 添加这一行
#include "TF1.h"
#include "TMath.h"
#include "TString.h"
#include "TChain.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;

// 简单的安全加和函数
void SimpleSumAll() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "SIMPLE FILE SUMMING" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 查找所有刻度后的文件
    TSystemDirectory dir(".", ".");
    TList* files = dir.GetListOfFiles();
    TIter next(files);
    TSystemFile* file;
    
    std::vector<TString> calFiles;
    Long64_t totalEvents = 0;
    
    std::cout << "Looking for calibrated files..." << std::endl;
    
    while ((file = (TSystemFile*)next())) {
        TString name = file->GetName();
        if (name.Contains("_calibrated.root") && !name.Contains("sum")) {
            TFile* testFile = TFile::Open(name, "READ");
            if (testFile && !testFile->IsZombie()) {
                TTree* tree = (TTree*)testFile->Get("CalibratedTree");
                if (tree) {
                    calFiles.push_back(name);
                    Long64_t nEntries = tree->GetEntries();
                    totalEvents += nEntries;
                    std::cout << "  Found: " << name << " (" << nEntries << " events)" << std::endl;
                }
                testFile->Close();
            }
        }
    }
    
    if (calFiles.empty()) {
        std::cout << "No calibrated files found!" << std::endl;
        return;
    }
    
    std::cout << "\nTotal: " << calFiles.size() << " files, " << totalEvents << " events" << std::endl;
    
    // 使用TChain
    std::cout << "\nCreating TChain..." << std::endl;
    TChain* chain = new TChain("CalibratedTree");
    
    for (size_t i = 0; i < calFiles.size(); i++) {
        chain->Add(calFiles[i]);
    }
    
    std::cout << "Chain created with " << chain->GetEntries() << " events" << std::endl;
    
    // 创建简化的加和文件
    std::cout << "\nCreating sum file..." << std::endl;
    TFile* sumFile = new TFile("total_sum.root", "RECREATE");
    
    // 只复制ADC0 CH0的数据作为示例
    TTree* sumTree = new TTree("TotalSum", "Total Sum of Calibrated Data");
    
    Float_t E_adc0_ch0, E_adc0_ch1, E_adc0_ch2;
    sumTree->Branch("E_adc0_ch0", &E_adc0_ch0, "E_adc0_ch0/F");
    sumTree->Branch("E_adc0_ch1", &E_adc0_ch1, "E_adc0_ch1/F");
    sumTree->Branch("E_adc0_ch2", &E_adc0_ch2, "E_adc0_ch2/F");
    
    // 设置chain的分支地址
    chain->SetBranchAddress("E_adc0_ch0", &E_adc0_ch0);
    chain->SetBranchAddress("E_adc0_ch1", &E_adc0_ch1);
    chain->SetBranchAddress("E_adc0_ch2", &E_adc0_ch2);
    
    // 处理事件
    Long64_t nEntries = chain->GetEntries();
    std::cout << "Processing " << nEntries << " events..." << std::endl;
    
    for (Long64_t i = 0; i < nEntries; i++) {
        chain->GetEntry(i);
        sumTree->Fill();
        
        if (nEntries > 100000 && i % 100000 == 0) {
            std::cout << "  " << (100*i/nEntries) << "% (" << i << "/" << nEntries << ")" << std::endl;
        }
    }
    
    sumTree->Write();
    sumFile->Close();
    
    std::cout << "\nSum file created: total_sum.root" << std::endl;
    std::cout << "Total events saved: " << sumTree->GetEntries() << std::endl;
    
    delete chain;
}

// 显示加和结果
void ShowTotalSum() {
    TFile* file = TFile::Open("total_sum.root", "READ");
    if (!file || file->IsZombie()) {
        std::cout << "Cannot open total_sum.root!" << std::endl;
        return;
    }
    
    TTree* tree = (TTree*)file->Get("TotalSum");
    if (!tree) {
        std::cout << "No TotalSum tree found!" << std::endl;
        file->Close();
        return;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "TOTAL SUMMED SPECTRUM" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "Total events: " << tree->GetEntries() << std::endl;
    
    // 创建显示
    TCanvas* canvas = new TCanvas("total", "Total Summed Spectrum", 1000, 800);
    canvas->Divide(2, 2);
    
    // 1. ADC0 CH0
    canvas->cd(1);
    TH1F* h1 = new TH1F("h1", "ADC0 CH0 - All Files", 200, 0, 2000);
    tree->Draw("E_adc0_ch0 >> h1");
    h1->SetLineColor(kBlue);
    h1->SetLineWidth(2);
    h1->GetXaxis()->SetTitle("Energy (keV)");
    h1->GetYaxis()->SetTitle("Counts");
    h1->Draw();
    
    // 标记1332 keV
    TLine* line1 = new TLine(1332, 0, 1332, h1->GetMaximum()*0.9);
    line1->SetLineColor(kRed);
    line1->SetLineStyle(2);
    line1->SetLineWidth(2);
    line1->Draw();
    
    TLatex* tex1 = new TLatex(1400, h1->GetMaximum()*0.8, "1332 keV");
    tex1->SetTextColor(kRed);
    tex1->Draw();
    
    // 2. 1332 keV峰区域
    canvas->cd(2);
    TH1F* h2 = new TH1F("h2", "1332 keV Peak Region", 100, 1200, 1500);
    tree->Draw("E_adc0_ch0 >> h2");
    h2->SetLineColor(kRed);
    h2->SetLineWidth(2);
    h2->GetXaxis()->SetTitle("Energy (keV)");
    h2->GetYaxis()->SetTitle("Counts");
    h2->Draw();
    
    // 拟合1332 keV峰
    TF1* fit = new TF1("fit", "gaus", 1280, 1380);
    fit->SetParameters(h2->GetMaximum(), 1332, 20);
    h2->Fit(fit, "RQ");
    
    std::cout << "\n1332 keV Peak Analysis:" << std::endl;
    std::cout << "  Mean position: " << fit->GetParameter(1) << " +/- " << fit->GetParError(1) << " keV" << std::endl;
    std::cout << "  Sigma: " << fit->GetParameter(2) << " +/- " << fit->GetParError(2) << " keV" << std::endl;
    std::cout << "  FWHM: " << 2.355 * fit->GetParameter(2) << " keV" << std::endl;
    
    // 3. ADC0 CH1
    canvas->cd(3);
    TH1F* h3 = new TH1F("h3", "ADC0 CH1 - All Files", 200, 0, 2000);
    tree->Draw("E_adc0_ch1 >> h3");
    h3->SetLineColor(kGreen);
    h3->SetLineWidth(2);
    h3->GetXaxis()->SetTitle("Energy (keV)");
    h3->GetYaxis()->SetTitle("Counts");
    h3->Draw();
    
    // 4. ADC0 CH2
    canvas->cd(4);
    TH1F* h4 = new TH1F("h4", "ADC0 CH2 - All Files", 200, 0, 2000);
    tree->Draw("E_adc0_ch2 >> h4");
    h4->SetLineColor(kMagenta);
    h4->SetLineWidth(2);
    h4->GetXaxis()->SetTitle("Energy (keV)");
    h4->GetYaxis()->SetTitle("Counts");
    h4->Draw();
    
    canvas->Update();
    canvas->SaveAs("total_sum_spectrum.png");
    
    std::cout << "\nPlot saved: total_sum_spectrum.png" << std::endl;
    
    // 计算统计信息
    double peakCounts = h2->Integral();
    double peakError = TMath::Sqrt(peakCounts);
    
    std::cout << "\nStatistics:" << std::endl;
    std::cout << "  Counts in 1332 keV region (1200-1500 keV): " << peakCounts << std::endl;
    std::cout << "  Statistical error: " << peakError << std::endl;
    std::cout << "  Relative error: " << (100*peakError/peakCounts) << "%" << std::endl;
    
    file->Close();
}

// 检查文件
void CheckCalibratedFiles() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "CHECKING CALIBRATED FILES" << std::endl;
    std::cout << "========================================" << std::endl;
    
    TSystemDirectory dir(".", ".");
    TList* files = dir.GetListOfFiles();
    TIter next(files);
    TSystemFile* file;
    
    int count = 0;
    Long64_t totalEvents = 0;
    
    std::cout << "Calibrated files:" << std::endl;
    
    while ((file = (TSystemFile*)next())) {
        TString name = file->GetName();
        if (name.Contains("_calibrated.root") && !name.Contains("sum")) {
            TFile* f = TFile::Open(name, "READ");
            if (f && !f->IsZombie()) {
                TTree* tree = (TTree*)f->Get("CalibratedTree");
                if (tree) {
                    count++;
                    Long64_t nEntries = tree->GetEntries();
                    totalEvents += nEntries;
                    
                    // 提取运行号
                    TString runStr = name;
                    runStr.ReplaceAll("2025_146Sm", "");
                    runStr.ReplaceAll("_calibrated.root", "");
                    
                    std::cout << "  Run " << runStr << ": " << nEntries << " events" << std::endl;
                }
                f->Close();
            }
        }
    }
    
    std::cout << "\nSummary:" << std::endl;
    std::cout << "  Files: " << count << std::endl;
    std::cout << "  Total events: " << totalEvents << std::endl;
    std::cout << "  Average: " << (count > 0 ? totalEvents/count : 0) << " events/file" << std::endl;
}

// 查看单个文件
void ViewSingleFile(int run) {
    TString filename = TString::Format("2025_146Sm%d_calibrated.root", run);
    
    if (gSystem->AccessPathName(filename)) {
        std::cout << "File not found: " << filename << std::endl;
        return;
    }
    
    TFile* file = TFile::Open(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cout << "Cannot open file!" << std::endl;
        return;
    }
    
    TTree* tree = (TTree*)file->Get("CalibratedTree");
    if (!tree) {
        std::cout << "No calibrated tree found!" << std::endl;
        file->Close();
        return;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "RUN " << run << " CALIBRATED SPECTRUM" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "Events: " << tree->GetEntries() << std::endl;
    
    TCanvas* canvas = new TCanvas(Form("run%d", run), Form("Run %d", run), 800, 400);
    canvas->Divide(2, 1);
    
    // ADC0 CH0
    canvas->cd(1);
    TH1F* h1 = new TH1F("h1", Form("Run %d - ADC0 CH0", run), 200, 0, 2000);
    tree->Draw("E_adc0_ch0 >> h1");
    h1->SetLineColor(kBlue);
    h1->GetXaxis()->SetTitle("Energy (keV)");
    h1->Draw();
    
    // 1332 keV峰区域
    canvas->cd(2);
    TH1F* h2 = new TH1F("h2", Form("Run %d - 1332 keV Peak", run), 100, 1200, 1500);
    tree->Draw("E_adc0_ch0 >> h2");
    h2->SetLineColor(kRed);
    h2->GetXaxis()->SetTitle("Energy (keV)");
    h2->Draw();
    
    canvas->Update();
    file->Close();
}

// 主函数：一键处理
void ProcessAll() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "ONE-STEP PROCESSING" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 1. 检查文件
    CheckCalibratedFiles();
    
    // 2. 加和所有文件
    SimpleSumAll();
    
    // 3. 显示结果
    ShowTotalSum();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "PROCESSING COMPLETED!" << std::endl;
    std::cout << "========================================" << std::endl;
}

// 帮助信息
void HelpSimple() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "SIMPLE MULTI-FILE PROCESSING" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nFunctions:" << std::endl;
    std::cout << "  1. ProcessAll() - One-step processing" << std::endl;
    std::cout << "  2. SimpleSumAll() - Sum all calibrated files" << std::endl;
    std::cout << "  3. ShowTotalSum() - View summed spectrum" << std::endl;
    std::cout << "  4. CheckCalibratedFiles() - Check files" << std::endl;
    std::cout << "  5. ViewSingleFile(run) - View single file" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  ProcessAll()" << std::endl;
    std::cout << "  ViewSingleFile(146)" << std::endl;
    std::cout << "========================================" << std::endl;
}
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include <iostream>

void Offline_146Sm2026_03(int run) {
    TString fin = TString::Format("2025_146Sm%d.root", run);
    TFile* fRun = new TFile(fin, "READ");
    if (!fRun || fRun->IsZombie()) {
        std::cout << "Error: Cannot open file " << fin << std::endl;
        return;
    }
    TTree* t = (TTree*)fRun->Get("Tree1");
    if (!t) {
        std::cout << "Error: Tree1 not found in file." << std::endl;
        return;
    }

    // --- 能量变量 ---
    Float_t E1f, E1b, E2f, E2b, E3f, E3b, E4f, E4b, xe;
    
    // --- 【补全】定义 8 个重数计数器 (分别对应 8 个探测器组) ---
    Int_t n1f, n1b, n2f, n2b, n3f, n3b, n4f, n4b;

    // 全局能量范围用于直方图 (单位: MeV)
    Float_t Emin = 0.5, Emax = 8.0;
    Float_t thmax = 8.0; 
    Long64_t ntot = t->GetEntriesFast();

    UInt_t bArray[32]; // ADC 0
    UInt_t cArray[32]; // ADC 1
    UInt_t dArray[32]; // ADC 2
    UInt_t eArray[32]; // ADC 3

    // ---------------------------------------------------------
    // 1. 校准系数 (保持不变)
    // ---------------------------------------------------------
    Float_t Calk[4][32] = {
        {0.001852, 0.001942, 0.001896, 0.001951, 0.001909, 0.001878, 0.001766, 0.001860, 0.001786, 0.001865, 0.001826, 0.001869, 0.001848, 0.001909, 0.001848, 0.001914, 0.001932, 0.001826, 0.001786, 0.001900, 0.001896, 0.002105, 0.001831, 0.001951, 0.001882, 0.001856, 0.001810, 0.001852, 0.001860, 0.001905, 0.001848, 0.001923},
        {0.001810, 0.001782, 0.001794, 0.001810, 0.001782, 0.001814, 0.001852, 0.001818, 0.001831, 0.001839, 0.001848, 0.001843, 0.001843, 0.001774, 0.001810, 0.001865, 0.001743, 0.001684, 0.001810, 0.001762, 0.001810, 0.001882, 0.001869, 0.001782, 0.001918, 0.001874, 0.001843, 0.001956, 0.001887, 0.001798, 0.001874, 0.001818},
        {0.002402, 0.002094, -0.000319, 0.002597, 0.002299, 0.001882, 0.002210, 0.002222, 0.001735, 0.001923, 0.002417, -0.000298, 0.003687, 0.002477, 0.002286, 0.002073, 0.002094, 0.001831, 0.001928, 0.001699, 0.002133, 0.001856, 0.002367, 0.001782, 0.002105, 0.001774, 0.002010, 0.001891, 0.002597, 0.001629, 0.002083, 0.001656},
        {0.001758, 0.001735, 0.001766, 0.001717, 0.001720, 0.001751, 0.001747, 0.001706, 0.001754, 0.001774, 0.001782, 0.001778, 0.001732, 0.001699, 0.001743, 0.001677, 0.001747, 0.001810, 0.001874, 0.001896, 0.001790, 0.001937, 0.001782, 0.001806, 0.001790, 0.001826, 0.001806, 0.001878, 0.001778, 0.001663, 0.001728, 0.001684}
    };

    Float_t Calb[4][32] = {
        {-0.59630, -0.75534, -0.66351, -0.71805, -0.74368, -0.65540, -0.54834, -0.63814, -0.58393, -0.71142, -0.67945, -0.65981, -0.64758, -0.65776, -0.65497, -0.68134, -0.72464, -0.53516, -0.52857, -0.64893, -0.72796, -0.94947, -0.68375, -0.74927, -0.77082, -0.63759, -0.66425, -0.67963, -0.62326, -0.71619, -0.65681, -0.70385},
        {-0.31493, -0.27884, -0.24933, -0.30769, -0.32873, -0.30930, -0.45556, -0.38727, -0.38719, -0.41471, -0.46282, -0.34654, -0.39263, -0.30599, -0.34389, -0.42797, -0.20654, -0.14737, -0.31674, -0.31013, -0.38009, -0.40188, -0.47290, -0.38931, -0.49017, -0.43747, -0.50138, -0.50954, -0.48113, -0.31371, -0.45059, -0.36909},
        {-1.35135, -0.77906, 3.34505, -1.46234, -1.13563, -0.51671, -0.95912, -1.11333, -0.37570, -0.52500, -1.20483, 3.32935, -3.03226, -1.41300, -1.13600, -0.78549, -0.70785, -0.41098, -0.67181, -0.30998, -1.03787, -0.57262, -1.39172, -0.47305, -0.99789, -0.45854, -0.78593, -0.58156, -1.61818, -0.12953, -0.76458, -0.19959},
        {-0.43516, -0.42603, -0.35585, -0.45150, -0.42925, -0.39475, -0.38952, -0.37612, -0.50702, -0.46918, -0.59777, -0.58133, -0.42944, -0.46115, -0.43660, -0.36478, -0.40000, -0.47964, -0.54052, -0.57630, -0.50201, -0.54140, -0.53185, -0.52460, -0.50201, -0.49498, -0.44153, -0.41878, -0.35022, -0.31185, -0.35335, -0.27874}
    };

    // ---------------------------------------------------------
    // 2. 定义并初始化独立阈值数组 [ADC索引][通道索引]
    // ---------------------------------------------------------
    UInt_t thArray[4][32];
    for(int i=0; i<4; i++) {
        for(int j=0; j<32; j++) {
            thArray[i][j] = 180; // 默认阈值
        }
    }
    // --- 手动设置特定通道的阈值 (单位：道数) ---
    thArray[0][2] = 160; thArray[0][3] = 165; thArray[0][7] = 170; thArray[0][9] = 200;
    thArray[0][10] = 200; thArray[0][11] = 190; thArray[0][12] = 200; thArray[0][14] = 205;
    thArray[1][0] = 190; thArray[1][2] = 190; thArray[1][3] = 200; thArray[1][4] = 215;
    thArray[1][5] = 190; thArray[1][6] = 190; thArray[1][8] = 210; thArray[1][10] = 210;
    thArray[1][11] = 200; thArray[1][12] = 205; thArray[1][13] = 210; thArray[1][14] = 210; thArray[1][15] = 200;
    thArray[2][0] = 150; thArray[2][1] = 140; thArray[2][2] = 130; thArray[2][3] = 120;
    thArray[2][4] = 150; thArray[2][5] = 150; thArray[2][8] = 160; thArray[2][9] = 150;
    thArray[2][10] = 170; thArray[2][11] = 140; thArray[2][12] = 170; thArray[2][13] = 150; thArray[2][14] = 160; thArray[2][15] = 180;
    thArray[3][0] = 160; thArray[3][1] = 150; thArray[3][2] = 170; thArray[3][3] = 170;
    thArray[3][4] = 170; thArray[3][5] = 150; thArray[3][6] = 160; thArray[3][7] = 180;
    thArray[3][8] = 170; thArray[3][9] = 160; thArray[3][10] = 180; thArray[3][11] = 160; thArray[3][12] = 180; thArray[3][13] = 160; thArray[3][14] = 200; thArray[3][15] = 180;

    // ---------------------------------------------------------
    // 3. 设置分支地址
    // ---------------------------------------------------------
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc0ch%d", i), &bArray[i]);
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc1ch%d", i), &cArray[i]);
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc3ch%d", i), &eArray[i]);

    // ---------------------------------------------------------
    // 4. 创建直方图
    // ---------------------------------------------------------
    TH1F *myHisto_1 = new TH1F("myHisto_1", "E1f (Sum)", 400, Emin, Emax);
    TH1F *myHisto_2 = new TH1F("myHisto_2", "E1b (Sum)", 400, Emin, Emax);
    TH1F *myHisto_3 = new TH1F("myHisto_3", "E2f (Sum)", 400, Emin, Emax);
    TH1F *myHisto_4 = new TH1F("myHisto_4", "E2b (Sum)", 400, Emin, Emax);
    TH1F *myHisto_5 = new TH1F("myHisto_5", "E3f (Sum)", 400, Emin, Emax);
    TH1F *myHisto_6 = new TH1F("myHisto_6", "E3b (Sum)", 400, Emin, Emax);
    TH1F *myHisto_7 = new TH1F("myHisto_7", "E4f (Sum)", 400, Emin, Emax);
    TH1F *myHisto_8 = new TH1F("myHisto_8", "E4b (Sum)", 400, Emin, Emax);
    TH2F *myHisto_10 = new TH2F("myHisto_10", "E1f vs E1b", 400, Emin, Emax, 400, Emin, Emax);
    TH2F *myHisto_11 = new TH2F("myHisto_11", "E2f vs E2b", 400, Emin, Emax, 400, Emin, Emax);
    TH2F *myHisto_12 = new TH2F("myHisto_12", "E3f vs E3b", 400, Emin, Emax, 400, Emin, Emax);
    TH2F *myHisto_13 = new TH2F("myHisto_13", "E4f vs E4b", 400, Emin, Emax, 400, Emin, Emax);

    // ---------------------------------------------------------
    // 5. 主循环
    // ---------------------------------------------------------
    for(Long64_t i=0; i<ntot; i++) {
        t->GetEntry(i);
        
        // --- 重置能量和重数计数器 ---
        E1f=E1b=E2f=E2b=E3f=E3b=E4f=E4b=0.;
        // 【关键】所有计数器归零
        n1f=n1b=n2f=n2b=n3f=n3b=n4f=n4b=0; 

        // --- 处理 ADC 0 (bArray) ---
        for(Int_t j=0; j<16; j++) {
            if(bArray[j] > thArray[0][j]) {
                xe = bArray[j] * Calk[0][j] + Calb[0][j];
                E1f += xe;
                n1f++; // 计数
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(bArray[j] > thArray[0][j]) {
                xe = bArray[j] * Calk[0][j] + Calb[0][j];
                E1b += xe;
                n1b++;
            }
        }

        // --- 处理 ADC 1 (cArray) ---
        for(Int_t j=0; j<16; j++) {
            if(cArray[j] > thArray[1][j]) {
                xe = cArray[j] * Calk[1][j] + Calb[1][j];
                E2f += xe;
                n2f++;
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(cArray[j] > thArray[1][j]) {
                xe = cArray[j] * Calk[1][j] + Calb[1][j];
                E2b += xe;
                n2b++;
            }
        }   // --- 处理 ADC 2 (dArray) ---
        for(Int_t j=0; j<16; j++) {
            if(dArray[j] > thArray[2][j]) {
                xe = dArray[j] * Calk[2][j] + Calb[2][j];
                E3f += xe;
                n3f++;
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(dArray[j] > thArray[2][j]) {
                xe = dArray[j] * Calk[2][j] + Calb[2][j];
                E3b += xe;
                n3b++;
            }
        }

        // --- 处理 ADC 3 (eArray) ---
        for(Int_t j=0; j<16; j++) {
            if(eArray[j] > thArray[3][j]) {
                xe = eArray[j] * Calk[3][j] + Calb[3][j];
                E4f += xe;
                n4f++;
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(eArray[j] > thArray[3][j]) {
                xe = eArray[j] * Calk[3][j] + Calb[3][j];
                E4b += xe;
                n4b++;
            }
        }

        // =================================================================
        // 【核心逻辑】多重性筛选
        // 如果一个探测器组有 >= 2 个通道触发，则将该组能量置零
        // =================================================================
        if(n1f >= 2) E1f = 0;
        if(n1b >= 2) E1b = 0;
        if(n2f >= 2) E2f = 0;
        if(n2b >= 2) E2b = 0;
        if(n3f >= 2) E3f = 0;
        if(n3b >= 2) E3b = 0;
        if(n4f >= 2) E4f = 0;
        if(n4b >= 2) E4b = 0;

        // --- 填充直方图 ---
        // 注意：因为上面已经置零，这里只需要判断能量 > 0 即可
        if(E1f > 0 && E1f < thmax) myHisto_1->Fill(E1f);
        if(E1b > 0 && E1b < thmax) myHisto_2->Fill(E1b);
        if(E2f > 0 && E2f < thmax) myHisto_3->Fill(E2f);
        if(E2b > 0 && E2b < thmax) myHisto_4->Fill(E2b);
        if(E3f > 0 && E3f < thmax) myHisto_5->Fill(E3f);
        if(E3b > 0 && E3b < thmax) myHisto_6->Fill(E3b);
        if(E4f > 0 && E4f < thmax) myHisto_7->Fill(E4f);
        if(E4b > 0 && E4b < thmax) myHisto_8->Fill(E4b);

        // --- 填充双维直方图 (符合谱) ---
        if(E1f > 0 && E1b > 0 && E1f < thmax && E1b < thmax) myHisto_10->Fill(E1f, E1b);
        if(E2f > 0 && E2b > 0 && E2f < thmax && E2b < thmax) myHisto_11->Fill(E2f, E2b);
        if(E3f > 0 && E3b > 0 && E3f < thmax && E3b < thmax) myHisto_12->Fill(E3f, E3b);
        if(E4f > 0 && E4b > 0 && E4f < thmax && E4b < thmax) myHisto_13->Fill(E4f, E4b);

    } // 结束主循环

    // ---------------------------------------------------------
    // 6. 绘图与保存
    // ---------------------------------------------------------
    TCanvas *c1 = new TCanvas("c1", "Offline Analysis", 1200, 800);
    c1->Divide(4, 3); // 4列3行

    c1->cd(1); myHisto_1->Draw();
    c1->cd(2); myHisto_2->Draw();
    c1->cd(3); myHisto_3->Draw();
    c1->cd(4); myHisto_4->Draw();
    c1->cd(5); myHisto_5->Draw();
    c1->cd(6); myHisto_6->Draw();
    c1->cd(7); myHisto_7->Draw();
    c1->cd(8); myHisto_8->Draw();
    c1->cd(9); myHisto_10->Draw("COLZ");
    c1->cd(10); myHisto_11->Draw("COLZ");
    c1->cd(11); myHisto_12->Draw("COLZ");
    c1->cd(12); myHisto_13->Draw("COLZ");

    // 可选：保存画布
    // c1->SaveAs(Form("Result_Run%d.pdf", run));
}
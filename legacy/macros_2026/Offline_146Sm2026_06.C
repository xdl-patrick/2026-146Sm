#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include <iostream>

void Offline_146Sm2026_06(int run) {
    TString fin = TString::Format("2026_146Sm%d.root", run);
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
        0.001728,
0.001776,
0.001732,
0.001763,
0.001705,
0.001762,
0.001769,
0.001705,
0.001659,
0.001738,
0.001716,
0.001783,
0.001718,
0.001752,
0.001742,
0.001744,
0.001789,
0.001764,
0.001771,
0.001839,
0.001839,
0.001787,
0.001739,
0.001786,
0.001727,
0.001743,
0.001754,
0.001798,
0.001816,
0.001790,
0.001793,
0.001843,
0.001805,
0.001783,
0.001821,
0.001801,
0.001805,
0.001822,
0.001827,
0.001804,
0.001800,
0.001757,
0.001775,
0.001807,
0.001828,
0,
0.001803,
0.001853,
0.001791,
0.001798,
0.001792,
0.001789,
0.001747,
0.001838,
0.001751,
0,
0.001823,
0.001866,
0.001775,
0.001865,
0.001835,
0.001819,
0.001802,
0.001824,
0.001829,
0.001791,
0.001804,
0.001812,
0.001778,
0.001821,
0.001877,
0.001848,
0.001763,
0.001779,
0.001811,
0.001829,
0.001769,
0.001793,
0.001790,
0.001796,
0.001861,
0.001922,
0.001806,
0.001880,
0.001909,
0.001800,
0.001756,
0.001794,
0.001781,
0.001848,
0.001840,
0.002121,
0.001841,
0.001858,
0.002262,
0.001728,
0.001765,
0.001737,
0.001792,
0.001710,
0.001713,
0.001739,
0.001787,
0.001705,
0.001758,
0.001784,
0.001714,
0.001717,
0.001744,
0.001691,
0.001744,
0,
0.001774,
0.001795,
0.001848,
0.001887,
0.001761,
0.001892,
0,
0.001791,
0.001774,
0.002054,
0.001834,
0.001935,
0.001868,
0.001718,
0.001787,
0.001767,
    };

    Float_t Calb[4][32] = {
       -0.46817,
-0.59677,
-0.52734,
-0.56275,
-0.60799,
-0.57564,
-0.54703,
-0.48981,
-0.32329,
-0.60198,
-0.58600,
-0.58728,
-0.58043,
-0.47347,
-0.61349,
-0.52925,
-0.46584,
-0.50980,
-0.56778,
-0.53737,
-0.30636,
-0.45703,
-0.52120,
-0.56064,
-0.56032,
-0.45870,
-0.68422,
-0.63837,
-0.50578,
-0.60405,
-0.65345,
-0.65462,
-0.41171,
-0.40829,
-0.37054,
-0.38958,
-0.38340,
-0.39022,
-0.45693,
-0.43208,
-0.41499,
-0.46787,
-0.44968,
-0.39677,
-0.47955,
0.00000,
-0.44974,
-0.59378,
-0.38993,
-0.41916,
-0.38702,
-0.48734,
-0.40842,
-0.43365,
-0.39039,
0.00000,
-0.38041,
-0.46427,
-0.48512,
-0.48082,
-0.48056,
-0.46083,
-0.40216,
-0.53217,
-0.45127,
-0.37073,
-0.47633,
-0.44649,
-0.43261,
-0.45167,
-0.54015,
-0.64220,
-0.48082,
-0.42821,
-0.49567,
-0.43593,
-0.48845,
-0.53866,
-0.51043,
-0.49217,
-0.58638,
-0.69871,
-0.61878,
-0.57793,
-0.74871,
-0.49735,
-0.44871,
-0.49574,
-0.54674,
-0.58882,
-0.54044,
-0.86061,
-0.54727,
-0.48703,
-1.03062,
-0.46613,
-0.50565,
-0.49106,
-0.48023,
-0.49687,
-0.50148,
-0.43421,
-0.57289,
-0.47792,
-0.59444,
-0.55138,
-0.58403,
-0.57692,
-0.54595,
-0.55890,
-0.55740,
0.00000,
-0.59028,
-0.58521,
-0.62450,
-0.69779,
-0.53554,
-0.53589,
0.00000,
-0.57869,
-0.54798,
-0.86689,
-0.57453,
-0.57200,
-0.57752,
-0.51681,
-0.60875,
-0.56361,
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
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include <iostream>

void Offline_146Sm2026_04(int run) {
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
       0.001740,
0.001760,
0.001721,
0.001747,
0.001679,
0.001751,
0.001764,
0.001707,
0.001659,
0.001703,
0.001680,
0.001734,
0.001659,
0.001702,
0.001692,
0.001685,
0.001768,
0.001732,
0.001735,
0.001824,
0.001993,
0.001819,
0.001768,
0.001772,
0.001727,
0.001750,
0.001713,
0.001787,
0.001816,
0.001765,
0.001748,
0.001807,
0.001734,
0.001688,
0.001726,
0.001684,
0.001675,
0.001674,
0.001648,
0.001622,
0.001750,
0.001701,
0.001737,
0.001780,
0.001797,
0,
0.001787,
0.001804,
0.001734,
0.001721,
0.001739,
0.001724,
0.001714,
0.001792,
0.001730,
0,
0.001820,
0.001836,
0.001745,
0.001826,
0.001784,
0.001757,
0.001765,
0.001724,
0.001773,
0.001755,
0.001731,
0.001757,
0.001853,
0.001730,
0.001768,
0.001727,
0.001731,
0.001744,
0.001782,
0.001824,
0.001752,
0.001775,
0.001776,
0.001781,
0.001871,
0.001878,
0.001751,
0.001857,
0.001863,
0.001789,
0.001772,
0.001782,
0.001776,
0.001813,
0.001828,
0.002091,
0.001808,
0.001833,
0.002214,
0.001693,
0.001685,
0.001657,
0.001710,
0.001662,
0.001661,
0.001710,
0.001734,
0.001680,
0.001734,
0.001771,
0.001697,
0.001705,
0.001730,
0.001671,
0.001720,
0.001658,
0.001728,
0.001759,
0.001805,
0.001824,
0.001747,
0.001884,
0.001945,
0.001771,
0.001768,
0.002034,
0.001817,
0.001909,
0.001826,
0.001689,
0.001737,
0.001710,

    };

    Float_t Calb[4][32] = {
     -0.50890,
-0.54183,
-0.48875,
-0.50749,
-0.51723,
-0.53747,
-0.53228,
-0.49493,
-0.32053,
-0.48029,
-0.46118,
-0.41832,
-0.36987,
-0.30408,
-0.43591,
-0.32569,
-0.39668,
-0.39924,
-0.44618,
-0.48978,
-0.78831,
-0.56262,
-0.62340,
-0.51432,
-0.55930,
-0.48156,
-0.53878,
-0.59838,
-0.50738,
-0.51866,
-0.49980,
-0.53247,
-0.18038,
-0.09261,
-0.06643,
-0.00799,
0.04109,
0.08761,
0.12748,
0.16480,
-0.25332,
-0.27783,
-0.32277,
-0.31061,
-0.37897,
0.00000,
-0.39959,
-0.43536,
-0.20154,
-0.16639,
-0.21253,
-0.26914,
-0.29528,
-0.28367,
-0.31974,
0.00000,
-0.36966,
-0.36811,
-0.38447,
-0.35380,
-0.31478,
-0.25963,
-0.27924,
-0.20009,
-0.27004,
-0.25227,
-0.23558,
-0.26722,
-0.68100,
-0.15665,
-0.19222,
-0.24196,
-0.37073,
-0.30952,
-0.40052,
-0.42106,
-0.43376,
-0.47924,
-0.46643,
-0.44120,
-0.61929,
-0.55756,
-0.43141,
-0.50255,
-0.60052,
-0.45943,
-0.50074,
-0.45410,
-0.52953,
-0.47264,
-0.50131,
-0.77004,
-0.44094,
-0.40857,
-0.89220,
-0.34360,
-0.23196,
-0.21839,
-0.20562,
-0.32787,
-0.31899,
-0.33523,
-0.39382,
-0.38956,
-0.51203,
-0.50809,
-0.52418,
-0.53612,
-0.49924,
-0.48613,
-0.47552,
-0.39584,
-0.43253,
-0.46294,
-0.48217,
-0.49134,
-0.48847,
-0.50930,
-0.70391,
-0.51208,
-0.52585,
-0.80519,
-0.52026,
-0.48984,
-0.44159,
-0.41546,
-0.43722,
-0.36838,

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
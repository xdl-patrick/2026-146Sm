#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TColor.h"   // [FIX] 显式包含 TColor 头文件
#include "TLegend.h"  // [FIX] 显式包含 TLegend 头文件
#include <iostream>

void Offline_146Sm2026_0602(int run) {
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
    Float_t E1f_raw, E1b_raw, E2f_raw, E2b_raw, E3f_raw, E3b_raw, E4f_raw, E4b_raw;
    Int_t n1f, n1b, n2f, n2b, n3f, n3b, n4f, n4b;

    Float_t Emin = 0.5, Emax = 8.0;
    Float_t thmax = 8.0; 
    Long64_t ntot = t->GetEntriesFast();

    UInt_t bArray[32], cArray[32], dArray[32], eArray[32];

    // ---------------------------------------------------------
    // 1. 校准系数 (保持不变)
    // ---------------------------------------------------------
    Float_t Calk[4][32] = {
       0.001613,
0.001633,
0.001598,
0.001621,
0.001560,
0.001624,
0.001636,
0.001584,
0.001541,
0.001584,
0.001557,
0.001613,
0.001542,
0.001581,
0.001542,
0.001551,
0.001662,
0.001639,
0.001645,
0.001708,
0.001709,
0.001660,
0.001615,
0.001659,
0.001604,
0.001619,
0.001630,
0.001670,
0.001687,
0.001663,
0.001665,
0.001712,
0.001676,
0.001657,
0.001691,
0.001672,
0.001677,
0.001692,
0.001697,
0.001675,
0.001671,
0.001632,
0.001649,
0.001678,
0.001698,
0,
0.001674,
0.001721,
0.001664,
0.001671,
0.001665,
0.001662,
0.001623,
0.001708,
0.001626,
0,
0.001693,
0.001734,
0.001648,
0.001733,
0.001704,
0.001689,
0.001674,
0.001694,
0.001699,
0.001664,
0.001675,
0.001683,
0.001652,
0.001691,
0.001743,
0.001716,
0.001638,
0.001653,
0.001682,
0.001699,
0.001643,
0.001665,
0.001662,
0.001669,
0.001728,
0.001785,
0.001678,
0.001746,
0.001773,
0.001672,
0.001631,
0.001666,
0.001654,
0.001717,
0.001709,
0.001970,
0.001710,
0.001725,
0.002101,
0.001605,
0.001640,
0.001613,
0.001665,
0.001589,
0.001591,
0.001615,
0.001660,
0.001584,
0.001633,
0.001657,
0.001592,
0.001594,
0.001620,
0.001571,
0.001620,
0,
0.001648,
0.001668,
0.001717,
0.001752,
0.001636,
0.001758,
0,
0.001663,
0.001648,
0.001908,
0.001703,
0.001797,
0.001735,
0.001596,
0.001660,
0.001642,
    };

    Float_t Calb[4][32] = {
    -0.07340,
-0.10761,
-0.06110,
-0.07467,
-0.09179,
-0.10123,
-0.09534,
-0.06506,
0.09086,
-0.06489,
-0.02301,
-0.00861,
0.04370,
0.10815,
0.08745,
0.13700,
0.01739,
0.01760,
-0.01799,
-0.05902,
-0.33439,
-0.13014,
-0.19004,
-0.08365,
-0.12731,
-0.05374,
-0.10305,
-0.16808,
-0.06795,
-0.08586,
-0.08446,
-0.07829,
0.10746,
0.09757,
0.07420,
0.07957,
0.01603,
0.03441,
0.05303,
0.02365,
0.10681,
0.15800,
0.12889,
0.15661,
0.11042,
0.00000,
0.07033,
0.00923,
0.18309,
0.17642,
0.11828,
0.10950,
0.07989,
0.08269,
0.04474,
0.00000,
-0.00048,
0.01910,
0.00425,
0.05044,
0.03803,
0.08875,
0.07982,
0.12870,
-0.01249,
0.00390,
0.00181,
-0.00611,
-0.00735,
-0.02370,
-0.03312,
-0.10238,
0.02314,
0.10157,
0.04050,
0.04485,
0.05032,
-0.01106,
-0.00010,
-0.00922,
-0.08744,
-0.11910,
-0.03277,
-0.07642,
-0.19936,
-0.02386,
-0.07430,
-0.02951,
-0.10166,
-0.05274,
-0.07271,
-0.33958,
-0.01661,
0.02259,
-0.42441,
0.06525,
0.23014,
0.19239,
0.19804,
0.10188,
0.10601,
0.08322,
0.04521,
0.03343,
-0.08720,
-0.07776,
-0.09112,
-0.10647,
-0.06597,
-0.05757,
-0.04009,
0.00000,
-0.00098,
-0.02158,
-0.03062,
-0.06843,
-0.06118,
-0.08309,
0.00000,
-0.07322,
-0.08192,
-0.34673,
-0.08031,
-0.05161,
-0.01825,
0.02046,
-0.02146,
0.04523,
    };

    // ---------------------------------------------------------
    // 2. 阈值数组 (保持不变)
    // ---------------------------------------------------------
    UInt_t thArray[4][32];
    for(int i=0; i<4; i++)
        for(int j=0; j<32; j++)
            thArray[i][j] = 180;

    thArray[0][2]=160; thArray[0][3]=165; thArray[0][7]=170; thArray[0][9]=200;
    thArray[0][10]=200; thArray[0][11]=190; thArray[0][12]=200; thArray[0][14]=205;
    thArray[1][0]=190; thArray[1][2]=190; thArray[1][3]=200; thArray[1][4]=215;
    thArray[1][5]=190; thArray[1][6]=190; thArray[1][8]=210; thArray[1][10]=210;
    thArray[1][11]=200; thArray[1][12]=205; thArray[1][13]=210; thArray[1][14]=210; thArray[1][15]=200;
    thArray[2][0]=150; thArray[2][1]=140; thArray[2][2]=130; thArray[2][3]=120;
    thArray[2][4]=150; thArray[2][5]=150; thArray[2][8]=160; thArray[2][9]=150;
    thArray[2][10]=170; thArray[2][11]=140; thArray[2][12]=170; thArray[2][13]=150; thArray[2][14]=160; thArray[2][15]=180;
    thArray[3][0]=160; thArray[3][1]=150; thArray[3][2]=170; thArray[3][3]=170;
    thArray[3][4]=170; thArray[3][5]=150; thArray[3][6]=160; thArray[3][7]=180;
    thArray[3][8]=170; thArray[3][9]=160; thArray[3][10]=180; thArray[3][11]=160; thArray[3][12]=180; thArray[3][13]=160; thArray[3][14]=200; thArray[3][15]=180;

    // ---------------------------------------------------------
    // 3. 设置分支地址 (保持不变)
    // ---------------------------------------------------------
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc0ch%d", i), &bArray[i]);
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc1ch%d", i), &cArray[i]);
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);
    for(int i=0; i<32; i++) t->SetBranchAddress(Form("adc3ch%d", i), &eArray[i]);

    // ---------------------------------------------------------
    // 4. 创建直方图
    // ---------------------------------------------------------
    TH1F *h_E1f_cut = new TH1F("h_E1f_cut", "E1f (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E1b_cut = new TH1F("h_E1b_cut", "E1b (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E2f_cut = new TH1F("h_E2f_cut", "E2f (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E2b_cut = new TH1F("h_E2b_cut", "E2b (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E3f_cut = new TH1F("h_E3f_cut", "E3f (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E3b_cut = new TH1F("h_E3b_cut", "E3b (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E4f_cut = new TH1F("h_E4f_cut", "E4f (With Mult. Cut)", 400, Emin, Emax);
    TH1F *h_E4b_cut = new TH1F("h_E4b_cut", "E4b (With Mult. Cut)", 400, Emin, Emax);

    TH1F *h_E1f_raw = new TH1F("h_E1f_raw", "E1f (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E1b_raw = new TH1F("h_E1b_raw", "E1b (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E2f_raw = new TH1F("h_E2f_raw", "E2f (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E2b_raw = new TH1F("h_E2b_raw", "E2b (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E3f_raw = new TH1F("h_E3f_raw", "E3f (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E3b_raw = new TH1F("h_E3b_raw", "E3b (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E4f_raw = new TH1F("h_E4f_raw", "E4f (Raw, No Cut)", 400, Emin, Emax);
    TH1F *h_E4b_raw = new TH1F("h_E4b_raw", "E4b (Raw, No Cut)", 400, Emin, Emax);

    TH1F *hist[8];
    for (int i =0; i<8;i++) hist[i] = new TH1F("h", "h", 100, 1.5, 3);
    TCanvas *c2 = new TCanvas("c2", "c2");

    // [CLEAN] 移除了旧的 SetLineColor/SetLineStyle 设置
    // 样式统一在第6节的 drawTransparentPair 中通过透明颜色索引管理

    // ---------------------------------------------------------
    // 5. 主循环 (逻辑完全保持不变)
    // ---------------------------------------------------------
    for(Long64_t i=0; i<ntot; i++) {
        t->GetEntry(i);
        
        E1f=E1b=E2f=E2b=E3f=E3b=E4f=E4b=0.;
        E1f_raw=E1b_raw=E2f_raw=E2b_raw=E3f_raw=E3b_raw=E4f_raw=E4b_raw=0.;
        n1f=n1b=n2f=n2b=n3f=n3b=n4f=n4b=0; 

        for(Int_t j=0; j<16; j++) {
            if(bArray[j] > thArray[0][j]) {
                xe = bArray[j] * Calk[0][j] + Calb[0][j];
                E1f += xe; n1f++;
                E1f_raw += xe;

                if ( j < 8) hist[j]->Fill(xe);
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(bArray[j] > thArray[0][j]) {
                xe = bArray[j] * Calk[0][j] + Calb[0][j];
                E1b += xe; n1b++;
                E1b_raw += xe;
            }
        }
        for(Int_t j=0; j<16; j++) {
            if(cArray[j] > thArray[1][j]) {
                xe = cArray[j] * Calk[1][j] + Calb[1][j];
                E2f += xe; n2f++;
                E2f_raw += xe;
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(cArray[j] > thArray[1][j]) {
                xe = cArray[j] * Calk[1][j] + Calb[1][j];
                E2b += xe; n2b++;
                E2b_raw += xe;
            }
        }
        for(Int_t j=0; j<16; j++) {
            if(dArray[j] > thArray[2][j]) {
                xe = dArray[j] * Calk[2][j] + Calb[2][j];
                E3f += xe; n3f++;
                E3f_raw += xe;
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(dArray[j] > thArray[2][j]) {
                xe = dArray[j] * Calk[2][j] + Calb[2][j];
                E3b += xe; n3b++;
                E3b_raw += xe;
            }
        }
        for(Int_t j=0; j<16; j++) {
            if(eArray[j] > thArray[3][j]) {
                xe = eArray[j] * Calk[3][j] + Calb[3][j];
                E4f += xe; n4f++;
                E4f_raw += xe;
            }
        }
        for(Int_t j=16; j<32; j++) {
            if(eArray[j] > thArray[3][j]) {
                xe = eArray[j] * Calk[3][j] + Calb[3][j];
                E4b += xe; n4b++;
                E4b_raw += xe;
            }
        }

        if(E1f_raw > 0 && E1f_raw < thmax) h_E1f_raw->Fill(E1f_raw);
        if(E1b_raw > 0 && E1b_raw < thmax) h_E1b_raw->Fill(E1b_raw);
        if(E2f_raw > 0 && E2f_raw < thmax) h_E2f_raw->Fill(E2f_raw);
        if(E2b_raw > 0 && E2b_raw < thmax) h_E2b_raw->Fill(E2b_raw);
        if(E3f_raw > 0 && E3f_raw < thmax) h_E3f_raw->Fill(E3f_raw);
        if(E3b_raw > 0 && E3b_raw < thmax) h_E3b_raw->Fill(E3b_raw);
        if(E4f_raw > 0 && E4f_raw < thmax) h_E4f_raw->Fill(E4f_raw);
        if(E4b_raw > 0 && E4b_raw < thmax) h_E4b_raw->Fill(E4b_raw);

        if(n1f >= 2) E1f = 0;
        if(n1b >= 2) E1b = 0;
        if(n2f >= 2) E2f = 0;
        if(n2b >= 2) E2b = 0;
        if(n3f >= 2) E3f = 0;
        if(n3b >= 2) E3b = 0;
        if(n4f >= 2) E4f = 0;
        if(n4b >= 2) E4b = 0;

        if(E1f > 0 && E1f < thmax) h_E1f_cut->Fill(E1f);
        if(E1b > 0 && E1b < thmax) h_E1b_cut->Fill(E1b);
        if(E2f > 0 && E2f < thmax) h_E2f_cut->Fill(E2f);
        if(E2b > 0 && E2b < thmax) h_E2b_cut->Fill(E2b);
        if(E3f > 0 && E3f < thmax) h_E3f_cut->Fill(E3f);
        if(E3b > 0 && E3b < thmax) h_E3b_cut->Fill(E3b);
        if(E4f > 0 && E4f < thmax) h_E4f_cut->Fill(E4f);
        if(E4b > 0 && E4b < thmax) h_E4b_cut->Fill(E4b);
    }

    // ---------------------------------------------------------
    // ---------------------------------------------------------
    // 6. 绘图与保存 (改为虚线区分)
    // ---------------------------------------------------------
    TCanvas *c1 = new TCanvas("c1", "Multiplicity Cut Comparison", 1600, 1000);
    c1->Divide(4, 2);

    auto drawDashedPair = [&](TPad* pad, TH1F* hraw, TH1F* hcut) {
        pad->cd();

        hraw->SetTitle(Form("%s ; Energy (MeV); Counts", hcut->GetTitle()));
        Double_t ymax = TMath::Max(hraw->GetMaximum(), hcut->GetMaximum()) * 1.15;
        hraw->SetMaximum(ymax);
        hraw->SetStats(0);
        hcut->SetStats(0);

        // Raw: 蓝色虚线，无填充
        hraw->SetLineColor(kBlue);
        hraw->SetLineStyle(2);      // 2 = 虚线 (dashed)
        hraw->SetLineWidth(2);
        hraw->SetFillStyle(0);      // 0 = 空心/无填充

        // Cut: 红色虚线，无填充
        hcut->SetLineColor(kRed);
        hcut->SetLineStyle(2);      // 2 = 虚线 (dashed)
        hcut->SetLineWidth(2);
        hcut->SetFillStyle(0);      // 0 = 空心/无填充

        hraw->Draw("HIST");
        hcut->Draw("HIST SAME");

        TLegend *leg = new TLegend(0.55, 0.72, 0.88, 0.88);
        leg->AddEntry(hraw, "Raw (No Cut)", "l");
        leg->AddEntry(hcut, "With Mult. Cut", "l");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->Draw();
    };

    c1->cd(1); drawDashedPair((TPad*)c1->GetPad(1), h_E1f_raw, h_E1f_cut);
    c1->cd(2); drawDashedPair((TPad*)c1->GetPad(2), h_E1b_raw, h_E1b_cut);
    c1->cd(3); drawDashedPair((TPad*)c1->GetPad(3), h_E2f_raw, h_E2f_cut);
    c1->cd(4); drawDashedPair((TPad*)c1->GetPad(4), h_E2b_raw, h_E2b_cut);
    c1->cd(5); drawDashedPair((TPad*)c1->GetPad(5), h_E3f_raw, h_E3f_cut);
    c1->cd(6); drawDashedPair((TPad*)c1->GetPad(6), h_E3b_raw, h_E3b_cut);
    c1->cd(7); drawDashedPair((TPad*)c1->GetPad(7), h_E4f_raw, h_E4f_cut);
    c1->cd(8); drawDashedPair((TPad*)c1->GetPad(8), h_E4b_raw, h_E4b_cut);

    c1->Update();
    // c1->SaveAs(Form("MultCut_Comparison_Run%d.pdf", run));

    Color_t colors[8] = {kBlack, kRed, kBlue, kGreen, 
                         kMagenta, kCyan, kOrange, kViolet};

    c2->cd();
    hist[0]->SetLineColor(colors[0]);
    hist[0]->Draw("HIST");
    hist[0]->GetYaxis()->SetRangeUser(0, 100);
    for(int i=1; i<8;i++){
        hist[i]->SetLineColor(colors[i]);
        hist[i]->Draw("HIST SAME");
    }
    c2->Update();
}
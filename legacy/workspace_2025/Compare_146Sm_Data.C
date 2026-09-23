#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TLegend.h"
#include "TString.h"
#include "TApplication.h"
#include <iostream>

// 先声明辅助函数
void DrawComparison(TH1F* h1, TH1F* h2, const char* title);

void Compare_146Sm_Data() {
    // 创建应用程序（保持程序运行）
    int argc = 0;
    char **argv = nullptr;
    TApplication app("app", &argc, argv);
    
    // 定义两个文件路径
    TString file1 = "/mnt/d/Sm146/146Sm45.root";
    TString file2 = "/mnt/d/Sm146-2025/2025_146Sm111.root";
    
    std::cout << "Opening file 1: " << file1 << std::endl;
    std::cout << "Opening file 2: " << file2 << std::endl;
    
    // 打开文件
    TFile* fRun1 = TFile::Open(file1);
    TFile* fRun2 = TFile::Open(file2);
    
    if (!fRun1 || !fRun2) {
        std::cout << "Error opening files!" << std::endl;
        return;
    }
    
    TTree* t1 = (TTree*)fRun1->Get("Tree1");
    TTree* t2 = (TTree*)fRun2->Get("Tree1");
    
    if (!t1 || !t2) {
        std::cout << "Error getting trees!" << std::endl;
        return;
    }
    
    std::cout << "File 1 entries: " << t1->GetEntries() << std::endl;
    std::cout << "File 2 entries: " << t2->GetEntries() << std::endl;
    
    // 定义变量
    Float_t E1f, E1b, E2f, E2b, E3f, E3b, E4f, E4b, xe;
    Float_t Emin = 0.5, Emax = 8, th = 0.5, thmax = 8;
    
    // 定义数组
    UInt_t bArray[32], cArray[32], dArray[32], eArray[32];
    
    // 第一个文件的刻度系数
    Float_t Calk1[4][32] = {
        {0.001642774,0.001651835,0.001608468,0.001641367,0.001576684,0.001640239,0.002455224,0.001601934,
          0.001587938,0.001627246,0.001611986,0.00168047,0.001624153,0.001670524,0.001636543,0.001641251,
          0.001749851,0.001684131,0.001678819,0.002428388,0.003890221,0.001805366,0.001711475,0.001683147,
          0.001648897,0.002115124,0.001629071,0,0.001885935,0.001722358,0.001649225,0.001671942},
         {0.00158142, 0.00157801, 0.00163209, 0.00160516, 0.00158022, 0.00166706, 0.00158586, 0.00161678, 
         0.00165936, 0.00167786, 0.00158946, 0.00186188, 0.00163951, 0.00159054, 0.00160591, 0.00156772, 
         0.00165706, 0.0016083, 0.00167759, 0.00161145, 0.00166573, 0.00165128, 0.00165605, 0.00161456, 
         0.0016445, 0.00158094, 0.00161622, 0.00163819, 0.0016396, 0.00166783, 0.00163079, 0.00162695},
         {0.00165951, 0.00168181, 0.00166228, 0.00167251, 0.00163437, 0.00167288, 0.00168972, 0.00162929, 
          0.00163325, 0.00164932, 0.00166735, 0.0017029, 0.00163851, 0.00165474, 0.00165569, 0.00165541, 
          0.00164467, 0.00163051, 0.00162451, 0.00168453, 0.0016282, 0.00165736, 0.00164959, 0.00165858, 
          0.00164866, 0.00167349, 0.00170528, 0.00169673, 0.00170055, 0.00171637, 0.00167252, 0.00158448},
         {0.00157584, 0.00154948, 0.00160235, 0.00153856, 0.00154766, 0.00156195, 0.00157738, 0.00152705, 
          0.00153825, 0.00157905, 0.0015297, 0.00153771, 0.00156201, 0.00150842, 0.00155083, 0.00150779, 
          0.00151516, 0.00155465, 0.00155169, 0.00158396, 0.00156146, 0.00162218, 0.00154954, 0.00156571, 
          0.00157495, 0.00155568, 0.00157732, 0.00159474, 0.00157166, 0.00152168, 0.00166922, 0.00154698}
    };

    Float_t Calb1[4][32] = {
         {-0.328435491,-0.278904899,-0.268454247,-0.315574318,-0.317141427,-0.316017503,-1.464001197,
           -0.298742845,-0.289806405,-0.371922391,-0.366959423,-0.391106758,-0.390996296,-0.340955822,
           -0.370696024,-0.309912598,-0.267508988,-0.238025103,-0.319187386,-1.19789601,-0.778423224,
           -0.381801814,-0.352011135,-0.332303527,-0.38984597,-0.77349919,-0.335810764,0,-0.42093706,
           -0.402912682,-0.317952841,-0.29958964},
          {0.0111801, 0.0151797, -0.0500475, -0.050221, -0.110727, -0.114079, -0.140961, -0.104585, 
           -0.184176, -0.160749, -0.174448, -0.316754, -0.118275, -0.0627836, -0.0596171, 0.000479147, 
           -0.142285, -0.118866, -0.18119, -0.154295, -0.18889, -0.149692, -0.158005, -0.166293, 
           -0.123126, -0.113371, -0.159062, -0.122975, -0.178798, -0.142715, -0.182475, -0.156589},
          {-0.251645, -0.229827, -0.237545, -0.226841, -0.228857, -0.231733, -0.234438, -0.25743, 
           -0.256989, -0.20435, -0.252861, -0.256635, -0.258212, -0.291914, -0.287968, -0.24038, 
           -0.235365, -0.243404, -0.296217, -0.292205, -0.297113, -0.264694, -0.33169, -0.270945, 
           -0.349385, -0.314098, -0.352844, -0.314638, -0.37893, -0.25178, -0.266134, -0.178252},
          {-0.191485, -0.171554, -0.147229, -0.19889, -0.188041, -0.136898, -0.163945, -0.145331, 
           -0.209174, -0.215382, -0.237406, -0.237256, -0.206838, -0.197602, -0.177536, -0.140724, 
           -0.0940485, -0.150724, -0.146914, -0.18587, -0.186787, -0.228114, -0.232219, -0.232274, 
           -0.234501, -0.245322, -0.233652, -0.173991, -0.166045, -0.147787, -0.31058, -0.14875}
    };
    
    // 第二个文件的刻度系数
    Float_t Calk2[4][32] = {
        {0.001591,0.001587,0.001598,0.001617,0.001547,0.001611,0.001599,0.001572,
        0.001554,0.001595,0.001580,0.001640,0.001583,0.001629,0.001593,0.001611,
        0.001673,0.001604,0.001613,0.001621,0.001633,0.001651,0.001555,0.001634,
        0.001589,0.001605,0.001582,0.001577,0.001599,0.001623,0.001605,0.001623},
        {0.001660,0.001635,0.001710,0.001638,0.001662,0.001665,0.001652,0.001651,
        0.001663,0.001601,0.001645,0.001657,0.001678,0.001687,0.001659,0.001662,
        0.001676,0.001625,0.001679,0.001632,0.001611,0.001693,0.001609,0.001631,
        0.001653,0.001667,0.001605,0.001651,0.001643,0.001641,0.001650,0.001635},
        {0.001623,0.001666,0.001644,0.001661,0.001626,0.001664,0.001674,0.001624,
        0.001624,0.001636,0.001656,0.001693,0.001620,0.001626,0.001617,0.001616,
        0.001681,0.001641,0.001624,0.001682,0.001610,0.001626,0.001622,0.001632,
        0.001636,0.001639,0.001656,0.001610,0.001619,0.001714,0.001681,0.001645},
        {0.001846,0.001821,0.001872,0.001805,0.001812,0.001812,0.001835,0.001789,
        0.001799,0.001852,0.001796,0.001812,0.001839,0.001777,0.001829,0.001777,
        0.001676,0.001696,0.001631,0.001676,0.001645,0.001699,0.001648,0.001666,
        0.001658,0.001654,0.001699,0.001686,0.001652,0.001596,0.001669,0.001625}
    };

    Float_t Calb2[4][32] = {
        {-0.2330,-0.2395,-0.2125,-0.2303,-0.2282,-0.2545,-0.2330,-0.2210,
        -0.2270,-0.2816,-0.2677,-0.2628,-0.2619,-0.2287,-0.2688,-0.2410,
        -0.2697,-0.2364,-0.2544,-0.2777,-0.3019,-0.2655,-0.2510,-0.2385,
        -0.2835,-0.2230,-0.2788,-0.2720,-0.2773,-0.2925,-0.2821,-0.2760},
        {-0.1975,-0.1685,-0.1720,-0.1481,-0.1973,-0.1733,-0.2054,-0.2028,
        -0.2314,-0.2011,-0.2399,-0.1891,-0.2331,-0.2271,-0.2209,-0.2121,
        -0.2779,-0.1979,-0.2573,-0.2250,-0.2171,-0.1991,-0.2024,-0.2366,
        -0.2206,-0.2109,-0.2555,-0.2020,-0.2325,-0.1975,-0.2391,-0.2299},
        {-0.1947,-0.1839,-0.1775,-0.1757,-0.1635,-0.1692,-0.1678,-0.2117,
        -0.2213,-0.1557,-0.2047,-0.2074,-0.1999,-0.2267,-0.2311,-0.2394,
        -0.2590,-0.2272,-0.2590,-0.2417,-0.2392,-0.1855,-0.2450,-0.1865,
        -0.2817,-0.2319,-0.2456,-0.1615,-0.2373,-0.2134,-0.2457,-0.2239},
        {-0.1690,-0.1542,-0.1117,-0.1729,-0.1612,-0.1301,-0.1515,-0.1380,
        -0.1799,-0.1759,-0.1907,-0.2194,-0.1769,-0.1807,-0.1754,-0.1279,
        -0.1621,-0.1759,-0.1494,-0.1661,-0.1424,-0.1610,-0.1831,-0.1726,
        -0.1538,-0.1891,-0.1708,-0.1206,-0.1360,-0.1434,-0.1814,-0.1774}
    };
    
    // 设置分支地址
    for(int i=0; i<32; i++) {
        t1->SetBranchAddress(Form("adc0ch%d", i), &bArray[i]);
        t1->SetBranchAddress(Form("adc1ch%d", i), &cArray[i]);
        t1->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);
        t1->SetBranchAddress(Form("adc3ch%d", i), &eArray[i]);
        
        t2->SetBranchAddress(Form("adc0ch%d", i), &bArray[i]);
        t2->SetBranchAddress(Form("adc1ch%d", i), &cArray[i]);
        t2->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);
        t2->SetBranchAddress(Form("adc3ch%d", i), &eArray[i]);
    }
    
    // 创建所有8个探测器的直方图
    TH1F *h1_E1f = new TH1F("h1_E1f", "E1f - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E1b = new TH1F("h1_E1b", "E1b - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E2f = new TH1F("h1_E2f", "E2f - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E2b = new TH1F("h1_E2b", "E2b - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E3f = new TH1F("h1_E3f", "E3f - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E3b = new TH1F("h1_E3b", "E3b - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E4f = new TH1F("h1_E4f", "E4f - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h1_E4b = new TH1F("h1_E4b", "E4b - 146Sm20;Energy (MeV);Counts", 400, Emin, Emax);
    
    TH1F *h2_E1f = new TH1F("h2_E1f", "E1f - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E1b = new TH1F("h2_E1b", "E1b - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E2f = new TH1F("h2_E2f", "E2f - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E2b = new TH1F("h2_E2b", "E2b - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E3f = new TH1F("h2_E3f", "E3f - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E3b = new TH1F("h2_E3b", "E3b - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E4f = new TH1F("h2_E4f", "E4f - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    TH1F *h2_E4b = new TH1F("h2_E4b", "E4b - 2025_146Sm26;Energy (MeV);Counts", 400, Emin, Emax);
    
    // 设置直方图样式
    // 文件1 - 蓝色
    h1_E1f->SetLineColor(kBlue); h1_E1f->SetLineWidth(2);
    h1_E1b->SetLineColor(kBlue); h1_E1b->SetLineWidth(2);
    h1_E2f->SetLineColor(kBlue); h1_E2f->SetLineWidth(2);
    h1_E2b->SetLineColor(kBlue); h1_E2b->SetLineWidth(2);
    h1_E3f->SetLineColor(kBlue); h1_E3f->SetLineWidth(2);
    h1_E3b->SetLineColor(kBlue); h1_E3b->SetLineWidth(2);
    h1_E4f->SetLineColor(kBlue); h1_E4f->SetLineWidth(2);
    h1_E4b->SetLineColor(kBlue); h1_E4b->SetLineWidth(2);
    
    // 文件2 - 红色
    h2_E1f->SetLineColor(kRed); h2_E1f->SetLineWidth(2);
    h2_E1b->SetLineColor(kRed); h2_E1b->SetLineWidth(2);
    h2_E2f->SetLineColor(kRed); h2_E2f->SetLineWidth(2);
    h2_E2b->SetLineColor(kRed); h2_E2b->SetLineWidth(2);
    h2_E3f->SetLineColor(kRed); h2_E3f->SetLineWidth(2);
    h2_E3b->SetLineColor(kRed); h2_E3b->SetLineWidth(2);
    h2_E4f->SetLineColor(kRed); h2_E4f->SetLineWidth(2);
    h2_E4b->SetLineColor(kRed); h2_E4b->SetLineWidth(2);
    
    // 处理第一个文件的数据
    std::cout << "Processing first file..." << std::endl;
    Long64_t ntot1 = t1->GetEntries();
    for(Long64_t i = 0; i < ntot1; i++) {
        t1->GetEntry(i);
        
        E1f = E1b = E2f = E2b = E3f = E3b = E4f = E4b = 0.;
        
        // 探测器1 (adc0)
        for(Int_t j = 0; j < 16; j++) {
            xe = bArray[j] * Calk1[0][j] + Calb1[0][j];
            if(xe > th) E1f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = bArray[j] * Calk1[0][j] + Calb1[0][j];
            if(xe > th) E1b += xe;
        }
        
        // 探测器2 (adc1)
        for(Int_t j = 0; j < 16; j++) {
            xe = cArray[j] * Calk1[1][j] + Calb1[1][j];
            if(xe > th) E2f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = cArray[j] * Calk1[1][j] + Calb1[1][j];
            if(xe > th) E2b += xe;
        }
        
        // 探测器3 (adc2)
        for(Int_t j = 0; j < 16; j++) {
            xe = dArray[j] * Calk1[2][j] + Calb1[2][j];
            if(xe > th) E3f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = dArray[j] * Calk1[2][j] + Calb1[2][j];
            if(xe > th) E3b += xe;
        }
        
        // 探测器4 (adc3)
        for(Int_t j = 0; j < 16; j++) {
            xe = eArray[j] * Calk1[3][j] + Calb1[3][j];
            if(xe > th) E4f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = eArray[j] * Calk1[3][j] + Calb1[3][j];
            if(xe > th) E4b += xe;
        }
        
        // 填充所有直方图
        if(E1f > th && E1f < thmax) h1_E1f->Fill(E1f);
        if(E1b > th && E1b < thmax) h1_E1b->Fill(E1b);
        if(E2f > th && E2f < thmax) h1_E2f->Fill(E2f);
        if(E2b > th && E2b < thmax) h1_E2b->Fill(E2b);
        if(E3f > th && E3f < thmax) h1_E3f->Fill(E3f);
        if(E3b > th && E3b < thmax) h1_E3b->Fill(E3b);
        if(E4f > th && E4f < thmax) h1_E4f->Fill(E4f);
        if(E4b > th && E4b < thmax) h1_E4b->Fill(E4b);
        
        if(i % 10000 == 0) std::cout << "File1: " << i << "/" << ntot1 << std::endl;
    }
    
    // 处理第二个文件的数据
    std::cout << "Processing second file..." << std::endl;
    Long64_t ntot2 = t2->GetEntries();
    for(Long64_t i = 0; i < ntot2; i++) {
        t2->GetEntry(i);
        
        E1f = E1b = E2f = E2b = E3f = E3b = E4f = E4b = 0.;
        
        // 探测器1 (adc0)
        for(Int_t j = 0; j < 16; j++) {
            xe = bArray[j] * Calk2[0][j] + Calb2[0][j];
            if(xe > th) E1f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = bArray[j] * Calk2[0][j] + Calb2[0][j];
            if(xe > th) E1b += xe;
        }
        
        // 探测器2 (adc1)
        for(Int_t j = 0; j < 16; j++) {
            xe = cArray[j] * Calk2[1][j] + Calb2[1][j];
            if(xe > th) E2f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = cArray[j] * Calk2[1][j] + Calb2[1][j];
            if(xe > th) E2b += xe;
        }
        
        // 探测器3 (adc2)
        for(Int_t j = 0; j < 16; j++) {
            xe = dArray[j] * Calk2[2][j] + Calb2[2][j];
            if(xe > th) E3f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = dArray[j] * Calk2[2][j] + Calb2[2][j];
            if(xe > th) E3b += xe;
        }
        
        // 探测器4 (adc3)
        for(Int_t j = 0; j < 16; j++) {
            xe = eArray[j] * Calk2[3][j] + Calb2[3][j];
            if(xe > th) E4f += xe;
        }
        for(Int_t j = 16; j < 32; j++) {
            xe = eArray[j] * Calk2[3][j] + Calb2[3][j];
            if(xe > th) E4b += xe;
        }
        
        // 填充所有直方图
        if(E1f > th && E1f < thmax) h2_E1f->Fill(E1f);
        if(E1b > th && E1b < thmax) h2_E1b->Fill(E1b);
        if(E2f > th && E2f < thmax) h2_E2f->Fill(E2f);
        if(E2b > th && E2b < thmax) h2_E2b->Fill(E2b);
        if(E3f > th && E3f < thmax) h2_E3f->Fill(E3f);
        if(E3b > th && E3b < thmax) h2_E3b->Fill(E3b);
        if(E4f > th && E4f < thmax) h2_E4f->Fill(E4f);
        if(E4b > th && E4b < thmax) h2_E4b->Fill(E4b);
        
        if(i % 10000 == 0) std::cout << "File2: " << i << "/" << ntot2 << std::endl;
    }
    
    // 输出统计信息
    std::cout << "\nHistogram statistics:" << std::endl;
    std::cout << "Detector 1 - File1: E1f=" << h1_E1f->GetEntries() << ", E1b=" << h1_E1b->GetEntries() << std::endl;
    std::cout << "Detector 2 - File1: E2f=" << h1_E2f->GetEntries() << ", E2b=" << h1_E2b->GetEntries() << std::endl;
    std::cout << "Detector 3 - File1: E3f=" << h1_E3f->GetEntries() << ", E3b=" << h1_E3b->GetEntries() << std::endl;
    std::cout << "Detector 4 - File1: E4f=" << h1_E4f->GetEntries() << ", E4b=" << h1_E4b->GetEntries() << std::endl;
    std::cout << "Detector 1 - File2: E1f=" << h2_E1f->GetEntries() << ", E1b=" << h2_E1b->GetEntries() << std::endl;
    std::cout << "Detector 2 - File2: E2f=" << h2_E2f->GetEntries() << ", E2b=" << h2_E2b->GetEntries() << std::endl;
    std::cout << "Detector 3 - File2: E3f=" << h2_E3f->GetEntries() << ", E3b=" << h2_E3b->GetEntries() << std::endl;
    std::cout << "Detector 4 - File2: E4f=" << h2_E4f->GetEntries() << ", E4b=" << h2_E4b->GetEntries() << std::endl;
    
    // 创建画布来显示所有8个探测器
    TCanvas *myc1 = new TCanvas("myc1", "Detectors 1-4 Comparison", 0, 0, 1400, 1000);
    myc1->Divide(4, 2);  // 4列2行
    
    // 第一行：探测器1-4的前端
    myc1->cd(1); DrawComparison(h1_E1f, h2_E1f, "E1f");
    myc1->cd(2); DrawComparison(h1_E2f, h2_E2f, "E2f");
    myc1->cd(3); DrawComparison(h1_E3f, h2_E3f, "E3f");
    myc1->cd(4); DrawComparison(h1_E4f, h2_E4f, "E4f");
    
    // 第二行：探测器1-4的后端
    myc1->cd(5); DrawComparison(h1_E1b, h2_E1b, "E1b");
    myc1->cd(6); DrawComparison(h1_E2b, h2_E2b, "E2b");
    myc1->cd(7); DrawComparison(h1_E3b, h2_E3b, "E3b");
    myc1->cd(8); DrawComparison(h1_E4b, h2_E4b, "E4b");
    
    // 保存画布
    myc1->SaveAs("146Sm_all_detectors_comparison.png");
    
    std::cout << "Comparison complete! Results saved to 146Sm_all_detectors_comparison.png" << std::endl;
    std::cout << "Close the canvas window to exit." << std::endl;
    
    // 运行应用程序，保持窗口打开
    //app.Run();
    std::cout << "Press Ctrl+C to exit..." << std::endl;
while (gSystem->ProcessEvents()) {
    gSystem->Sleep(100); // 每100毫秒检查一次事件
}
    
    // 清理
    fRun1->Close();
    fRun2->Close();
}

// 辅助函数：绘制比较图
void DrawComparison(TH1F* h1, TH1F* h2, const char* title) {
    double max1 = h1->GetMaximum();
    double max2 = h2->GetMaximum();
    double ymax = TMath::Max(max1, max2) * 1.2;
    h1->SetMaximum(ymax);
    
    h1->Draw("HIST");
    h2->Draw("HIST SAME");
    
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->AddEntry(h1, "146Sm20", "l");
    leg->AddEntry(h2, "2025_146Sm26", "l");
    leg->SetHeader(title);
    leg->Draw();
}

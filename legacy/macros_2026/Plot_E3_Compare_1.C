#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TLegend.h"
#include <iostream>

void Plot_E3_Compare_1() {
    // 1. 设置全局样式
    gStyle->SetOptStat(1111);
    gStyle->SetGridColor(kGray);
    
    // 2. 定义要比较的两个 Run 号
    Int_t run1 = 35;  // <<< 修改: 34 -> 35
    Int_t run2 = 29;  // <<< 修改: 28 -> 29
    
    // 3. 定义文件名
    TString fin1 = TString::Format("2026_146Sm%d.root", run1);
    TString fin2 = TString::Format("2026_146Sm%d.root", run2);
    
    std::cout << "Opening file for Run " << run1 << ": " << fin1 << std::endl;
    std::cout << "Opening file for Run " << run2 << ": " << fin2 << std::endl;

    // 4. 打开文件
    TFile* fRun1 = new TFile(fin1, "READ");
    TFile* fRun2 = new TFile(fin2, "READ");
    
    if (!fRun1 || fRun1->IsZombie()) { 
        std::cout << "Error: Cannot open file " << fin1 << std::endl; 
        return; 
    }
    if (!fRun2 || fRun2->IsZombie()) { 
        std::cout << "Error: Cannot open file " << fin2 << std::endl; 
        return; 
    }
    
    // 5. 获取 Tree
    TTree* t1 = (TTree*)fRun1->Get("Tree1");
    TTree* t2 = (TTree*)fRun2->Get("Tree1");
    
    if (!t1) { std::cout << "Error: Tree1 not found in file " << fin1 << std::endl; return; }
    if (!t2) { std::cout << "Error: Tree1 not found in file " << fin2 << std::endl; return; }
    
    // 6. 能量与重数变量
    Float_t E3f_1 = 0, E3f_2 = 0;
    Int_t n3f_1 = 0, n3f_2 = 0;
    Float_t xe;
    
    // 7. 全局能量范围
    Float_t Emin = 0.5, Emax = 8.0; 
    Float_t thmax = 8.0;
    
    // 8. ADC 数组
    UInt_t dArray[32];

    // 9. 旧校准系数 (用于 Run 29) - 保持不变
    Float_t Calk_old[4][32] = {  // <<< 修改: 重命名为 Calk_old 以示区分
        {0.001740, 0.001760, 0.001721, 0.001747, 0.001679, 0.001751, 0.001764, 0.001707, 0.001659, 0.001703, 0.001680, 0.001734, 0.001659, 0.001702, 0.001692, 0.001685, 0.001768, 0.001732, 0.001735, 0.001824, 0.001993, 0.001819, 0.001768, 0.001772, 0.001727, 0.001750, 0.001713, 0.001787, 0.001816, 0.001765, 0.001748, 0.001807},
        {0.001734, 0.001688, 0.001726, 0.001684, 0.001675, 0.001674, 0.001648, 0.001622, 0.001750, 0.001701, 0.001737, 0.001780, 0.001797, 0, 0.001787, 0.001804, 0.001734, 0.001721, 0.001739, 0.001724, 0.001714, 0.001792, 0.001730, 0, 0.001820, 0.001836, 0.001745, 0.001826, 0.001784, 0.001757, 0.001765, 0.001724},
        {0.001773, 0.001755, 0.001731, 0.001757, 0.001853, 0.001730, 0.001768, 0.001727, 0.001731, 0.001744, 0.001782, 0.001824, 0.001752, 0.001775, 0.001776, 0.001781, 0.001871, 0.001878, 0.001751, 0.001857, 0.001863, 0.001789, 0.001772, 0.001782, 0.001776, 0.001813, 0.001828, 0.002091, 0.001808, 0.001833, 0.002214, 0.001693},
        {0.001685, 0.001657, 0.001710, 0.001662, 0.001661, 0.001710, 0.001734, 0.001680, 0.001734, 0.001771, 0.001697, 0.001705, 0.001730, 0.001671, 0.001720, 0.001658, 0.001728, 0.001759, 0.001805, 0.001824, 0.001747, 0.001884, 0.001945, 0.001771, 0.001768, 0.002034, 0.001817, 0.001909, 0.001826, 0.001689, 0.001737, 0.001710}
    };
    
    Float_t Calb_old[4][32] = {  // <<< 修改: 重命名为 Calb_old
        {-0.50890, -0.54183, -0.48875, -0.50749, -0.51723, -0.53747, -0.53228, -0.49493, -0.32053, -0.48029, -0.46118, -0.41832, -0.36987, -0.30408, -0.43591, -0.32569, -0.39668, -0.39924, -0.44618, -0.48978, -0.78831, -0.56262, -0.62340, -0.51432, -0.55930, -0.48156, -0.53878, -0.59838, -0.50738, -0.51866, -0.49980, -0.53247},
        {-0.18038, -0.09261, -0.06643, -0.00799, 0.04109, 0.08761, 0.12748, 0.16480, -0.25332, -0.27783, -0.32277, -0.31061, -0.37897, 0.00000, -0.39959, -0.43536, -0.20154, -0.16639, -0.21253, -0.26914, -0.29528, -0.28367, -0.31974, 0.00000, -0.36966, -0.36811, -0.38447, -0.35380, -0.31478, -0.25963, -0.27924, -0.20009},
        {-0.27004, -0.25227, -0.23558, -0.26722, -0.68100, -0.15665, -0.19222, -0.24196, -0.37073, -0.30952, -0.40052, -0.42106, -0.43376, -0.47924, -0.46643, -0.44120, -0.61929, -0.55756, -0.43141, -0.50255, -0.60052, -0.45943, -0.50074, -0.45410, -0.52953, -0.47264, -0.50131, -0.77004, -0.44094, -0.40857, -0.89220, -0.34360},
        {-0.23196, -0.21839, -0.20562, -0.32787, -0.31899, -0.33523, -0.39382, -0.38956, -0.51203, -0.50809, -0.52418, -0.53612, -0.49924, -0.48613, -0.47552, -0.39584, -0.43253, -0.46294, -0.48217, -0.49134, -0.48847, -0.50930, -0.70391, -0.51208, -0.52585, -0.80519, -0.52026, -0.48984, -0.44159, -0.41546, -0.43722, -0.36838}
    };

    // <<< 新增: 新校准系数 (仅 Ring2，用于 Run 35)
    Float_t Calk_new[32] = {
        0.001829, 0.001791, 0.001804, 0.001812, 0.001778, 0.001821, 0.001877, 0.001848,
        0.001763, 0.001779, 0.001811, 0.001829, 0.001769, 0.001793, 0.001790, 0.001796,
        0.001861, 0.001922, 0.001806, 0.001880, 0.001909, 0.001800, 0.001756, 0.001794,
        0.001781, 0.001848, 0.001840, 0.002121, 0.001841, 0.001858, 0.002262, 0.001728
    };
    Float_t Calb_new[32] = {
        -0.45127, -0.37073, -0.47633, -0.44649, -0.43261, -0.45167, -0.54015, -0.64220,
        -0.48082, -0.42821, -0.49567, -0.43593, -0.48845, -0.53866, -0.51043, -0.49217,
        -0.58638, -0.69871, -0.61878, -0.57793, -0.74871, -0.49735, -0.44871, -0.49574,
        -0.54674, -0.58882, -0.54044, -0.86061, -0.54727, -0.48703, -1.03062, -0.46613
    };

    // 10. 阈值数组 - 保持不变
    UInt_t thArray[4][32];
    for(int i=0; i<4; i++) for(int j=0; j<32; j++) thArray[i][j] = 180;
    
    thArray[0][2] = 160; thArray[0][3] = 165; thArray[0][7] = 170; thArray[0][9] = 200;
    thArray[0][10] = 200; thArray[0][11] = 190; thArray[0][12] = 200; thArray[0][14] = 205;
    thArray[1][0] = 190; thArray[1][2] = 190; thArray[1][3] = 200; thArray[1][4] = 215;
    thArray[1][5] = 190; thArray[1][6] = 190; thArray[1][8] = 210; thArray[1][10] = 210;
    thArray[1][11] = 200; thArray[1][12] = 205; thArray[1][13] = 210; thArray[1][14] = 210;
    thArray[1][15] = 200;
    thArray[2][0] = 150; thArray[2][1] = 140; thArray[2][2] = 130; thArray[2][3] = 120;
    thArray[2][4] = 150; thArray[2][5] = 150; thArray[2][8] = 160; thArray[2][9] = 150;
    thArray[2][10] = 170; thArray[2][11] = 140; thArray[2][12] = 170; thArray[2][13] = 150;
    thArray[2][14] = 160; thArray[2][15] = 180;
    thArray[3][0] = 160; thArray[3][1] = 150; thArray[3][2] = 170; thArray[3][3] = 170;
    thArray[3][4] = 170; thArray[3][5] = 150; thArray[3][6] = 160; thArray[3][7] = 180;
    thArray[3][8] = 170; thArray[3][9] = 160; thArray[3][10] = 180; thArray[3][11] = 160;
    thArray[3][12] = 180; thArray[3][13] = 160; thArray[3][14] = 200; thArray[3][15] = 180;

    // 11. 创建直方图
    TH1F *h_E3f_Run1 = new TH1F("h_E3f_Run1", "Detector 3f Comparison;Energy (MeV);Counts", 400, Emin, Emax);  // <<< 修改: 通用命名
    TH1F *h_E3f_Run2 = new TH1F("h_E3f_Run2", "Detector 3f Comparison;Energy (MeV);Counts", 400, Emin, Emax);  // <<< 修改: 通用命名

    // 12. 处理 Run 35 (第一个文件) - 使用新系数
    Long64_t ntot1 = t1->GetEntriesFast();
    std::cout << "Processing " << ntot1 << " entries for Run " << run1 << std::endl;

    for(int i=0; i<32; i++) t1->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);

    for(Long64_t i=0; i<ntot1; i++) {
        t1->GetEntry(i);
        
        E3f_1 = 0.0;
        n3f_1 = 0;
        
        for(Int_t j=0; j<16; j++) {
            if(dArray[j] > thArray[2][j]) {
                xe = dArray[j] * Calk_new[j] + Calb_new[j];  // <<< 修改: 使用新系数
                E3f_1 += xe;
                n3f_1++;
            }
        }
        
        if(n3f_1 >= 2) E3f_1 = 0;
        
        if(E3f_1 > 0 && E3f_1 < thmax) h_E3f_Run1->Fill(E3f_1);  // <<< 修改: 对应新直方图名
    }

    // 13. 处理 Run 29 (第二个文件) - 使用旧系数
    Long64_t ntot2 = t2->GetEntriesFast();
    std::cout << "Processing " << ntot2 << " entries for Run " << run2 << std::endl;

    for(int i=0; i<32; i++) t2->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);

    for(Long64_t i=0; i<ntot2; i++) {
        t2->GetEntry(i);
        
        E3f_2 = 0.0;
        n3f_2 = 0;
        
        for(Int_t j=0; j<16; j++) {
            if(dArray[j] > thArray[2][j]) {
                xe = dArray[j] * Calk_old[2][j] + Calb_old[2][j];  // <<< 修改: 显式使用旧系数
                E3f_2 += xe;
                n3f_2++;
            }
        }
        
        if(n3f_2 >= 2) E3f_2 = 0;
        
        if(E3f_2 > 0 && E3f_2 < thmax) h_E3f_Run2->Fill(E3f_2);  // <<< 修改: 对应新直方图名
    }

    // 14. 绘图
    TCanvas *c1 = new TCanvas("c1", Form("E3f Comparison: Run %d vs Run %d", run1, run2), 1000, 2000);  // <<< 修改: 动态标题
    c1->SetGrid();
    
    // 自动设置Y轴最大值，避免硬编码250导致谱线截断或空白  // <<< 修改
    Float_t globalMax = TMath::Max(h_E3f_Run1->GetMaximum(), h_E3f_Run2->GetMaximum());
    h_E3f_Run1->SetMaximum(globalMax * 1.2);
    
    h_E3f_Run1->SetLineColor(kRed);
    h_E3f_Run1->SetLineWidth(2);
    h_E3f_Run1->SetStats(0);

    h_E3f_Run2->SetLineColor(kBlue);
    h_E3f_Run2->SetLineWidth(2);
    h_E3f_Run2->SetStats(0);

    h_E3f_Run1->Draw(); 
    h_E3f_Run2->Draw("SAME");

    // 15. 添加图例
    TLegend *leg = new TLegend(0.7, 0.7, 0.88, 0.85);
    leg->AddEntry(h_E3f_Run1, Form("Run %d (New Calib)", run1), "l");  // <<< 修改: 正确标签
    leg->AddEntry(h_E3f_Run2, Form("Run %d (Old Calib)", run2), "l");  // <<< 修改: 正确标签
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->Draw();

    // 16. 保存与显示
    c1->Update();
    TString outName = Form("E3f_Run%d_vs_Run%d.pdf", run1, run2);  // <<< 修改: 动态文件名
    c1->SaveAs(outName);
    std::cout << "Plot finished and saved as " << outName << std::endl;
}
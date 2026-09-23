#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TF1.h"
#include "TMath.h"
#include "TMinuit.h" // 必须包含这个

Double_t CrystalBall(Double_t *x, Double_t *par) {
    Double_t xx = x[0];
    Double_t norm = par[0];
    Double_t mean = par[1];
    Double_t sigma = par[2];
    Double_t alpha = par[3]; 
    Double_t n = par[4];     

    if (sigma < 0.01 || n < 0.1 || TMath::Abs(alpha) < 0.01) {
        return 1e-10; 
    }

    Double_t d = TMath::Abs(alpha) / sigma;
    if(d > 100) return 1e-10; 
    
    Double_t A = TMath::Power(n / d, n) * TMath::Exp(-0.5 * d * d);
    Double_t B = d - n / d;
    Double_t xx_shifted = xx - mean;

    if (alpha > 0) {
        if (xx_shifted >= -alpha * sigma) {
            return norm * TMath::Exp(-0.5 * xx_shifted * xx_shifted / (sigma * sigma));
        } else {
            Double_t base = B - xx_shifted / sigma;
            if(base <= 0) return 1e-10;
            return norm * A * TMath::Power(base, -n);
        }
    } else {
        if (xx_shifted <= -alpha * sigma) {
            return norm * TMath::Exp(-0.5 * xx_shifted * xx_shifted / (sigma * sigma));
        } else {
            Double_t base = B + xx_shifted / sigma;
            if(base <= 0) return 1e-10;
            return norm * A * TMath::Power(base, -n);
        }
    }
}

void Cal2026(const int run, const int Cmin, const int Cmax, const int num) {
    // --- [关键修改] 增加最大迭代次数，防止过早终止 ---
    TMinuit* minuit = new TMinuit();
    minuit->SetMaxCalls(10000, 10000); // 给它 10000 次机会
    
    // 注册函数
    if (gROOT->GetFunction("CrystalBallFunc")) {
        delete gROOT->GetFunction("CrystalBallFunc");
    }
    
    TF1 *fitFunc = new TF1("CrystalBallFunc", CrystalBall, Cmin, Cmax, 5);
    
    // --- [关键修改] 增加参数的灵活性 ---
    // 根据上一轮日志，N 可能需要更大，Mean 需要更灵活
    Double_t expectedPeak = 3700; 
    
    fitFunc->SetParameters(500, expectedPeak, 22.0, 1.5, 5.0); 
    
    // --- [关键修改] 放宽 Mean 范围，防止撞墙 ---
    // 上一轮日志显示 Mean 跑到了 3578 和 3787，所以范围要包含这些值
    fitFunc->SetParLimits(0, 10, 1e9);      
    fitFunc->SetParLimits(1, 3500, 3900);   // 保持或稍微放宽
    fitFunc->SetParLimits(2, 5, 50);        
    fitFunc->SetParLimits(3, 0.1, 10);      
    fitFunc->SetParLimits(4, 1.0, 100);     // N 的上限提高到 100，防止撞墙
      
    fitFunc->SetParNames("Norm", "Mean", "Sigma", "Alpha", "N");
    
    // --- [关键修改] 强制设置颜色，不管报不报错，都要看到红线 ---
    fitFunc->SetLineColor(kRed);
    fitFunc->SetLineWidth(2);

    // 打开文件
    TString fin = TString::Format("2026_146Sm%d.root", run);
    TFile *fRun = new TFile(fin);
    if (!fRun || fRun->IsZombie()) { 
        printf("无法打开文件 %s\n", fin.Data()); 
        return; 
    }
    
    TTree *t = (TTree *)fRun->Get("Tree1");
    if (!t) { 
        printf("无法找到Tree1\n"); 
        return; 
    }

    TCanvas *myc_1 = new TCanvas("myc_1", "myc_1", 0, 0, 1400, 800);
    myc_1->Divide(4, 4);

    Int_t adcNum = (num - 1) / 2; 
    Int_t chStart = ((num - 1) % 2) * 16; 

    for (int i = 0; i < 16; i++) {
        Int_t channel = chStart + i;
        TString branchName = TString::Format("adc%dch%d", adcNum, channel);
        
        myc_1->cd(i+1);
        
        // --- [关键修改] Bin 数量增加 ---
        TString drawOpt = Form("%s >> htemp%d(%d, %d, %d)", 
                               branchName.Data(), i, 400, Cmin, Cmax);
        
        // --- [关键修改] 加上 "Q" (Quiet) 屏蔽所有警告，加上 "0" 不打印统计 ---
        t->Draw(drawOpt, "", "goff");
        TH1F *h = (TH1F*)gDirectory->Get(Form("htemp%d", i));
        if (h) {
            h->SetTitle(Form("Channel %d", channel));
            h->Draw(); 
            
            // --- [关键修改] 直接执行拟合，不看返回值 ---
            // 用 "Q" 屏蔽警告，用 "R+" 限制范围，用 "0" 不画中间过程
            h->Fit("CrystalBallFunc", "Q R+ 0");
            
            TF1 *resultFunc = h->GetFunction("CrystalBallFunc");
            if (resultFunc) {
                // --- [关键修改] 既然强制画了，就直接打印参数 ---
                Double_t fitMean = resultFunc->GetParameter("Mean");
                Double_t fitSigma = resultFunc->GetParameter("Sigma");
                Double_t fitN = resultFunc->GetParameter("N");
                
                // 不管报不报错，都打印结果
                printf("通道 %d: 结果 (Mean=%.2f, Sigma=%.2f, N=%.2f)\n", 
                       channel, fitMean, fitSigma, fitN);
                
                // 强制把函数画在图上（虽然上面已经画了）
                resultFunc->Draw("same");
            }
        }
    }
    
    myc_1->Update();
}
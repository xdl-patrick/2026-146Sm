// do_background.C
// 直接在ROOT中运行

{
    // 这是一个简单的ROOT脚本，不是C++函数
    
    cout << "========================================" << endl;
    cout << "   生成本底扣除后文件" << endl;
    cout << "========================================" << endl;
    
    // 打开输出文件
    TFile* fout = new TFile("2025_146Sm470.root", "RECREATE");
    
    // 只处理前几个通道测试
    int maxChannels = 4;  // 先测试4个通道
    
    for (int adc = 0; adc < 1; adc++) {  // 只处理ADC0
        for (int ch = 0; ch < maxChannels; ch++) {
            TString histName = Form("adc%dch%d", adc, ch);
            cout << "处理: " << histName << endl;
            
            // 1. 读取样品
            TFile* fSample = TFile::Open("2025_146Sm370.root");
            TH1I* hSample = (TH1I*)fSample->Get(histName);
            
            if (!hSample) {
                fSample->Close();
                continue;
            }
            
            // 2. 读取本底
            double bgSum = 0;
            int bgCount = 0;
            
            for (int run = 98; run <= 101; run++) {
                TString bgFile = Form("2025_146Sm%d.root", run);
                TFile* fBg = TFile::Open(bgFile);
                if (fBg) {
                    TH1I* hBg = (TH1I*)fBg->Get(histName);
                    if (hBg) {
                        bgSum += hBg->Integral();
                        bgCount++;
                    }
                    fBg->Close();
                }
            }
            
            // 3. 创建结果
            TH1F* hResult = new TH1F(histName, 
                                   Form("ADC%d Ch%d (BG Subtracted)", adc, ch),
                                   hSample->GetNbinsX(),
                                   hSample->GetXaxis()->GetXmin(),
                                   hSample->GetXaxis()->GetXmax());
            
            // 4. 扣除本底（简单平均）
            if (bgCount > 0) {
                double bgPerBin = bgSum / bgCount / hSample->GetNbinsX();
                
                for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
                    double val = hSample->GetBinContent(bin) - bgPerBin;
                    if (val < 0) val = 0;
                    hResult->SetBinContent(bin, val);
                }
                
                cout << "  " << histName << ": 样品=" << hSample->Integral() 
                     << ", 本底=" << bgSum/bgCount 
                     << ", 结果=" << hResult->Integral() << endl;
            } else {
                // 没有本底，直接复制
                for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
                    hResult->SetBinContent(bin, hSample->GetBinContent(bin));
                }
                cout << "  " << histName << ": 无本底数据，直接复制" << endl;
            }
            
            // 5. 保存
            fout->cd();
            hResult->Write();
            
            // 清理
            delete hResult;
            fSample->Close();
        }
    }
    
    fout->Close();
    cout << "\n测试完成！生成文件: 2025_146Sm360_bgsub.root" << endl;
}
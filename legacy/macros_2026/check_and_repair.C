// check_and_repair.C
// 检查并修复损坏的文件

void CheckAndRepair() {
    cout << "检查损坏的文件..." << endl;
    
    // 1. 检查原始文件
    cout << "\n1. 检查原始文件..." << endl;
    TFile* f360 = TFile::Open("2025_146Sm360.root");
    if (!f360 || f360->IsZombie()) {
        cout << "错误：原始文件损坏或无法打开" << endl;
        return;
    }
    cout << "原始文件正常，大小: " << f360->GetSize() / 1024 / 1024 << " MB" << endl;
    f360->Close();
    
    // 2. 检查本底文件
    cout << "\n2. 检查本底文件..." << endl;
    for (int run = 98; run <= 101; run++) {
        TString filename = Form("2025_146Sm%d.root", run);
        TFile* f = TFile::Open(filename);
        if (!f || f->IsZombie()) {
            cout << "警告：文件 " << filename << " 可能损坏" << endl;
        } else {
            cout << "  " << filename << " OK, 大小: " << f->GetSize() / 1024 << " KB" << endl;
            f->Close();
        }
    }
    
    // 3. 检查生成的bgsub文件
    cout << "\n3. 检查生成的文件..." << endl;
    TFile* fbgsub = TFile::Open("2025_146Sm360_bgsub.root");
    if (!fbgsub) {
        cout << "文件不存在" << endl;
    } else if (fbgsub->IsZombie()) {
        cout << "文件是僵尸文件（已损坏）" << endl;
        
        // 尝试恢复
        cout << "尝试恢复数据..." << endl;
        
        // 方法1：使用TFileRecover（如果可用）
        // TFileRecover recover("2025_146Sm360_bgsub.root");
        // recover.Recover();
        
        // 方法2：检查文件大小
        Long64_t size = gSystem->GetPathInfo("2025_146Sm360_bgsub.root", 0, 0, 0, 0);
        cout << "文件大小: " << size << " 字节" << endl;
        
        if (size == 0) {
            cout << "文件为空，删除并重新生成" << endl;
            gSystem->Unlink("2025_146Sm360_bgsub.root");
        }
        
        fbgsub->Close();
    } else {
        cout << "文件正常，大小: " << fbgsub->GetSize() / 1024 / 1024 << " MB" << endl;
        cout << "文件内容:" << endl;
        fbgsub->ls();
        fbgsub->Close();
    }
}

// 快速修复：只生成最重要的通道
void QuickFix() {
    cout << "快速修复：生成关键通道..." << endl;
    
    TFile* fout = new TFile("2025_146Sm360_bgsub_quick.root", "RECREATE");
    
    // 只处理前2个ADC的前8个通道
    int channels_to_process = 16;
    
    for (int adc = 0; adc < 2; adc++) {
        for (int ch = 0; ch < 8; ch++) {
            TString histName = Form("adc%dch%d", adc, ch);
            cout << "处理 " << histName << "..." << endl;
            
            // 读取样品
            TFile* fSample = TFile::Open("2025_146Sm360.root");
            TH1I* hSample = (TH1I*)fSample->Get(histName);
            
            if (!hSample) {
                fSample->Close();
                continue;
            }
            
            // 简单本底扣除：使用平均本底
            double sampleTotal = hSample->Integral();
            double bgTotal = 0;
            int bgCount = 0;
            
            for (int run = 98; run <= 101; run++) {
                TString bgFile = Form("2025_146Sm%d.root", run);
                TFile* fBg = TFile::Open(bgFile);
                if (fBg) {
                    TH1I* hBg = (TH1I*)fBg->Get(histName);
                    if (hBg) {
                        bgTotal += hBg->Integral();
                        bgCount++;
                    }
                    fBg->Close();
                }
            }
            
            // 创建结果
            TH1F* hResult = new TH1F(histName, 
                                    Form("ADC%d Ch%d (Quick BG Sub)", adc, ch),
                                    hSample->GetNbinsX(),
                                    hSample->GetXaxis()->GetXmin(),
                                    hSample->GetXaxis()->GetXmax());
            
            if (bgCount > 0) {
                double bgPerBin = (bgTotal / bgCount) / hSample->GetNbinsX();
                
                for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
                    double val = hSample->GetBinContent(bin) - bgPerBin;
                    if (val < 0) val = 0;
                    hResult->SetBinContent(bin, val);
                }
            } else {
                for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
                    hResult->SetBinContent(bin, hSample->GetBinContent(bin));
                }
            }
            
            // 保存
            fout->cd();
            hResult->Write();
            
            delete hResult;
            fSample->Close();
        }
    }
    
    fout->Close();
    cout << "\n快速修复完成！文件: 2025_146Sm360_bgsub_quick.root" << endl;
}
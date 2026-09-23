// simple_channel_by_channel.C
// 逐个通道处理，最安全的方法

void ProcessChannelByChannel(int adc, int ch) {
    TString histName = Form("adc%dch%d", adc, ch);
    cout << "处理 " << histName << "..." << endl;
    
    // 读取样品
    TFile* fSample = TFile::Open("2025_146Sm360.root");
    TH1I* hSample = (TH1I*)fSample->Get(histName);
    
    if (!hSample) {
        cout << "  跳过：未找到" << endl;
        fSample->Close();
        return;
    }
    
    // 读取本底
    TH1F* hBgAvg = nullptr;
    int bgCount = 0;
    
    for (int run = 98; run <= 101; run++) {
        TString bgFile = Form("2025_146Sm%d.root", run);
        TFile* fBg = TFile::Open(bgFile);
        if (fBg) {
            TH1I* hBg = (TH1I*)fBg->Get(histName);
            if (hBg) {
                if (!hBgAvg) {
                    hBgAvg = new TH1F(Form("bg_%s", histName.Data()), 
                                     Form("Background %s", histName.Data()),
                                     hBg->GetNbinsX(),
                                     hBg->GetXaxis()->GetXmin(),
                                     hBg->GetXaxis()->GetXmax());
                }
                hBgAvg->Add(hBg);
                bgCount++;
            }
            fBg->Close();
        }
    }
    
    // 打开输出文件（追加模式）
    TFile* fout = TFile::Open("2025_146Sm360_bgsub_safe.root", "UPDATE");
    if (!fout) {
        fout = new TFile("2025_146Sm360_bgsub_safe.root", "RECREATE");
    }
    
    // 创建结果
    TH1F* hResult = new TH1F(histName, 
                            Form("ADC%d Ch%d (BG Subtracted)", adc, ch),
                            hSample->GetNbinsX(),
                            hSample->GetXaxis()->GetXmin(),
                            hSample->GetXaxis()->GetXmax());
    
    if (bgCount > 0) {
        hBgAvg->Scale(1.0 / bgCount);
        
        for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
            double sample = hSample->GetBinContent(bin);
            double bg = hBgAvg->GetBinContent(bin);
            double result = sample - bg;
            if (result < 0) result = 0;
            hResult->SetBinContent(bin, result);
        }
        
        delete hBgAvg;
    } else {
        // 直接复制
        for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
            hResult->SetBinContent(bin, hSample->GetBinContent(bin));
        }
    }
    
    // 写入
    fout->cd();
    hResult->Write("", TObject::kOverwrite);
    
    // 清理
    delete hResult;
    fout->Close();
    fSample->Close();
    
    cout << "  完成: 样品=" << hSample->Integral() 
         << ", 结果=" << hResult->Integral() << endl;
}

void BuildFileSafely() {
    cout << "安全构建本底扣除文件..." << endl;
    
    // 先删除旧文件（如果存在）
    gSystem->Unlink("2025_146Sm360_bgsub_safe.root");
    
    // 逐个处理通道
    for (int adc = 0; adc < 4; adc++) {
        for (int ch = 0; ch < 32; ch++) {
            ProcessChannelByChannel(adc, ch);
            
            // 每8个通道显示一次进度
            if ((adc * 32 + ch + 1) % 8 == 0) {
                cout << "进度: " << (adc * 32 + ch + 1) << "/128 通道" << endl;
            }
        }
    }
    
    // 复制其他数据
    cout << "\n复制其他数据..." << endl;
    TFile* fOrig = TFile::Open("2025_146Sm360.root");
    TFile* fout = TFile::Open("2025_146Sm360_bgsub_safe.root", "UPDATE");
    
    if (fOrig && fout) {
        // 复制Trees
        for (int i = 0; i < 4; i++) {
            TString treeName = Form("Tree%d", i);
            TTree* tree = (TTree*)fOrig->Get(treeName);
            if (tree) {
                fout->cd();
                TTree* newTree = tree->CloneTree();
                newTree->Write();
                delete newTree;
                cout << "复制: " << treeName << endl;
            }
        }
        
        // 复制Scaler
        TH1I* scaler = (TH1I*)fOrig->Get("scaler");
        if (scaler) {
            fout->cd();
            scaler->Write();
            cout << "复制: scaler" << endl;
        }
    }
    
    if (fOrig) fOrig->Close();
    if (fout) fout->Close();
    
    cout << "\n完成！文件: 2025_146Sm360_bgsub_safe.root" << endl;
}
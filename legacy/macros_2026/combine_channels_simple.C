// combine_channels_simple.C
void combine_channels_simple() {
    // 输入文件列表
    const char* files[] = {
        "2025_146Sm86.root",
        "2025_146Sm89.root",
        "2025_146Sm90.root",
        "2025_146Sm91.root",
       // "2025_146Sm-8.2830.root",
       // "2025_146Sm-8.2832.root",
       // "2025_146Sm-8.2841.root",
        nullptr
    };
    
    // 创建输出文件
    TFile* outputFile = new TFile("combined_result.root", "RECREATE");
    
    // 从第一个文件获取通道列表
    TFile* firstFile = TFile::Open(files[0]);
    TList* keys = firstFile->GetListOfKeys();
    
    // 处理每个通道
    TIter next(keys);
    TKey* key;
    while ((key = (TKey*)next())) {
        TString name = key->GetName();
        
        // 只处理ADC通道
        if (name.BeginsWith("adc") && name.Contains("ch")) {
            // 获取模板直方图
            TH1I* templateHist = (TH1I*)firstFile->Get(name);
            TH1I* sumHist = (TH1I*)templateHist->Clone(name);
            sumHist->Reset();
            sumHist->SetTitle(Form("Sum of %s", name.Data()));
            
            cout << "Processing channel: " << name << endl;
            
            // 叠加所有文件
            for (int i = 0; files[i] != nullptr; i++) {
                TFile* f = TFile::Open(files[i]);
                if (f && !f->IsZombie()) {
                    TH1I* hist = (TH1I*)f->Get(name);
                    if (hist) {
                        sumHist->Add(hist);
                        cout << "  Added " << files[i] << " (" << hist->GetEntries() << " entries)" << endl;
                    }
                    f->Close();
                }
            }
            
            // 保存结果
            outputFile->cd();
            sumHist->Write();
            cout << "Total entries: " << sumHist->GetEntries() << endl;
        }
    }
    
    firstFile->Close();
    outputFile->Close();
    
    cout << "Channel combination complete!" << endl;
    cout << "Results saved to: combined_result.root" << endl;
}

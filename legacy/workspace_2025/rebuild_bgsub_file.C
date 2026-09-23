// rebuild_bgsub_file.C
// 重新构建本底扣除文件，更安全的方法

#include <iostream>

void RebuildBackgroundSubtractedFile() {
    cout << "========================================" << endl;
    cout << "   重新构建本底扣除文件" << endl;
    cout << "========================================" << endl;
    
    // 1. 检查所有输入文件
    cout << "\n1. 检查输入文件..." << endl;
    
    const char* requiredFiles[] = {
        "2025_146Sm98.root",
        "2025_146Sm99.root", 
        "2025_146Sm100.root",
        "2025_146Sm101.root",
        "2025_146Sm360.root"
    };
    
    for (int i = 0; i < 5; i++) {
        if (gSystem->AccessPathName(requiredFiles[i])) {
            cout << "错误：缺少文件 " << requiredFiles[i] << endl;
            return;
        }
        cout << "  找到: " << requiredFiles[i] << endl;
    }
    
    // 2. 创建输出文件
    cout << "\n2. 创建输出文件..." << endl;
    
    TString outfile = "2025_146Sm360_bgsub_FIXED.root";
    TFile* fout = new TFile(outfile, "RECREATE");
    if (!fout || fout->IsZombie()) {
        cout << "错误：无法创建输出文件" << endl;
        return;
    }
    
    // 3. 处理一个测试通道（先测试）
    cout << "\n3. 测试处理一个通道..." << endl;
    
    int test_adc = 0;
    int test_ch = 0;
    TString test_hist = Form("adc%dch%d", test_adc, test_ch);
    
    // 读取样品
    TFile* fSample = TFile::Open("2025_146Sm360.root");
    TH1I* hSample = (TH1I*)fSample->Get(test_hist);
    
    if (!hSample) {
        cout << "错误：测试通道 " << test_hist << " 不存在" << endl;
        fSample->Close();
        fout->Close();
        return;
    }
    
    cout << "  测试通道 " << test_hist << " 样品计数: " << hSample->Integral() << endl;
    
    // 读取本底
    TH1F* hBgSum = nullptr;
    int bgCount = 0;
    
    for (int run = 98; run <= 101; run++) {
        TString bgFile = Form("2025_146Sm%d.root", run);
        TFile* fBg = TFile::Open(bgFile);
        if (fBg) {
            TH1I* hBg = (TH1I*)fBg->Get(test_hist);
            if (hBg) {
                if (!hBgSum) {
                    hBgSum = new TH1F("hBgSum_test", "Background Sum",
                                     hBg->GetNbinsX(),
                                     hBg->GetXaxis()->GetXmin(),
                                     hBg->GetXaxis()->GetXmax());
                }
                hBgSum->Add(hBg);
                bgCount++;
            }
            fBg->Close();
        }
    }
    
    if (bgCount > 0) {
        hBgSum->Scale(1.0 / bgCount);
        cout << "  平均本底计数: " << hBgSum->Integral() << endl;
    }
    
    // 创建结果
    TH1F* hResult = new TH1F(test_hist, Form("ADC%d Ch%d (BG Subtracted)", test_adc, test_ch),
                            hSample->GetNbinsX(),
                            hSample->GetXaxis()->GetXmin(),
                            hSample->GetXaxis()->GetXmax());
    
    // 逐道扣除
    for (int bin = 1; bin <= hSample->GetNbinsX(); bin++) {
        double sample = hSample->GetBinContent(bin);
        double bg = (hBgSum) ? hBgSum->GetBinContent(bin) : 0;
        double result = sample - bg;
        if (result < 0) result = 0;  // 简单处理负值
        hResult->SetBinContent(bin, result);
    }
    
    cout << "  扣除本底后计数: " << hResult->Integral() << endl;
    
    // 测试写入
    fout->cd();
    hResult->Write();
    cout << "  测试写入成功" << endl;
    
    // 清理测试数据
    delete hResult;
    if (hBgSum) delete hBgSum;
    fSample->Close();
    
    // 4. 处理所有通道（分批处理，避免内存问题）
    cout << "\n4. 处理所有128个通道（分批处理）..." << endl;
    
    int processed = 0;
    int batch_size = 16; // 每批处理16个通道
    
    for (int adc = 0; adc < 4; adc++) {
        for (int ch_start = 0; ch_start < 32; ch_start += batch_size) {
            cout << "  处理 ADC" << adc << " 通道 " << ch_start << "-" 
                 << TMath::Min(ch_start + batch_size - 1, 31) << "..." << endl;
            
            for (int ch = ch_start; ch < ch_start + batch_size && ch < 32; ch++) {
                TString histName = Form("adc%dch%d", adc, ch);
                
                // 跳过已处理的测试通道
                if (adc == test_adc && ch == test_ch) {
                    processed++;
                    continue;
                }
                
                // 读取样品
                TFile* fSample2 = TFile::Open("2025_146Sm360.root");
                TH1I* hSample2 = (TH1I*)fSample2->Get(histName);
                
                if (!hSample2) {
                    fSample2->Close();
                    continue;
                }
                
                // 读取本底
                TH1F* hBgSum2 = nullptr;
                int bgCount2 = 0;
                
                for (int run = 98; run <= 101; run++) {
                    TString bgFile = Form("2025_146Sm%d.root", run);
                    TFile* fBg = TFile::Open(bgFile);
                    if (fBg) {
                        TH1I* hBg = (TH1I*)fBg->Get(histName);
                        if (hBg) {
                            if (!hBgSum2) {
                                hBgSum2 = new TH1F(Form("hBgSum_%s", histName.Data()), 
                                                 Form("Background %s", histName.Data()),
                                                 hBg->GetNbinsX(),
                                                 hBg->GetXaxis()->GetXmin(),
                                                 hBg->GetXaxis()->GetXmax());
                            }
                            hBgSum2->Add(hBg);
                            bgCount2++;
                        }
                        fBg->Close();
                    }
                }
                
                // 创建结果
                TH1F* hResult2 = new TH1F(histName, 
                                         Form("ADC%d Ch%d (BG Subtracted)", adc, ch),
                                         hSample2->GetNbinsX(),
                                         hSample2->GetXaxis()->GetXmin(),
                                         hSample2->GetXaxis()->GetXmax());
                
                // 本底扣除
                if (bgCount2 > 0) {
                    hBgSum2->Scale(1.0 / bgCount2);
                    
                    for (int bin = 1; bin <= hSample2->GetNbinsX(); bin++) {
                        double sample = hSample2->GetBinContent(bin);
                        double bg = hBgSum2->GetBinContent(bin);
                        double result = sample - bg;
                        if (result < 0) result = 0;
                        hResult2->SetBinContent(bin, result);
                    }
                    
                    delete hBgSum2;
                } else {
                    // 没有本底数据，直接复制
                    for (int bin = 1; bin <= hSample2->GetNbinsX(); bin++) {
                        hResult2->SetBinContent(bin, hSample2->GetBinContent(bin));
                    }
                }
                
                // 写入文件
                fout->cd();
                hResult2->Write();
                
                // 清理
                delete hResult2;
                fSample2->Close();
                
                processed++;
                
                // 进度显示
                if (processed % 16 == 0) {
                    cout << "    进度: " << processed << "/128 个通道" << endl;
                }
            }
            
            // 强制写入并刷新
            fout->Flush();
            cout << "    批次完成，已刷新到磁盘" << endl;
        }
    }
    
    // 5. 复制其他数据（Trees和Scaler）
    cout << "\n5. 复制其他数据..." << endl;
    
    TFile* fOrig = TFile::Open("2025_146Sm360.root");
    if (fOrig) {
        // 复制Trees
        for (int i = 0; i < 4; i++) {
            TString treeName = Form("Tree%d", i);
            TTree* tree = (TTree*)fOrig->Get(treeName);
            if (tree) {
                fout->cd();
                TTree* newTree = tree->CloneTree();
                newTree->Write();
                delete newTree;
                cout << "  复制: " << treeName << endl;
            }
        }
        
        // 复制Scaler
        TH1I* scaler = (TH1I*)fOrig->Get("scaler");
        if (scaler) {
            fout->cd();
            scaler->Write();
            cout << "  复制: scaler" << endl;
        }
        
        fOrig->Close();
    }
    
    // 6. 完成
    fout->Close();
    
    cout << "\n========================================" << endl;
    cout << "   重新构建完成！" << endl;
    cout << "   新文件: " << outfile << endl;
    cout << "   共处理 " << processed << " 个通道" << endl;
    cout << "========================================" << endl;
    
    // 7. 验证文件
    cout << "\n验证文件..." << endl;
    TFile* ftest = TFile::Open(outfile);
    if (ftest && !ftest->IsZombie()) {
        cout << "文件打开成功！" << endl;
        cout << "文件大小: " << ftest->GetSize() / 1024 / 1024 << " MB" << endl;
        cout << "文件内容:" << endl;
        ftest->ls();
        ftest->Close();
    } else {
        cout << "警告：文件验证失败" << endl;
    }
}
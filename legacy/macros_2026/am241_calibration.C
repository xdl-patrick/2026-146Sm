#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

void am241_calibration() {
    cout << "==========================================" << endl;
    cout << "  ²⁴¹Am α源硅探测器分析程序" << endl;
    cout << "==========================================" << endl << endl;

    // [1/6] 打开文件
    cout << "[1/6] 打开文件..." << endl;
    TFile *file = TFile::Open("/mnt/d/Sm146-2025/smadd.root");
    if (!file || file->IsZombie()) {
        cout << "错误: 无法打开文件!" << endl;
        return;
    }

    // 获取Trees
    TTree *Tree0 = (TTree*)file->Get("Tree0");
    TTree *Tree1 = (TTree*)file->Get("Tree1"); 
    TTree *Tree2 = (TTree*)file->Get("Tree2");
    TTree *Tree3 = (TTree*)file->Get("Tree3");

    if (Tree0) cout << "Tree0 事件数: " << Tree0->GetEntries() << endl;
    if (Tree1) cout << "Tree1 事件数: " << Tree1->GetEntries() << endl;
    if (Tree2) cout << "Tree2 事件数: " << Tree2->GetEntries() << endl;
    if (Tree3) cout << "Tree3 事件数: " << Tree3->GetEntries() << endl;

    // [2/6] 直接读取直方图数据并截取500-4000道
    cout << "[2/6] 读取直方图数据(500-4000道)..." << endl;
    
    const int nDetectors = 4;
    const int nChannels = 32;
    const int low_channel = 500;   // 起始道数
    const int high_channel = 4000; // 结束道数
    
    // 存储截取后的直方图
    TH1D *cropped_hists[nDetectors][nChannels];
    
    for (int det = 0; det < nDetectors; det++) {
        cout << "处理探测器 " << det << "..." << endl;
        
        for (int ch = 0; ch < nChannels; ch++) {
            TString histName = Form("adc%dch%d", det, ch);
            TH1I *original_hist = (TH1I*)file->Get(histName);
            
            if (!original_hist) {
                cout << "警告: 直方图 " << histName << " 不存在!" << endl;
                cropped_hists[det][ch] = nullptr;
                continue;
            }
            
            // 创建新的直方图，只包含500-4000道
            cropped_hists[det][ch] = new TH1D(Form("cropped_adc%dch%d", det, ch),
                                             Form("截取后的 adc%dch%d (%d-%d道)", det, ch, low_channel, high_channel),
                                             high_channel - low_channel + 1, low_channel, high_channel);
            
            // 复制500-4000道的数据
            for (int bin = low_channel; bin <= high_channel; bin++) {
                double content = original_hist->GetBinContent(bin);
                if (content > 0) {
                    cropped_hists[det][ch]->SetBinContent(bin - low_channel + 1, content);
                }
            }
            
            cout << Form("  通道 %d: 原始计数=%d, 截取后计数=%d", 
                        ch, (int)original_hist->GetEntries(), (int)cropped_hists[det][ch]->GetEntries()) << endl;
        }
    }

    // [3/6] 创建画布用于显示高斯拟合
    cout << "[3/6] 进行高斯拟合并显示图像(500-4000道)..." << endl;
    
    // 创建多个画布来显示拟合结果
    TCanvas *fitCanvases[nDetectors];
    for (int det = 0; det < nDetectors; det++) {
        fitCanvases[det] = new TCanvas(Form("fit_canvas_det%d", det), 
                                      Form("探测器%d 各通道高斯拟合(%d-%d道)", det, low_channel, high_channel), 
                                      1600, 1200);
        fitCanvases[det]->Divide(8, 4); // 8x4网格显示32个通道
    }

    double peak_positions[nDetectors][nChannels] = {0};
    double fit_means[nDetectors][nChannels] = {0};
    double fit_sigmas[nDetectors][nChannels] = {0};
    double fit_chi2[nDetectors][nChannels] = {0};
    
    for (int det = 0; det < nDetectors; det++) {
        cout << "拟合探测器 " << det << "..." << endl;
        
        for (int ch = 0; ch < nChannels; ch++) {
            if (!cropped_hists[det][ch]) continue;
            
            TH1D *hist = cropped_hists[det][ch];
            if (hist->GetEntries() < 100) continue;
            
            // 切换到对应的画布子pad
            fitCanvases[det]->cd(ch + 1);
            
            // 使用TSpectrum寻峰
            TSpectrum *s = new TSpectrum(10);
            int nFound = s->Search(hist, 2, "nodraw", 0.1);
            
            if (nFound > 0) {
                double *xpeaks = s->GetPositionX();
                double max_peak = 0;
                int main_peak_idx = -1;
                
                // 找到最高的峰
                for (int i = 0; i < nFound; i++) {
                    if (xpeaks[i] > max_peak) {
                        max_peak = xpeaks[i];
                        main_peak_idx = i;
                    }
                }
                
                if (main_peak_idx >= 0) {
                    peak_positions[det][ch] = xpeaks[main_peak_idx];
                    
                    // 进行高斯拟合
                    double peak_x = xpeaks[main_peak_idx];
                    double peak_y = hist->GetBinContent(hist->FindBin(peak_x));
                    
                    // 定义拟合范围（峰中心±3σ估计）
                    double fit_low = peak_x - 20;
                    double fit_high = peak_x + 20;
                    if (fit_low < hist->GetXaxis()->GetXmin()) fit_low = hist->GetXaxis()->GetXmin();
                    if (fit_high > hist->GetXaxis()->GetXmax()) fit_high = hist->GetXaxis()->GetXmax();
                    
                    // 创建高斯函数
                    TF1 *gaussFit = new TF1(Form("gauss_det%d_ch%d", det, ch), 
                                           "gaus", fit_low, fit_high);
                    gaussFit->SetParameters(peak_y, peak_x, 5.0); // 初始参数
                    
                    // 执行拟合
                    hist->Fit(gaussFit, "RQ+"); // R=范围, Q=安静模式, +=添加到图形
                    
                    // 获取拟合结果
                    fit_means[det][ch] = gaussFit->GetParameter(1);
                    fit_sigmas[det][ch] = gaussFit->GetParameter(2);
                    fit_chi2[det][ch] = gaussFit->GetChisquare() / gaussFit->GetNDF();
                    
                    // 绘制直方图和拟合曲线
                    hist->Draw();
                    gaussFit->Draw("same");
                    
                    // 添加文本框显示拟合结果
                    TPaveText *pt = new TPaveText(0.15, 0.7, 0.85, 0.85, "NDC");
                    pt->SetFillColor(0);
                    pt->SetBorderSize(1);
                    pt->AddText(Form("Det%d Ch%d (%d-%d道)", det, ch, low_channel, high_channel));
                    pt->AddText(Form("Mean: %.1f #pm %.1f", 
                                   gaussFit->GetParameter(1), gaussFit->GetParError(1)));
                    pt->AddText(Form("Sigma: %.1f", gaussFit->GetParameter(2)));
                    pt->AddText(Form("#chi^{2}/NDF: %.2f", 
                                   gaussFit->GetChisquare() / gaussFit->GetNDF()));
                    pt->Draw();
                    
                    cout << Form("  Det%d Ch%d: 峰位=%.1f, 拟合均值=%.1f, σ=%.1f", 
                                det, ch, peak_x, fit_means[det][ch], fit_sigmas[det][ch]) << endl;
                    
                    delete gaussFit;
                    delete pt;
                }
            }
            
            delete s;
        }
        
        // 更新画布
        fitCanvases[det]->Update();
    }

    // [6/6] 保存分析结果
    cout << "[6/6] 保存分析结果..." << endl;
    
    TFile *outfile = new TFile("calibration_results.root", "RECREATE");
    
    // 保存截取后的直方图
    for (int det = 0; det < nDetectors; det++) {
        for (int ch = 0; ch < nChannels; ch++) {
            if (cropped_hists[det][ch]) {
                cropped_hists[det][ch]->Write();
            }
        }
    }
        
    // 保存拟合画布
    for (int det = 0; det < nDetectors; det++) {
        fitCanvases[det]->Write();
    }
    
    // 保存拟合参数
    TH2D *fit_means_hist = new TH2D("fit_means", "高斯拟合均值", 
                                   nChannels, 0, nChannels, nDetectors, 0, nDetectors);
    TH2D *fit_sigmas_hist = new TH2D("fit_sigmas", "高斯拟合标准差", 
                                    nChannels, 0, nChannels, nDetectors, 0, nDetectors);
    TH2D *fit_chi2_hist = new TH2D("fit_chi2", "高斯拟合χ²/NDF", 
                                  nChannels, 0, nChannels, nDetectors, 0, nDetectors);
    
    for (int det = 0; det < nDetectors; det++) {
        for (int ch = 0; ch < nChannels; ch++) {
            fit_means_hist->SetBinContent(ch + 1, det + 1, fit_means[det][ch]);
            fit_sigmas_hist->SetBinContent(ch + 1, det + 1, fit_sigmas[det][ch]);
            fit_chi2_hist->SetBinContent(ch + 1, det + 1, fit_chi2[det][ch]);
        }
    }
    
    fit_means_hist->Write();
    fit_sigmas_hist->Write();
    fit_chi2_hist->Write();
    
    // 保存高斯拟合均值为指定格式
    ofstream mean_file("fit_means_output.txt");
    if (mean_file.is_open()) {
        mean_file << "高斯拟合均值输出格式:" << endl;
        mean_file << "==========================" << endl;
        
        for (int det = 0; det < nDetectors; det++) {
            mean_file << "探测器 " << det << ":" << endl;
            mean_file << "{";
            for (int ch = 0; ch < nChannels; ch++) {
                mean_file << fixed << setprecision(1) << fit_means[det][ch];
                if (ch < nChannels - 1) {
                    mean_file << ", ";
                }
                // 每行显示6个数据，便于阅读
                if ((ch + 1) % 6 == 0 && ch < nChannels - 1) {
                    mean_file << endl << "        ";
                }
            }
            mean_file << "}" << endl << endl;
        }
        
        // 同时保存为可直接复制的格式
        mean_file << "可直接复制的格式:" << endl;
        mean_file << "==========================" << endl;
        for (int det = 0; det < nDetectors; det++) {
            mean_file << "// 探测器 " << det << endl;
            mean_file << "{";
            for (int ch = 0; ch < nChannels; ch++) {
                mean_file << fixed << setprecision(1) << fit_means[det][ch] << ", {}";
                if (ch < nChannels - 1) {
                    mean_file << ", ";
                }
                // 每行显示4个数据对
                if ((ch + 1) % 4 == 0 && ch < nChannels - 1) {
                    mean_file << endl << "        ";
                }
            }
            mean_file << "}" << endl << endl;
        }
        
        mean_file.close();
    }
    
    outfile->Close();
    
    cout << endl << "=== 分析完成 ===" << endl;
    cout << "结果已保存到 calibration_results.root" << endl;
    cout << "高斯拟合均值已保存到 fit_means_output.txt" << endl;

    // [7/7] 保持程序运行
    cout << "[7/7] 程序完成，图像已显示..." << endl;
    cout << "按Ctrl+C退出或关闭窗口..." << endl;
    
    // 保持程序运行以查看图像
    gApplication->Run();
}

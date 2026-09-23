// SmEu_improved_calibration.C
// 改进的双峰刻度程序

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TLine.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

// 两个主要α峰能量 (keV)
const double sm_energy = 2280.0;  // ¹⁴⁷Sm 2.28 MeV
const double eu_energy = 2900.0;  // ¹⁴⁷Eu 2.90 MeV

// 改进的峰查找函数
vector<double> find_two_peaks_improved(TH1D* hist) {
    vector<double> peaks;
    
    // 平滑直方图以减少噪声
    TH1D* hist_smooth = (TH1D*)hist->Clone("hist_smooth");
    hist_smooth->Smooth(2);  // 轻度平滑
    
    // 寻找候选峰
    vector<pair<double, double>> candidate_peaks;
    double max_content = hist_smooth->GetMaximum();
    
    for (int bin = 5; bin <= hist_smooth->GetNbinsX() - 4; bin++) {
        double y0 = hist_smooth->GetBinContent(bin);
        
        // 检查是否是局部最大值（更宽松的条件）
        bool is_peak = true;
        for (int offset = 1; offset <= 4; offset++) {
            if (y0 <= hist_smooth->GetBinContent(bin - offset) || 
                y0 <= hist_smooth->GetBinContent(bin + offset)) {
                is_peak = false;
                break;
            }
        }
        
        // 降低阈值到最大值的2%
        if (is_peak && y0 > max_content * 0.02 && y0 > 5) {
            double bin_center = hist_smooth->GetBinCenter(bin);
            candidate_peaks.push_back(make_pair(bin_center, y0));
        }
    }
    
    // 按高度排序
    sort(candidate_peaks.begin(), candidate_peaks.end(), 
        [](const pair<double, double>& a, const pair<double, double>& b) {
            return a.second > b.second;
        });
    
    // 选择峰，放宽间距要求
    double min_peak_distance = 50.0;  // 降低最小峰间距
    for (const auto& candidate : candidate_peaks) {
        bool too_close = false;
        for (double existing_peak : peaks) {
            if (fabs(candidate.first - existing_peak) < min_peak_distance) {
                too_close = true;
                break;
            }
        }
        
        if (!too_close && peaks.size() < 2) {
            peaks.push_back(candidate.first);
        }
        
        if (peaks.size() >= 2) break;
    }
    
    // 如果找到的峰少于2个，尝试更宽松的条件
    if (peaks.size() < 2 && candidate_peaks.size() >= 2) {
        peaks.clear();
        for (int i = 0; i < min(2, (int)candidate_peaks.size()); i++) {
            peaks.push_back(candidate_peaks[i].first);
        }
    }
    
    // 按道数排序
    sort(peaks.begin(), peaks.end());
    
    delete hist_smooth;
    return peaks;
}

void SmEu_improved_calibration() {
    cout << "==========================================" << endl;
    cout << "  ¹⁴⁷Sm/¹⁴⁷Eu α源改进刻度程序" << endl;
    cout << "==========================================" << endl << endl;

    // 打开文件
    TFile *file = TFile::Open("/mnt/d/Sm146-2025/2025_146Sm120.root");
    if (!file || file->IsZombie()) {
        cout << "错误: 无法打开文件!" << endl;
        return;
    }

    TTree *tree = (TTree*)file->Get("Tree1");
    if (!tree) {
        cout << "错误: 无法找到 Tree1!" << endl;
        file->Close();
        return;
    }

    cout << "总事件数: " << tree->GetEntries() << endl;

    // 设置分支地址
    UInt_t bArray[32], cArray[32], dArray[32], eArray[32];
    for(int i=0; i<32; i++) {
        tree->SetBranchAddress(Form("adc0ch%d", i), &bArray[i]);
        tree->SetBranchAddress(Form("adc1ch%d", i), &cArray[i]);
        tree->SetBranchAddress(Form("adc2ch%d", i), &dArray[i]);
        tree->SetBranchAddress(Form("adc3ch%d", i), &eArray[i]);
    }

    // 创建直方图
    const int nDetectors = 4;
    const int nChannels = 32;
    const int hist_bins = 600;  // 进一步减少bin数以提高信噪比
    const double hist_min = 500;
    const double hist_max = 2500;
    
    TH1D *channel_spectra[nDetectors][nChannels];
    
    for (int det = 0; det < nDetectors; det++) {
        for (int ch = 0; ch < nChannels; ch++) {
            channel_spectra[det][ch] = new TH1D(
                Form("det%d_ch%d", det, ch),
                Form("Det%d Ch%d;Channel;Counts", det, ch),
                hist_bins, hist_min, hist_max
            );
        }
    }
    
    // 处理数据
    Long64_t nentries = tree->GetEntries();
    cout << "处理数据..." << endl;
    
    for(Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        
        for(int ch=0; ch<32; ch++) {
            if(bArray[ch] >= hist_min && bArray[ch] <= hist_max) 
                channel_spectra[0][ch]->Fill(bArray[ch]);
            if(cArray[ch] >= hist_min && cArray[ch] <= hist_max) 
                channel_spectra[1][ch]->Fill(cArray[ch]);
            if(dArray[ch] >= hist_min && dArray[ch] <= hist_max) 
                channel_spectra[2][ch]->Fill(dArray[ch]);
            if(eArray[ch] >= hist_min && eArray[ch] <= hist_max) 
                channel_spectra[3][ch]->Fill(eArray[ch]);
        }
    }

    // 改进的刻度过程
    cout << "开始刻度..." << endl;
    
    vector<int> calibrated_channels;
    vector<double> slopes, intercepts, sm_peaks, eu_peaks;
    
    TFile *output_file = new TFile("SmEu_improved_calibration_results.root", "RECREATE");
    
    int successful_calibrations = 0;
    
    for (int det = 0; det < nDetectors; det++) {
        for (int ch = 0; ch < nChannels; ch++) {
            TH1D *hist = channel_spectra[det][ch];
            
            // 降低数据量阈值
            if (hist->GetEntries() < 100) continue;
            
            vector<double> peaks = find_two_peaks_improved(hist);
            
            if (peaks.size() == 2) {
                double sm_peak = peaks[0];
                double eu_peak = peaks[1];
                
                // 计算刻度系数
                double a = (eu_energy - sm_energy) / (eu_peak - sm_peak);
                double b = sm_energy - a * sm_peak;
                
                // 放宽合理性检查
                if (a > 0.05 && a < 10.0) {
                    calibrated_channels.push_back(det * 100 + ch);
                    slopes.push_back(a);
                    intercepts.push_back(b);
                    sm_peaks.push_back(sm_peak);
                    eu_peaks.push_back(eu_peak);
                    successful_calibrations++;
                    
                    cout << Form("✓ Det%d Ch%d: E=%.3f*ch+%.1f", 
                               det, ch, a, b) << endl;
                    
                    hist->Write();
                }
            }
        }
    }
    
    cout << "总计完成 " << successful_calibrations << " 个通道的刻度" << endl;
    
    // 生成合并能谱
    if (successful_calibrations > 0) {
        TCanvas *energy_canvas = new TCanvas("energy_canvas", "Energy Spectra", 1600, 1200);
        energy_canvas->Divide(2, 2);
        
        for (int det = 0; det < nDetectors; det++) {
            energy_canvas->cd(det + 1);
            
            TH1D *energy_spectrum = new TH1D(
                Form("energy_det%d", det), Form("Detector %d;Energy (keV);Counts", det),
                400, 1500, 3500
            );
            
            int calibrated_in_det = 0;
            for (int ch = 0; ch < nChannels; ch++) {
                int channel_id = det * 100 + ch;
                auto it = find(calibrated_channels.begin(), calibrated_channels.end(), channel_id);
                
                if (it != calibrated_channels.end()) {
                    calibrated_in_det++;
                    int index = distance(calibrated_channels.begin(), it);
                    double a = slopes[index];
                    double b = intercepts[index];
                    
                    TH1D *hist = channel_spectra[det][ch];
                    for (int bin = 1; bin <= hist->GetNbinsX(); bin++) {
                        double counts = hist->GetBinContent(bin);
                        if (counts > 0) {
                            double energy = a * hist->GetBinCenter(bin) + b;
                            energy_spectrum->Fill(energy, counts);
                        }
                    }
                }
            }
            
            if (calibrated_in_det > 0) {
                energy_spectrum->Draw();
                
                TLine *line1 = new TLine(sm_energy, 0, sm_energy, energy_spectrum->GetMaximum() * 0.8);
                TLine *line2 = new TLine(eu_energy, 0, eu_energy, energy_spectrum->GetMaximum() * 0.8);
                line1->SetLineColor(kRed); line2->SetLineColor(kGreen);
                line1->SetLineStyle(2); line2->SetLineStyle(2);
                line1->Draw(); line2->Draw();
                
                cout << Form("探测器 %d: %d 个通道", det, calibrated_in_det) << endl;
                energy_spectrum->Write();
            }
        }
        
        energy_canvas->SaveAs("improved_energy_spectra.png");
        delete energy_canvas;
    }
    
    // 保存结果
    ofstream calib_file("improved_calibration_parameters.txt");
    calib_file << "改进刻度结果 - 使用更宽松的峰检测条件" << endl;
    calib_file << "总刻度通道数: " << successful_calibrations << endl << endl;
    
    calib_file << "通道ID\t探测器\t通道\t斜率a\t截距b\tSm峰\tEu峰" << endl;
    for (size_t i = 0; i < calibrated_channels.size(); i++) {
        int det = calibrated_channels[i] / 100;
        int ch = calibrated_channels[i] % 100;
        calib_file << Form("%d\t%d\t%d\t%.6f\t%.6f\t%.1f\t%.1f", 
                          calibrated_channels[i], det, ch, 
                          slopes[i], intercepts[i], sm_peaks[i], eu_peaks[i]) << endl;
    }
    calib_file.close();
    
    output_file->Close();
    file->Close();
    
    cout << "改进刻度完成!" << endl;
}

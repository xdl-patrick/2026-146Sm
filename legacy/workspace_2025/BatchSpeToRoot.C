#include <fstream>
#include "TH1D.h"
#include "TFile.h"
#include "TSpectrum.h"
#include "TAxis.h"
#include <iostream>
#include <vector>
#include <string>

void BatchSpeToRoot()
{
    // 定义要处理的文件列表
    std::vector<std::string> speFiles = {
        "1#-1.Spe"
        "2#-1.Spe"
        "4#-1.Spe"
        "6#-1.Spe"
        "10.18_1#with Pb.Spe",
        "10.18_2#with Pb.Spe", 
        "10.18_3#with Pb.Spe",
        "10.18_4#with Pb.Spe",
        "10.19_1#with Pb.Spe",
        "10.19_2#with Pb.Spe",
        "10.19_3#with Pb.Spe", 
        "10.19_4#with Pb.Spe",
        "10.20_1#with Pb.Spe",
        "10.20_2#with Pb.Spe",
        "10.20_3#with Pb.Spe",
        "10.20_4#with Pb.Spe",
        "10.21_1#with Pb.Spe",
        "10.21_2#with Pb.Spe",
        "10.21_3#with Pb.Spe",
        "10.21_4#with Pb.Spe",
        "10.22_1#with Pb.Spe",
        "10.22_2#with Pb.Spe",
        "10.22_3#with Pb.Spe",
        "10.22_4#with Pb.Spe",
        "10.23_1#with Pb.Spe",
        "10.23_2#with Pb.Spe",
        "10.23_3#with Pb.Spe",
        "10.23_4#with Pb.Spe",

    };
    
    // 能量刻度系数
    double a = 0.200927085;
    double b = -3.964978762;
    
    // 逐个处理每个文件
    for (const auto& speFile : speFiles) {
        
        // 检查SPE文件是否存在
        std::ifstream file(speFile);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << speFile << std::endl;
            continue;  // 跳过不存在的文件，继续处理下一个
        }
        
        std::cout << "Processing: " << speFile << std::endl;
        
        // 生成对应的ROOT文件名（将扩展名.spe改为.root）
        std::string rootFile = speFile;
        size_t pos = rootFile.find(".Spe");
        if (pos != std::string::npos) {
            rootFile.replace(pos, 4, ".root");
        } else {
            rootFile += ".root";
        }
        
        // 读取SPE文件数据
        const int nChannels = 16384;
        TH1D* hist = new TH1D("spectrum", "SPE Spectrum;Channel;Counts", nChannels, 0, nChannels);
        
        std::string line;
        // 跳过文件头（12行）
        for (int i = 0; i < 12; i++) {
            std::getline(file, line);
        }
        
        // 读取数据
        int channel = 0, count;
        while (file >> count && channel < nChannels) {
            hist->SetBinContent(channel + 1, count);
            channel++;
        }
        file.close();
        
        // 创建能量刻度后的直方图
        TH1D* calibratedHist = new TH1D("h", 
                                       "Energy h;Energy (keV);Counts", 
                                       nChannels, b, a * nChannels + b);
        
        // 复制数据到刻度后的直方图
        for (int i = 1; i <= nChannels; i++) {
            double counts = hist->GetBinContent(i);
            calibratedHist->SetBinContent(i, counts);
        }
        
        // 保存到ROOT文件
        TFile out(rootFile.c_str(), "RECREATE");
        hist->Write();           // 保存原始数据
        calibratedHist->Write(); // 保存刻度后的数据
        out.Close();
        
        // 清理内存
        delete hist;
        delete calibratedHist;
        
        std::cout << "Successfully converted: " << speFile << " -> " << rootFile << std::endl;
        std::cout << "Calibration coefficients: a = " << a << ", b = " << b << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
    
    std::cout << "Batch processing completed!" << std::endl;
    std::cout << "Total files processed: " << speFiles.size() << std::endl;
}

// 可以在ROOT中直接运行的函数
void runBatch() {
    BatchSpeToRoot();
}


#include <fstream>
#include "TH1D.h"
#include "TFile.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TAxis.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void BatchSpeToRoot2024()
{
    // 能量刻度系数
    double a = 0.200927085;
    double b = -3.964978762;
    
    // 定义根目录路径
    std::string basePath = "/mnt/d/Sm146-2025/Sm147-2025";
    
    // 获取目录下的所有文件
    TSystemDirectory dir(basePath.c_str(), basePath.c_str());
    TList* fileList = dir.GetListOfFiles();
    
    if (!fileList) {
        std::cerr << "Error: Cannot access directory: " << basePath << std::endl;
        return;
    }
    
    std::vector<std::string> speFiles;
    
    // 遍历目录，收集所有.spe文件
    TSystemFile* file;
    TIter next(fileList);
    while ((file = (TSystemFile*)next())) {
        std::string fileName = file->GetName();
        
        // 跳过当前目录和上级目录链接
        if (fileName == "." || fileName == "..") continue;
        
        // 检查是否为.Spe文件（不区分大小写）
        std::string lowerName = fileName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find(".spe") != std::string::npos) {
            std::string fullSpePath = basePath + "/" + fileName;
            speFiles.push_back(fullSpePath);
            std::cout << "Found SPE file: " << fileName << std::endl;
        }
    }
    
    std::cout << "Total SPE files found: " << speFiles.size() << std::endl;
    std::cout << "========================================" << std::endl;
    
    int totalProcessed = 0;
    
    // 处理所有.spe文件
    for (const auto& speFilePath : speFiles) {
        // 提取文件名（不含路径）
        std::string speFileName = speFilePath.substr(speFilePath.find_last_of("/") + 1);
        
        std::cout << "Converting: " << speFileName << std::endl;
        
        // 检查SPE文件是否存在
        std::ifstream file(speFilePath);
        if (!file.is_open()) {
            std::cerr << "  Error opening file: " << speFilePath << std::endl;
            continue;
        }
        
        // 生成对应的ROOT文件名 - 强制添加.root后缀
        std::string rootFileName;
        
        // 方法1：查找并替换.spe后缀（不区分大小写）
        std::string lowerSpeName = speFileName;
        std::transform(lowerSpeName.begin(), lowerSpeName.end(), lowerSpeName.begin(), ::tolower);
        
        size_t pos = lowerSpeName.find(".spe");
        if (pos != std::string::npos) {
            // 找到.spe后缀，替换为.root（保持原文件名的大小写）
            rootFileName = speFileName.substr(0, pos) + ".root";
        } else {
            // 没有找到.spe后缀，直接添加.root
            rootFileName = speFileName + ".root";
        }
        
        // 额外的安全检查：确保文件名以.root结尾
        if (rootFileName.length() < 5 || rootFileName.substr(rootFileName.length() - 5) != ".root") {
            rootFileName += ".root";
        }
        
        std::string rootFilePath = basePath + "/" + rootFileName;
        
        // 调试输出
        std::cout << "  Input file: " << speFileName << std::endl;
        std::cout << "  Output file: " << rootFileName << std::endl;
        
        // 读取SPE文件数据
        const int nChannels = 16384;
        TH1D* hist = new TH1D("spectrum", "SPE Spectrum;Channel;Counts", nChannels, 0, nChannels);
        
        std::string line;
        // 跳过文件头（12行）
        for (int i = 0; i < 12; i++) {
            if (!std::getline(file, line)) {
                std::cerr << "  Error reading header of file: " << speFilePath << std::endl;
                file.close();
                delete hist;
                continue;
            }
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
                                       "Energy Spectrum;Energy (keV);Counts", 
                                       nChannels, b, a * nChannels + b);
        
        // 复制数据到刻度后的直方图
        for (int i = 1; i <= nChannels; i++) {
            double counts = hist->GetBinContent(i);
            calibratedHist->SetBinContent(i, counts);
        }
        
        // 保存到ROOT文件
        TFile out(rootFilePath.c_str(), "RECREATE");
        if (out.IsZombie()) {
            std::cerr << "  Error creating ROOT file: " << rootFilePath << std::endl;
            delete hist;
            delete calibratedHist;
            continue;
        }
        
        hist->Write();           // 保存原始数据
        calibratedHist->Write(); // 保存刻度后的数据
        out.Close();
        
        // 清理内存
        delete hist;
        delete calibratedHist;
        
        std::cout << "  Successfully converted: " << speFileName << " -> " << rootFileName << std::endl;
        totalProcessed++;
        
        std::cout << "  ----------------------------------------" << std::endl;
    }
    
    delete fileList;
    
    std::cout << "========================================" << std::endl;
    std::cout << "Batch processing completed!" << std::endl;
    std::cout << "Total SPE files converted: " << totalProcessed << std::endl;
}

// 可以在ROOT中直接运行的函数
void runBatch() {
    BatchSpeToRoot2024();
}
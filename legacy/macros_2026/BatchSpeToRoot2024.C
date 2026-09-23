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
    double a = 0.194615634;
    double b = -3.723970159;
    
    // 定义根目录路径
    std::string basePath = "/mnt/d/Sm146-2025/gama-2024";
    
    // 获取根目录下的所有文件夹
    TSystemDirectory dir(basePath.c_str(), basePath.c_str());
    TList* fileList = dir.GetListOfFiles();
    
    if (!fileList) {
        std::cerr << "Error: Cannot access directory: " << basePath << std::endl;
        return;
    }
    
    std::vector<std::string> folders;
    
    // 遍历目录，收集所有文件夹
    TSystemFile* file;
    TIter next(fileList);
    while ((file = (TSystemFile*)next())) {
        std::string fileName = file->GetName();
        
        // 跳过当前目录和上级目录链接
        if (fileName == "." || fileName == "..") continue;
        
        // 检查是否为目录
        std::string fullPath = basePath + "/" + fileName;
        if (file->IsDirectory()) {
            folders.push_back(fileName);
            std::cout << "Found folder: " << fileName << std::endl;
        }
    }
    
    std::cout << "Total folders found: " << folders.size() << std::endl;
    std::cout << "========================================" << std::endl;
    
    int totalProcessed = 0;
    
    // 逐个处理每个文件夹
    for (const auto& folder : folders) {
        std::string folderPath = basePath + "/" + folder;
        
        std::cout << "Processing folder: " << folder << std::endl;
        
        // 获取该文件夹中的所有.Spe文件
        TSystemDirectory subDir(folder.c_str(), folderPath.c_str());
        TList* speFileList = subDir.GetListOfFiles();
        
        if (!speFileList) {
            std::cerr << "Warning: Cannot access folder: " << folderPath << std::endl;
            continue;
        }
        
        std::vector<std::string> speFiles;
        
        TSystemFile* speFile;
        TIter speNext(speFileList);
        while ((speFile = (TSystemFile*)speNext())) {
            std::string speFileName = speFile->GetName();
            
            // 检查是否为.Spe文件（不区分大小写）
            std::string lowerName = speFileName;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            
            if (lowerName.find(".spe") != std::string::npos) {
                std::string fullSpePath = folderPath + "/" + speFileName;
                speFiles.push_back(fullSpePath);
                std::cout << "  Found SPE file: " << speFileName << std::endl;
            }
        }
        
        // 处理该文件夹中的所有.Spe文件
        for (const auto& speFilePath : speFiles) {
            // 提取文件名（不含路径）
            std::string speFileName = speFilePath.substr(speFilePath.find_last_of("/") + 1);
            
            std::cout << "  Converting: " << speFileName << std::endl;
            
            // 检查SPE文件是否存在
            std::ifstream file(speFilePath);
            if (!file.is_open()) {
                std::cerr << "  Error opening file: " << speFilePath << std::endl;
                continue;
            }
            
            // 生成对应的ROOT文件名（将扩展名.spe改为.root）
            std::string rootFileName = speFileName;
            size_t pos = rootFileName.find(".Spe");
            if (pos != std::string::npos) {
                rootFileName.replace(pos, 4, ".root");
            } else {
                pos = rootFileName.find(".spe");
                if (pos != std::string::npos) {
                    rootFileName.replace(pos, 4, ".root");
                } else {
                    rootFileName += ".root";
                }
            }
            
            std::string rootFilePath = folderPath + "/" + rootFileName;
            
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
            
            // 保存到ROOT文件（保存在原文件夹中）
            TFile out(rootFilePath.c_str(), "RECREATE");
            hist->Write();           // 保存原始数据
            calibratedHist->Write(); // 保存刻度后的数据
            out.Close();
            
            // 清理内存
            delete hist;
            delete calibratedHist;
            
            std::cout << "  Successfully converted: " << speFileName << " -> " << rootFileName << std::endl;
            totalProcessed++;
        }
        
        std::cout << "  Finished processing folder: " << folder << std::endl;
        std::cout << "  ----------------------------------------" << std::endl;
        
        delete speFileList;
    }
    
    delete fileList;
    
    std::cout << "========================================" << std::endl;
    std::cout << "Batch processing completed!" << std::endl;
    std::cout << "Total folders processed: " << folders.size() << std::endl;
    std::cout << "Total SPE files converted: " << totalProcessed << std::endl;
}

// 可以在ROOT中直接运行的函数
void runBatch() {
    BatchSpeToRoot2024();
}

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <sys/stat.h>  // 用于获取文件修改时间
#include "TFile.h"
#include "TH1D.h"
#include "TMath.h"
#include "TSystem.h"
#include "TList.h"
#include "TSystemDirectory.h"

// 获取文件的最后修改时间
time_t getFileModificationTime(const std::string& filePath) {
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0) {
        return fileStat.st_mtime;
    }
    return 0;
}

// 从时间戳转换为可读的日期时间字符串
std::string timeToString(time_t timestamp) {
    if (timestamp == 0) return "Unknown";
    struct tm* timeinfo = localtime(&timestamp);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

// 计算两个时间戳之间的小时数差（time2 - time1）
double calculateHoursDifference(time_t time1, time_t time2) {
    return difftime(time2, time1) / 3600.0;  // 不使用绝对值
}

// 从SPE文件头中提取精确测量时间和死时间
bool extractTimeAndDeadTimeFromSPE(const std::string& filePath, 
                                  std::string& dateTimeStr, 
                                  double& liveTime, 
                                  double& realTime,
                                  double& deadTime) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    
    std::string line;
    std::string dateLine, measTimeLine;
    
    // 读取文件头寻找时间信息
    while (std::getline(file, line)) {
        if (line.find("$DATE_MEA:") != std::string::npos) {
            // 下一行是日期时间
            if (std::getline(file, dateLine)) {
                dateTimeStr = dateLine;
            }
        }
        else if (line.find("$MEAS_TIM:") != std::string::npos) {
            // 下一行是测量时间
            if (std::getline(file, measTimeLine)) {
                // 测量时间格式: "582 613" (活时间 实时间)
                std::istringstream iss(measTimeLine);
                if (iss >> liveTime >> realTime) {
                    // 计算死时间比例
                    if (realTime > 0) {
                        deadTime = (realTime - liveTime) / realTime * 100.0; // 死时间百分比
                    }
                }
            }
        }
        else if (line.find("$DATA:") != std::string::npos) {
            break;
        }
    }
    
    file.close();
    return !dateTimeStr.empty();
}

// 分析直方图中的747 keV峰（745-749范围）
bool analyzePeak747(TH1D* hist, double& netCounts, double& uncertainty) {
    const double LOW_LIMIT = 745.0;   // 下限
    const double HIGH_LIMIT = 749.0;  // 上限
    
    int binStart = hist->FindBin(LOW_LIMIT);
    int binEnd = hist->FindBin(HIGH_LIMIT);
    
    if (binStart >= binEnd) {
        std::cerr << "Warning: Invalid bin range for 747 keV peak" << std::endl;
        return false;
    }
    
    // 估计本底区域 - 使用峰两侧的区域
    double bgWidth = 4.0; // 背景区域宽度（keV）
    
    // 左侧背景区域
    int leftBgStart = hist->FindBin(LOW_LIMIT - bgWidth - 2.0);
    int leftBgEnd = hist->FindBin(LOW_LIMIT - 2.0);
    
    // 右侧背景区域
    int rightBgStart = hist->FindBin(HIGH_LIMIT + 2.0);
    int rightBgEnd = hist->FindBin(HIGH_LIMIT + bgWidth + 2.0);
    
    // 确保边界有效
    leftBgStart = std::max(1, leftBgStart);
    leftBgEnd = std::max(1, leftBgEnd);
    rightBgStart = std::min(hist->GetNbinsX(), rightBgStart);
    rightBgEnd = std::min(hist->GetNbinsX(), rightBgEnd);
    
    // 计算本底
    double leftBg = 0, rightBg = 0;
    int leftCount = 0, rightCount = 0;
    
    if (leftBgEnd > leftBgStart) {
        for (int i = leftBgStart; i <= leftBgEnd; i++) {
            leftBg += hist->GetBinContent(i);
            leftCount++;
        }
        if (leftCount > 0) leftBg /= leftCount;
    }
    
    if (rightBgEnd > rightBgStart) {
        for (int i = rightBgStart; i <= rightBgEnd; i++) {
            rightBg += hist->GetBinContent(i);
            rightCount++;
        }
        if (rightCount > 0) rightBg /= rightCount;
    }
    
    // 使用线性本底估计
    double averageBg = (leftBg + rightBg) / 2.0;
    
    // 计算峰区域总计数
    double totalCounts = 0;
    for (int i = binStart; i <= binEnd; i++) {
        totalCounts += hist->GetBinContent(i);
    }
    
    // 计算本底计数
    double bgCounts = averageBg * (binEnd - binStart + 1);
    netCounts = totalCounts - bgCounts;
    
    // 计算统计不确定度
    uncertainty = TMath::Sqrt(totalCounts + bgCounts);
    
    // 检查结果有效性
    if (netCounts <= 0 || TMath::IsNaN(netCounts) || TMath::IsNaN(uncertainty)) {
        return false;
    }
    
    return true;
}

// 分析直方图中的633/634复合峰（631-636范围）
bool analyzePeak633_634(TH1D* hist, double& netCounts, double& uncertainty) {
    const double LOW_LIMIT = 631.0;   // 下限
    const double HIGH_LIMIT = 636.0;  // 上限
    
    int binStart = hist->FindBin(LOW_LIMIT);
    int binEnd = hist->FindBin(HIGH_LIMIT);
    
    if (binStart >= binEnd) {
        std::cerr << "Warning: Invalid bin range for 633/634 keV peak" << std::endl;
        return false;
    }
    
    // 估计本底区域 - 使用峰两侧的区域
    double bgWidth = 5.0; // 背景区域宽度（keV）
    
    // 左侧背景区域
    int leftBgStart = hist->FindBin(LOW_LIMIT - bgWidth - 2.0);
    int leftBgEnd = hist->FindBin(LOW_LIMIT - 2.0);
    
    // 右侧背景区域
    int rightBgStart = hist->FindBin(HIGH_LIMIT + 2.0);
    int rightBgEnd = hist->FindBin(HIGH_LIMIT + bgWidth + 2.0);
    
    // 确保边界有效
    leftBgStart = std::max(1, leftBgStart);
    leftBgEnd = std::max(1, leftBgEnd);
    rightBgStart = std::min(hist->GetNbinsX(), rightBgStart);
    rightBgEnd = std::min(hist->GetNbinsX(), rightBgEnd);
    
    // 计算本底
    double leftBg = 0, rightBg = 0;
    int leftCount = 0, rightCount = 0;
    
    if (leftBgEnd > leftBgStart) {
        for (int i = leftBgStart; i <= leftBgEnd; i++) {
            leftBg += hist->GetBinContent(i);
            leftCount++;
        }
        if (leftCount > 0) leftBg /= leftCount;
    }
    
    if (rightBgEnd > rightBgStart) {
        for (int i = rightBgStart; i <= rightBgEnd; i++) {
            rightBg += hist->GetBinContent(i);
            rightCount++;
        }
        if (rightCount > 0) rightBg /= rightCount;
    }
    
    // 使用线性本底估计
    double averageBg = (leftBg + rightBg) / 2.0;
    
    // 计算峰区域总计数
    double totalCounts = 0;
    for (int i = binStart; i <= binEnd; i++) {
        totalCounts += hist->GetBinContent(i);
    }
    
    // 计算本底计数
    double bgCounts = averageBg * (binEnd - binStart + 1);
    netCounts = totalCounts - bgCounts;
    
    // 计算统计不确定度
    uncertainty = TMath::Sqrt(totalCounts + bgCounts);
    
    // 检查结果有效性
    if (netCounts <= 0 || TMath::IsNaN(netCounts) || TMath::IsNaN(uncertainty)) {
        return false;
    }
    
    return true;
}

// 从ROOT文件读取直方图并分析峰，返回计数和误差
bool analyzeROOTFile(const std::string& rootFilePath, 
                    double& rate747, double& error747,
                    double& rate633_634, double& error633_634) {
    
    TFile* rootFile = TFile::Open(rootFilePath.c_str(), "READ");
    if (!rootFile || rootFile->IsZombie()) {
        std::cerr << "Error: Cannot open ROOT file: " << rootFilePath << std::endl;
        return false;
    }
    
    // 获取刻度后的直方图
    TH1D* hist = (TH1D*)rootFile->Get("h");
    if (!hist) {
        std::cerr << "Error: Histogram 'h' not found in " << rootFilePath << std::endl;
        rootFile->Close();
        return false;
    }
    
    // 分析747 keV峰
    double netCounts747, countsError747;
    bool success747 = analyzePeak747(hist, netCounts747, countsError747);
    
    // 分析633/634复合峰
    double netCounts633_634, countsError633_634;
    bool success633_634 = analyzePeak633_634(hist, netCounts633_634, countsError633_634);
    
    rootFile->Close();
    delete rootFile;
    
    // 设置计数和误差
    rate747 = netCounts747;
    error747 = countsError747;
    rate633_634 = netCounts633_634;
    error633_634 = countsError633_634;
    
    return (success747 && success633_634);
}

// 解析日期字符串并转换为时间戳
time_t parseDateString(const std::string& dateStr) {
    struct tm timeinfo = {0};
    
    // 假设日期格式为 "MM.DD_HH:MM:SS" 或类似格式
    // 首先尝试解析文件名中的日期
    if (dateStr.length() >= 5) {
        // 提取月份和日期
        size_t dotPos = dateStr.find('.');
        if (dotPos != std::string::npos) {
            std::string monthStr = dateStr.substr(0, dotPos);
            std::string dayStr = dateStr.substr(dotPos + 1, 2);
            
            try {
                timeinfo.tm_mon = std::stoi(monthStr) - 1;  // tm_mon 是 0-11
                timeinfo.tm_mday = std::stoi(dayStr);
                timeinfo.tm_year = 125;  // 2025 - 1900 = 125
                
                // 尝试从文件名提取时间
                size_t underscorePos = dateStr.find('_');
                if (underscorePos != std::string::npos) {
                    std::string timePart = dateStr.substr(underscorePos + 1);
                    // 简化处理，假设时间为09:00:00
                    timeinfo.tm_hour = 9;
                } else {
                    timeinfo.tm_hour = 0;
                }
                
                timeinfo.tm_min = 0;
                timeinfo.tm_sec = 0;
                
                return mktime(&timeinfo);
            } catch (...) {
                return 0;
            }
        }
    }
    return 0;
}

// 计算HPGe探测器死时间测量误差（百分比）
// 对于高纯锗探测器，死时间误差通常较小
double calculateHPGeDeadTimeError(double deadTimePercent) {
    // HPGe探测器死时间误差估计：
    // 1. 低死时间(<5%): 误差较小，约0.1-0.2%
    // 2. 中死时间(5-15%): 误差适中，约0.2-0.3%
    // 3. 高死时间(>15%): 误差较大，约0.3-0.5%
    
    if (deadTimePercent < 0) {
        return 0.2; // 默认值
    }
    else if (deadTimePercent < 5.0) {
        return 0.15; // 低死时间范围：0.15%误差
    }
    else if (deadTimePercent < 15.0) {
        return 0.25; // 中死时间范围：0.25%误差
    }
    else {
        return 0.4; // 高死时间范围：0.4%误差
    }
}

// 计算死时间修正后的计数率及其总误差
void calculateCorrectedRate(double measuredRate, double measuredRateError,
                          double deadTimePercent, double deadTimeErrorPercent,
                          double& correctedRate, double& correctedRateError) {
    
    // 从强源干扰法得到的修正参数
    const double P0 = 102.5;        // 零死时间时的计数率 (counts/sec)
    const double P1 = -16.07;       // 斜率
    const double P0_ERR = 0.3899;   // 截距误差
    const double P1_ERR = 5.231;    // 斜率误差
    
    double deadTimeFraction = deadTimePercent / 100.0;
    double deadTimeFractionErr = deadTimeErrorPercent / 100.0;
    
    // 计算修正因子
    // 从修正公式：测量计数率 = P0 + P1 × DT
    // 在零死时间时，真实计数率 = P0
    // 在死时间DT时，测量计数率 = P0 + P1 × DT
    
    // 计算相对损失
    double measuredRateAtDT = P0 + P1 * deadTimeFraction;
    double relativeLoss = (P0 - measuredRateAtDT) / P0;
    
    // 修正因子 = 1 / (1 - 相对损失)
    double correctionFactor = 1.0 / (1.0 - relativeLoss);
    
    // 修正后的计数率
    correctedRate = measuredRate * correctionFactor;
    
    // 计算误差分量
    // 1. 统计误差（按比例传递）
    double statError = measuredRateError * correctionFactor;
    
    // 2. 修正因子误差（来自参数误差和死时间误差）
    // 修正因子 C = P0 / (P0 + P1 × DT)
    
    double denominator = P0 + P1 * deadTimeFraction;
    
    // 计算相对误差
    double relErrP0 = P0_ERR / P0;
    double relErrP1Term = (deadTimeFraction * P1_ERR) / denominator;
    double relErrDTTerm = (P1 * deadTimeFractionErr) / denominator;
    
    // 修正因子的相对误差
    double correctionFactorRelErr = sqrt(relErrP0*relErrP0 + relErrP1Term*relErrP1Term + relErrDTTerm*relErrDTTerm);
    
    // 3. 总误差 = sqrt(统计误差² + (修正率×修正因子相对误差)²)
    correctedRateError = sqrt(statError*statError + 
                            (correctedRate * correctionFactorRelErr) * 
                            (correctedRate * correctionFactorRelErr));
}

void AnalyzeSPEandROOTFiles() {
    // 基础路径
    std::string basePath = "/mnt/d/Sm146-2025/Sm147-2025";
    
    // 获取目录下的所有SPE文件
    std::vector<std::string> speFiles;
    TSystemDirectory dir(basePath.c_str(), basePath.c_str());
    TList* fileList = dir.GetListOfFiles();
    
    if (!fileList) {
        std::cerr << "Error: Cannot access directory: " << basePath << std::endl;
        return;
    }
    
    // 收集符合条件的SPE文件
    TSystemFile* file;
    TIter next(fileList);
    while ((file = (TSystemFile*)next())) {
        std::string fileName = file->GetName();
        
        if (fileName == "." || fileName == "..") continue;
        
        std::string lowerName = fileName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find(".spe") != std::string::npos) {
            if (fileName.length() >= 5) {
                std::string datePrefix = fileName.substr(0, 5);
                // 根据实际文件日期范围修改
                if (datePrefix >= "11.24" && datePrefix <= "12.20") {
                    std::string fullPath = basePath + "/" + fileName;
                    speFiles.push_back(fullPath);
                }
            }
        }
    }
    
    // 按文件名排序
    std::sort(speFiles.begin(), speFiles.end());
    
    std::cout << "Found " << speFiles.size() << " SPE files from 10.18 to 11.17" << std::endl;
    std::cout << "========================================================================================" << std::endl;
    
    // 输出表头 - 按照要求格式
    std::cout << std::left << std::setw(25) << "Filename" 
              << std::setw(12) << "DeadTime(%)"
              << std::setw(15) << "DeadTimeErr(%)"
              << std::setw(20) << "747keV Rate±Err"
              << std::setw(20) << "747keV Corr±Err"
              << std::setw(20) << "633/634 Rate±Err"
              << std::setw(20) << "633/634 Corr±Err"
              << std::endl;
    std::cout << std::string(140, '-') << std::endl;
    
    // 从强源干扰法得到的修正参数
    const double P0 = 102.5;        // counts/sec at zero dead time
    const double P1 = -16.07;       // slope
    const double P0_ERR = 0.3899;   // intercept error
    const double P1_ERR = 5.231;    // slope error
    
    std::cout << "Dead time correction from Cs-137/Co-60 two-source method" << std::endl;
    std::cout << "Correction formula: Measured Rate = " << P0 << " + (" << P1 << ") × DeadTime(%)" << std::endl;
    std::cout << "Valid for dead time range: 0% to ~14%" << std::endl;
    std::cout << "HPGe detector dead time measurement error estimated based on dead time level" << std::endl;
    std::cout << std::endl;
    
    int analyzedCount = 0;
    time_t referenceTime = 0; // 10.16下午3点的时间
    bool referenceSet = false;
    
    // 设置基准时间：10.16 15:00:00 (下午3点)
    struct tm referenceTM = {0};
    referenceTM.tm_mon = 9;   // 10月 = 10 - 1
    referenceTM.tm_mday = 16; // 16日
    referenceTM.tm_year = 125; // 2025 - 1900 = 125
    referenceTM.tm_hour = 15;  // 下午3点
    referenceTM.tm_min = 0;
    referenceTM.tm_sec = 0;
    
    referenceTime = mktime(&referenceTM);
    referenceSet = true;
    
    // 分析每个SPE文件及其对应的ROOT文件
    for (size_t i = 0; i < speFiles.size(); i++) {
        std::string speFilePath = speFiles[i];
        std::string speFileName = speFilePath.substr(speFilePath.find_last_of("/") + 1);
        
        // 从SPE文件提取时间和死时间信息
        std::string dateTimeStr;
        double liveTime = 0.0, realTime = 0.0, deadTimePercent = 0.0;
        bool timeExtracted = extractTimeAndDeadTimeFromSPE(speFilePath, dateTimeStr, 
                                                          liveTime, realTime, deadTimePercent);
        
        // 如果没有提取到时间，使用默认值
        if (!timeExtracted) {
            liveTime = 100.0; // 默认值
            deadTimePercent = 0.0;
        }
        
        // 检查死时间是否在有效范围内
        if (deadTimePercent < 0 || deadTimePercent > 20) {
            std::cerr << "Warning: Dead time " << deadTimePercent 
                      << "% outside expected range (0-14%) for " 
                      << speFileName << std::endl;
        }
        
        // 计算HPGe探测器死时间测量误差
        double deadTimeErrorPercent = calculateHPGeDeadTimeError(deadTimePercent);
        
        // 构建对应的ROOT文件名
        std::string rootFileName = speFileName;
        size_t pos = rootFileName.find(".spe");
        if (pos != std::string::npos) {
            rootFileName.replace(pos, 4, ".root");
        } else {
            pos = rootFileName.find(".Spe");
            if (pos != std::string::npos) {
                rootFileName.replace(pos, 4, ".root");
            } else {
                pos = rootFileName.find(".SPE");
                if (pos != std::string::npos) {
                    rootFileName.replace(pos, 4, ".root");
                } else {
                    continue; // 无法确定ROOT文件名，跳过
                }
            }
        }
        
        // 只显示文件名，不显示完整路径
        std::string displayFileName = rootFileName;
        
        // 截断过长的文件名，确保对齐
        if (displayFileName.length() > 22) {
            displayFileName = displayFileName.substr(0, 19) + "...";
        }
        
        std::string rootFilePath = basePath + "/" + rootFileName;
        
        // 检查ROOT文件是否存在
        if (gSystem->AccessPathName(rootFilePath.c_str())) {
            std::cout << std::left << std::setw(25) << displayFileName 
                      << std::right << std::setw(11) << std::fixed << std::setprecision(2) << deadTimePercent << " "
                      << std::right << std::setw(14) << std::fixed << std::setprecision(2) << deadTimeErrorPercent << " "
                      << std::setw(20) << "ROOT file missing"
                      << std::endl;
            continue;
        }
        
        // 从ROOT文件分析峰
        double counts747, error747, counts633_634, error633_634;
        
        bool analysisSuccess = analyzeROOTFile(rootFilePath, counts747, error747, 
                                               counts633_634, error633_634);
        
        // 计算原始计数率及其统计误差
        double countRate747 = 0.0, countRateError747 = 0.0;
        double countRate633_634 = 0.0, countRateError633_634 = 0.0;
        
        if (liveTime > 0 && analysisSuccess) {
            countRate747 = counts747 / liveTime;
            countRateError747 = error747 / liveTime;  // 计数率统计误差
            
            countRate633_634 = counts633_634 / liveTime;
            countRateError633_634 = error633_634 / liveTime;  // 计数率统计误差
        }
        
        // 进行死时间修正并计算总误差
        double correctedRate747 = 0.0, correctedError747 = 0.0;
        double correctedRate633_634 = 0.0, correctedError633_634 = 0.0;
        
        if (analysisSuccess) {
            // 计算747 keV峰的修正后计数率和误差
            calculateCorrectedRate(countRate747, countRateError747,
                                 deadTimePercent, deadTimeErrorPercent,
                                 correctedRate747, correctedError747);
            
            // 计算633/634 keV峰的修正后计数率和误差
            calculateCorrectedRate(countRate633_634, countRateError633_634,
                                 deadTimePercent, deadTimeErrorPercent,
                                 correctedRate633_634, correctedError633_634);
        }
        
        // 输出结果
        std::cout << std::left << std::setw(25) << displayFileName;
        
        // 死时间和死时间误差
        std::cout << std::right << std::setw(11) << std::fixed << std::setprecision(2) << deadTimePercent << " ";
        std::cout << std::right << std::setw(14) << std::fixed << std::setprecision(2) << deadTimeErrorPercent << " ";
        
        if (analysisSuccess) {
            // 747 keV峰：原始计数率±误差
            std::cout << std::right << std::setw(18) 
                      << std::fixed << std::setprecision(4) << countRate747 
                      << " ± " << std::setprecision(4) << countRateError747 << " ";
            
            // 747 keV峰：修正后计数率±误差
            std::cout << std::right << std::setw(18) 
                      << std::fixed << std::setprecision(4) << correctedRate747 
                      << " ± " << std::setprecision(4) << correctedError747 << " ";
            
            // 633/634 keV峰：原始计数率±误差
            std::cout << std::right << std::setw(18) 
                      << std::fixed << std::setprecision(4) << countRate633_634 
                      << " ± " << std::setprecision(4) << countRateError633_634 << " ";
            
            // 633/634 keV峰：修正后计数率±误差
            std::cout << std::right << std::setw(18) 
                      << std::fixed << std::setprecision(4) << correctedRate633_634 
                      << " ± " << std::setprecision(4) << correctedError633_634;
            
            analyzedCount++;
        } else {
            // 分析失败时
            std::cout << std::right << std::setw(20) << "N/A" << " "
                      << std::right << std::setw(20) << "N/A" << " "
                      << std::right << std::setw(20) << "N/A" << " "
                      << std::right << std::setw(20) << "N/A";
        }
        std::cout << std::endl;
    }
    
    std::cout << "========================================================================================" << std::endl;
    std::cout << "Successfully analyzed " << analyzedCount << " file pairs" << std::endl;
    std::cout << "Dead time correction based on Cs-137/Co-60 two-source method" << std::endl;
    std::cout << "Correction formula: Measured Rate = " << P0 << " + (" << P1 << ") × DeadTime(%)" << std::endl;
    std::cout << "Valid range: 0% to ~14% dead time" << std::endl;
    std::cout << "HPGe detector dead time error estimation:" << std::endl;
    std::cout << "  - Dead time < 5%: 0.15% error" << std::endl;
    std::cout << "  - Dead time 5-15%: 0.25% error" << std::endl;
    std::cout << "  - Dead time > 15%: 0.40% error" << std::endl;
    std::cout << "Corrected rate error includes: " << std::endl;
    std::cout << "  1. Statistical error from peak analysis" << std::endl;
    std::cout << "  2. Dead time measurement error" << std::endl;
    std::cout << "  3. Correction formula parameter errors (p0, p1)" << std::endl;
    
    delete fileList;
}

// 运行函数
void runFullAnalysis() {
    AnalyzeSPEandROOTFiles();
}
// 文件名：Sm.C
#include "TFile.h"
#include "TH1D.h"
#include <iostream>
#include <fstream>
#include <string>

void Sm() {
    // ====【用户设置部分】====
    TString filename = "2025_146Sm130.root";  // ROOT 文件名
    TString histName = "adc2ch21;1";                // 想导出的 TH1D 名字（在这里修改）

    // 打开ROOT文件
    TFile *file = TFile::Open(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    // 从文件中读取指定的TH1D
    TH1D *hist = (TH1D *)file->Get(histName);
    if (!hist) {
        std::cerr << "未找到名为 \"" << histName << "\" 的TH1D直方图！" << std::endl;
        file->Close();
        return;
    }

    // 输出CSV文件名
    TString csvName = histName + ".csv";
    std::ofstream out(csvName.Data());
    if (!out.is_open()) {
        std::cerr << "无法创建CSV文件: " << csvName << std::endl;
        file->Close();
        return;
    }

    // 写入表头
    out << "X,Y\n";

    // 遍历所有bin，输出X轴中心和Y值
    int nbins = hist->GetNbinsX();
    for (int i = 1; i <= nbins; ++i) {
        double x = hist->GetBinCenter(i);
        double y = hist->GetBinContent(i);
        out << x << "," << y << "\n";
    }

    out.close();
    std::cout << "已成功导出 " << histName << " 到文件: " << csvName << std::endl;

    file->Close();
}

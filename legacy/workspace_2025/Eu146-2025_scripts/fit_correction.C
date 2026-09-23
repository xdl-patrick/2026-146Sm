#include <vector>
#include <TGraph.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TString.h>
#include <TLatex.h>
#include <iostream>
#include <algorithm>

void fit_correction() {
    // 设置样式
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetLabelFont(42, "XYZ");
    gStyle->SetTitleSize(0.05, "XYZ");
    gStyle->SetLabelSize(0.04, "XYZ");
    
    // 铅砖数据
    std::vector<double> time_lead = {972.7,995.7,1021.7,1043.7,1068.7,1096.7,1117.7,1141.7,1166.7,1189.7,1213.7};
    std::vector<double> y747_lead = {2.806106223,2.687068956,2.539362404,2.417746931,2.322443581,2.193911636,2.101710539,2.003708088,1.922298232,1.795448568,1.709460457};
    
    // 铜砖数据
    std::vector<double> time_copper = {1142.7,1164.7,1190.7,1212.7,1237.7,1261.7,1285.7,1309.7,1333.7,1358.7,1382.7,1407.7,1435.7,1449.7,1485.7,1504.7,1534.7,1545.7,1579.7};
    std::vector<double> y747_copper = {2.334761825,2.230847386,2.144819554,2.061691182,1.973108331,1.896607354,1.817955017,1.76615672,1.674422763,1.592749431,1.528932598,1.479500055,1.441078431,1.408055853,1.34142774,1.323287999,1.277594944,1.26624313,1.191495283};
    
    int n_lead = time_lead.size();
    int n_copper = time_copper.size();
    
    cout << "=== 数据统计 ===" << endl;
    cout << "铅砖数据点数: " << n_lead << endl;
    cout << "铜砖数据点数: " << n_copper << endl;
    
    // 1. 分别拟合两种材料的数据
    TGraph *g_lead = new TGraph(n_lead, &time_lead[0], &y747_lead[0]);
    TF1 *fit_lead = new TF1("fit_lead", "[0] + [1]*x", 900, 1600);
    fit_lead->SetParameters(3.5, -0.001);
    fit_lead->SetLineColor(kBlue);
    fit_lead->SetLineWidth(2);
    
    TGraph *g_copper = new TGraph(n_copper, &time_copper[0], &y747_copper[0]);
    TF1 *fit_copper = new TF1("fit_copper", "[0] + [1]*x", 900, 1600);
    fit_copper->SetParameters(1.3, -0.001);
    fit_copper->SetLineColor(kRed);
    fit_copper->SetLineWidth(2);
    fit_copper->SetLineStyle(2);
    
    cout << "\n=== 分别拟合 ===" << endl;
    g_lead->Fit("fit_lead", "QR");
    g_copper->Fit("fit_copper", "QR");
    
    double a_lead = fit_lead->GetParameter(0);
    double b_lead = fit_lead->GetParameter(1);
    double a_copper = fit_copper->GetParameter(0);
    double b_copper = fit_copper->GetParameter(1);
    
    cout << "铅砖: y = " << a_lead << " + " << b_lead << " * x" << endl;
    cout << "铜砖: y = " << a_copper << " + " << b_copper << " * x" << endl;
    
    // 2. 检查斜率是否一致（这是修正的前提）
    double slope_ratio = fabs(b_copper / b_lead);
    cout << "\n斜率比: " << slope_ratio << endl;
    
    if (fabs(1 - slope_ratio) > 0.1) {
        cout << "警告：斜率差异较大！两种材料的衰减特性可能不同。" << endl;
        cout << "这种情况下，简单的垂直偏移修正可能不适用。" << endl;
    }
    
    // 3. 选择重叠时间区域进行修正（这里是关键）
    // 寻找时间重叠区域
    double min_time_lead = *std::min_element(time_lead.begin(), time_lead.end());
    double max_time_copper = *std::max_element(time_copper.begin(), time_copper.end());
    
    cout << "\n=== 寻找重叠时间区域 ===" << endl;
    cout << "铅砖最早时间: " << min_time_lead << endl;
    cout << "铜砖最晚时间: " << max_time_copper << endl;
    
    // 由于时间没有重叠，我们使用外推法
    // 在铜砖的最大时间和铅砖的最小时间之间选择中间点
    double ref_time = (min_time_lead + max_time_copper) / 2.0;
    cout << "参考时间点: " << ref_time << endl;
    
    // 4. 计算在参考时间点的偏移
    double lead_at_ref = a_lead + b_lead * ref_time;
    double copper_at_ref = a_copper + b_copper * ref_time;
    double offset = copper_at_ref - lead_at_ref;
    
    cout << "\n=== 参考点计算 ===" << endl;
    cout << "在时间 " << ref_time << " 处:" << endl;
    cout << "铅砖拟合值: " << lead_at_ref << endl;
    cout << "铜砖拟合值: " << copper_at_ref << endl;
    cout << "偏移量: " << offset << endl;
    
    // 5. 更合理的修正方法：使用拟合残差的标准差
    // 计算铅砖拟合的标准差
    double sum_sq_resid_lead = 0;
    for (int i = 0; i < n_lead; i++) {
        double residual = y747_lead[i] - (a_lead + b_lead * time_lead[i]);
        sum_sq_resid_lead += residual * residual;
    }
    double std_lead = sqrt(sum_sq_resid_lead / (n_lead - 2));
    
    // 计算铜砖拟合的标准差
    double sum_sq_resid_copper = 0;
    for (int i = 0; i < n_copper; i++) {
        double residual = y747_copper[i] - (a_copper + b_copper * time_copper[i]);
        sum_sq_resid_copper += residual * residual;
    }
    double std_copper = sqrt(sum_sq_resid_copper / (n_copper - 2));
    
    cout << "\n=== 拟合质量 ===" << endl;
    cout << "铅砖拟合标准差: " << std_lead << endl;
    cout << "铜砖拟合标准差: " << std_copper << endl;
    
    // 6. 基于物理考虑重新计算修正
    // 由于数据没有重叠，且偏移太大，可能需要考虑：
    // a) 实验条件不同（如源强不同）
    // b) 测量系统不同
    // c) 数据需要归一化
    
    cout << "\n=== 问题分析 ===" << endl;
    cout << "原始方法得到的修正系数太大（16.9倍），这可能表明：" << endl;
    cout << "1. 两种测量可能使用了不同的源强" << endl;
    cout << "2. 探测器效率可能有变化" << endl;
    cout << "3. 测量条件不一致" << endl;
    
    // 7. 尝试归一化方法：基于早期数据点
    // 找到时间上最接近的点
    double min_time_diff = 1e9;
    int idx_lead_close = -1;
    int idx_copper_close = -1;
    
    for (int i = 0; i < n_lead; i++) {
        for (int j = 0; j < n_copper; j++) {
            double time_diff = fabs(time_lead[i] - time_copper[j]);
            if (time_diff < min_time_diff) {
                min_time_diff = time_diff;
                idx_lead_close = i;
                idx_copper_close = j;
            }
        }
    }
    
    cout << "\n=== 最接近点分析 ===" << endl;
    cout << "最小时差: " << min_time_diff << endl;
    if (idx_lead_close >= 0 && idx_copper_close >= 0) {
        cout << "铅砖点 " << idx_lead_close << ": 时间=" << time_lead[idx_lead_close] 
             << ", 值=" << y747_lead[idx_lead_close] << endl;
        cout << "铜砖点 " << idx_copper_close << ": 时间=" << time_copper[idx_copper_close] 
             << ", 值=" << y747_copper[idx_copper_close] << endl;
        
        // 计算基于最接近点的偏移
        double offset_close = y747_copper[idx_copper_close] - y747_lead[idx_lead_close];
        cout << "直接偏移: " << offset_close << endl;
        cout << "修正系数: " << TMath::Exp(-offset_close) << endl;
    }
    
    // 8. 建议的修正方法：基于斜率一致性的假设
    // 假设两种材料对相同辐射的响应斜率相同，只差一个常数因子
    if (fabs(b_copper - b_lead) / fabs(b_lead) < 0.1) {
        cout << "\n=== 基于斜率一致性的修正 ===" << endl;
        
        // 计算平均偏移（考虑时间依赖）
        double sum_weighted_offset = 0;
        double sum_weights = 0;
        
        for (int i = 0; i < n_copper; i++) {
            // 对每个铜砖点，计算到铅砖拟合线的偏移
            double lead_expected = a_lead + b_lead * time_copper[i];
            double offset_i = y747_copper[i] - lead_expected;
            
            // 使用权重（基于时间接近程度）
            double weight = 1.0 / (1.0 + fabs(time_copper[i] - ref_time)/100.0);
            sum_weighted_offset += offset_i * weight;
            sum_weights += weight;
        }
        
        double weighted_offset = sum_weighted_offset / sum_weights;
        cout << "加权平均偏移: " << weighted_offset << endl;
        cout << "修正系数: " << TMath::Exp(-weighted_offset) << endl;
        
        // 使用这个偏移
        offset = weighted_offset;
    }
    
    // 9. 应用修正
    double correction_factor = TMath::Exp(-offset);
    
    cout << "\n=== 最终修正参数 ===" << endl;
    cout << "偏移量: " << offset << endl;
    cout << "修正系数: " << correction_factor << endl;
    
    // 10. 验证修正效果
    std::vector<double> y747_copper_corrected(n_copper);
    for (int i = 0; i < n_copper; i++) {
        y747_copper_corrected[i] = y747_copper[i] - offset;
    }
    
    // 11. 绘图展示
    TCanvas *c1 = new TCanvas("c1", "数据修正分析", 1200, 800);
    c1->Divide(2, 2);
    
    // 图1: 原始数据对比
    c1->cd(1);
    gPad->SetGrid(1, 1);
    
    TGraph *g1_lead = new TGraph(n_lead, &time_lead[0], &y747_lead[0]);
    TGraph *g1_copper = new TGraph(n_copper, &time_copper[0], &y747_copper[0]);
    
    g1_lead->SetTitle("原始数据;时间;ln(计数率)");
    g1_lead->SetMarkerColor(kBlue);
    g1_lead->SetMarkerStyle(20);
    g1_lead->SetMarkerSize(1.2);
    g1_copper->SetMarkerColor(kRed);
    g1_copper->SetMarkerStyle(21);
    g1_copper->SetMarkerSize(1.2);
    
    TMultiGraph *mg1 = new TMultiGraph();
    mg1->Add(g1_lead);
    mg1->Add(g1_copper);
    mg1->Draw("AP");
    
    fit_lead->Draw("same");
    fit_copper->Draw("same");
    
    TLegend *leg1 = new TLegend(0.15, 0.75, 0.45, 0.9);
    leg1->AddEntry(g1_lead, "铅砖", "p");
    leg1->AddEntry(g1_copper, "铜砖", "p");
    leg1->AddEntry(fit_lead, Form("铅拟合: %.3f+%.5fx", a_lead, b_lead), "l");
    leg1->AddEntry(fit_copper, Form("铜拟合: %.3f+%.5fx", a_copper, b_copper), "l");
    leg1->SetBorderSize(1);
    leg1->SetFillStyle(0);
    leg1->Draw();
    
    // 图2: 修正后数据
    c1->cd(2);
    gPad->SetGrid(1, 1);
    
    TGraph *g2_lead = new TGraph(n_lead, &time_lead[0], &y747_lead[0]);
    TGraph *g2_copper = new TGraph(n_copper, &time_copper[0], &y747_copper_corrected[0]);
    
    g2_lead->SetTitle("修正后数据;时间;ln(计数率)");
    g2_lead->SetMarkerColor(kBlue);
    g2_lead->SetMarkerStyle(20);
    g2_lead->SetMarkerSize(1.2);
    g2_copper->SetMarkerColor(kGreen+2);
    g2_copper->SetMarkerStyle(22);
    g2_copper->SetMarkerSize(1.2);
    
    TMultiGraph *mg2 = new TMultiGraph();
    mg2->Add(g2_lead);
    mg2->Add(g2_copper);
    mg2->Draw("AP");
    
    // 绘制铅砖拟合线作为参考
    fit_lead->Draw("same");
    
    TLegend *leg2 = new TLegend(0.15, 0.75, 0.45, 0.9);
    leg2->AddEntry(g2_lead, "铅砖", "p");
    leg2->AddEntry(g2_copper, "铜砖(修正后)", "p");
    leg2->AddEntry(fit_lead, "铅拟合线", "l");
    leg2->SetBorderSize(1);
    leg2->SetFillStyle(0);
    leg2->Draw();
    
    // 添加修正信息
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.04);
    tex->DrawLatex(0.55, 0.25, Form("修正系数: %.3f", correction_factor));
    tex->DrawLatex(0.55, 0.20, Form("偏移量: %.3f", offset));
    
    // 图3: 残差分析
    c1->cd(3);
    gPad->SetGrid(1, 1);
    
    // 计算残差
    std::vector<double> residuals_lead, residuals_copper;
    std::vector<double> res_time_lead, res_time_copper;
    
    for (int i = 0; i < n_lead; i++) {
        double expected = a_lead + b_lead * time_lead[i];
        residuals_lead.push_back(y747_lead[i] - expected);
        res_time_lead.push_back(time_lead[i]);
    }
    
    for (int i = 0; i < n_copper; i++) {
        double expected = a_copper + b_copper * time_copper[i];
        residuals_copper.push_back(y747_copper[i] - expected);
        res_time_copper.push_back(time_copper[i]);
    }
    
    TGraph *g3_lead = new TGraph(res_time_lead.size(), &res_time_lead[0], &residuals_lead[0]);
    TGraph *g3_copper = new TGraph(res_time_copper.size(), &res_time_copper[0], &residuals_copper[0]);
    
    g3_lead->SetTitle("拟合残差;时间;残差");
    g3_lead->SetMarkerColor(kBlue);
    g3_lead->SetMarkerStyle(20);
    g3_lead->SetMarkerSize(1.2);
    g3_copper->SetMarkerColor(kRed);
    g3_copper->SetMarkerStyle(21);
    g3_copper->SetMarkerSize(1.2);
    
    TMultiGraph *mg3 = new TMultiGraph();
    mg3->Add(g3_lead);
    mg3->Add(g3_copper);
    mg3->Draw("AP");
    
    TLegend *leg3 = new TLegend(0.15, 0.75, 0.4, 0.9);
    leg3->AddEntry(g3_lead, "铅砖残差", "p");
    leg3->AddEntry(g3_copper, "铜砖残差", "p");
    leg3->SetBorderSize(1);
    leg3->SetFillStyle(0);
    leg3->Draw();
    
    // 图4: 偏移量分析
    c1->cd(4);
    gPad->SetGrid(1, 1);
    
    std::vector<double> offsets, offset_times;
    for (int i = 0; i < n_copper; i++) {
        double lead_expected = a_lead + b_lead * time_copper[i];
        offsets.push_back(y747_copper[i] - lead_expected);
        offset_times.push_back(time_copper[i]);
    }
    
    TGraph *g4 = new TGraph(offset_times.size(), &offset_times[0], &offsets[0]);
    g4->SetTitle("铜砖相对于铅砖拟合的偏移;时间;偏移量");
    g4->SetMarkerColor(kMagenta);
    g4->SetMarkerStyle(23);
    g4->SetMarkerSize(1.2);
    g4->Draw("AP");
    
    // 绘制平均偏移线
    TLine *avg_line = new TLine(time_copper[0], offset, time_copper.back(), offset);
    avg_line->SetLineColor(kRed);
    avg_line->SetLineWidth(2);
    avg_line->SetLineStyle(2);
    avg_line->Draw("same");
    
    TLegend *leg4 = new TLegend(0.15, 0.75, 0.4, 0.9);
    leg4->AddEntry(g4, "各点偏移", "p");
    leg4->AddEntry(avg_line, Form("平均偏移: %.3f", offset), "l");
    leg4->SetBorderSize(1);
    leg4->SetFillStyle(0);
    leg4->Draw();
    
    c1->Update();
    
    // 12. 保存和输出
    cout << "\n=== 应用建议 ===" << endl;
    cout << "由于数据时间范围不重叠且偏移较大，建议：" << endl;
    cout << "1. 检查实验条件是否一致" << endl;
    cout << "2. 考虑是否需要重新测量重叠时间区域的数据" << endl;
    cout << "3. 当前修正系数: " << correction_factor << endl;
    
    if (correction_factor > 10 || correction_factor < 0.1) {
        cout << "警告：修正系数异常，建议重新评估数据有效性！" << endl;
    }
    
    c1->SaveAs("correction_analysis.pdf");
    cout << "\n图形已保存为: correction_analysis.pdf" << endl;
}

// 如果还是太大，尝试这个更简单的归一化方法
void simple_normalization() {
    // 铅砖数据
    std::vector<double> time_lead = {615.7, 636.7, 662.7, 686.7, 710.7, 731.7, 758.7, 780.7};
    std::vector<double> y747_lead = {3.49661, 3.3642, 3.20826, 3.05996, 2.88283, 2.74319, 2.57336, 2.42208};
    
    // 铜砖数据
    std::vector<double> time_copper = {514.7, 538.7, 564.7, 590.7, 612.7, 634.7, 658.7};
    std::vector<double> y747_copper = {1.30128, 1.19079, 1.01324, 0.82418, 0.7089, 0.55043, 0.42918};
    
    cout << "\n=== 简单归一化方法 ===" << endl;
    cout << "假设两种材料在相同时间应有相同的衰减率" << endl;
    
    // 计算平均斜率
    double sum_slope_lead = 0;
    for (int i = 0; i < time_lead.size()-1; i++) {
        double slope = (y747_lead[i+1] - y747_lead[i]) / (time_lead[i+1] - time_lead[i]);
        sum_slope_lead += slope;
    }
    double avg_slope_lead = sum_slope_lead / (time_lead.size()-1);
    
    double sum_slope_copper = 0;
    for (int i = 0; i < time_copper.size()-1; i++) {
        double slope = (y747_copper[i+1] - y747_copper[i]) / (time_copper[i+1] - time_copper[i]);
        sum_slope_copper += slope;
    }
    double avg_slope_copper = sum_slope_copper / (time_copper.size()-1);
    
    cout << "铅砖平均斜率: " << avg_slope_lead << endl;
    cout << "铜砖平均斜率: " << avg_slope_copper << endl;
    
    // 使用共同斜率，只调整截距
    double common_slope = (avg_slope_lead + avg_slope_copper) / 2.0;
    cout << "共同斜率: " << common_slope << endl;
    
    // 计算归一化因子
    double norm_factor = exp(-(y747_copper[0] - y747_lead[0]));
    cout << "基于第一点的归一化因子: " << norm_factor << endl;
    
    // 或者使用时间加权
    double time_mid = (time_lead[0] + time_copper.back()) / 2.0;
    double lead_mid = y747_lead[0] + common_slope * (time_mid - time_lead[0]);
    double copper_mid = y747_copper.back() + common_slope * (time_mid - time_copper.back());
    double norm_factor2 = exp(-(copper_mid - lead_mid));
    
    cout << "基于中间点的归一化因子: " << norm_factor2 << endl;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

// 通道数据结构
struct ChannelData {
    std::string name;
    double alpha_peak;    // α源峰位 (ADC道址)
    double sm147_peak;    // Sm-147源峰位 (ADC道址)
    double slope;         // 计算得到的斜率
    double intercept;     // 计算得到的截距
    double quality;       // 质量因子
};

// 已知源能量 (MeV)
const double ALPHA_ENERGY = 5.48;
const double SM147_ENERGY = 2.0;

// 读取通道数据
std::vector<ChannelData> ReadChannelData() {
    std::vector<ChannelData> channels;
    
    // ADC0 数据
    channels.push_back({"adc0ch0", 3487.10, 1430.7});
    channels.push_back({"adc0ch1", 3514.10, 1436.77});
    channels.push_back({"adc0ch2", 3525.93, 1438.43});
    channels.push_back({"adc0ch3", 3502.58, 1427.53});
    channels.push_back({"adc0ch4", 3618.61, 1492.19});
    channels.push_back({"adc0ch5", 3506.44, 1442.28});
    channels.push_back({"adc0ch6", 3492.06, 1429.73});
    channels.push_back({"adc0ch7", 3564.83, 1457.87});
    channels.push_back({"adc0ch8", 3557.28, 1460.84});
    channels.push_back({"adc0ch9", 3500.60, 1471.88});
    channels.push_back({"adc0ch10", 3604.86, 1490.1});
    channels.push_back({"adc0ch11", 3414.67, 1430.45});
    channels.push_back({"adc0ch12", 3552.37, 1488.24});
    channels.push_back({"adc0ch13", 3423.39, 1422.95});
    channels.push_back({"adc0ch14", 3553.05, 1478.7});
    channels.push_back({"adc0ch15", 3471.76, 1437.78});
    channels.push_back({"adc0ch16", 2863.74, 1182.79});
    channels.push_back({"adc0ch17", 3355.92, 1314.01});
    channels.push_back({"adc0ch18", 3383.90, 1384.4});
    channels.push_back({"adc0ch19", 3136.14, 1376.81});
    channels.push_back({"adc0ch20", 3216.53, 0}); // 峰前移，标记为无效
    channels.push_back({"adc0ch21", 3345.14, 1385.18});
    channels.push_back({"adc0ch22", 3489.25, 1453.51});
    channels.push_back({"adc0ch23", 3321.3, 1390.51});
    channels.push_back({"adc0ch24", 3463.15, 1463.5});
    channels.push_back({"adc0ch25", 3416.49, 1399.13});
    channels.push_back({"adc0ch26", 3536.43, 1467.56});
    channels.push_back({"adc0ch27", 3342.47, 1441.24});
    channels.push_back({"adc0ch28", 3020.44, 1288.08});
    channels.push_back({"adc0ch29", 2420.04, 1408.22});
    channels.push_back({"adc0ch30", 3514.78, 1411.01});
    channels.push_back({"adc0ch31", 0, 1359.48}); // 双峰，标记为无效

    // ADC1 数据
    channels.push_back({"adc1ch0", 3458.18, 1257.62});
    channels.push_back({"adc1ch1", 3463.11, 1257.8});
    channels.push_back({"adc1ch2", 3388.33, 1256.09});
    channels.push_back({"adc1ch3", 3445.28, 1277.27});
    channels.push_back({"adc1ch4", 3537.95, 1335.72});
    channels.push_back({"adc1ch5", 3355.66, 1268.15});
    channels.push_back({"adc1ch6", 3544.42, 1350.03});
    channels.push_back({"adc1ch7", 3454.13, 1301.71});
    channels.push_back({"adc1ch8", 3413.48, 1316.28});
    channels.push_back({"adc1ch9", 3361.87, 1287.8});
    channels.push_back({"adc1ch10", 3557.46, 1368.04});
    channels.push_back({"adc1ch11", 3113.39, 1244.31});
    channels.push_back({"adc1ch12", 3414.61, 1292.02});
    channels.push_back({"adc1ch13", 3484.85, 1296.91});
    channels.push_back({"adc1ch14", 3449.51, 1282.52});
    channels.push_back({"adc1ch15", 3495.21, 1275.43});
    channels.push_back({"adc1ch16", 3392.92, 1292.82});
    channels.push_back({"adc1ch17", 3481.24, 1317.46});
    channels.push_back({"adc1ch18", 3374.59, 1300.19});
    channels.push_back({"adc1ch19", 3496.42, 1336.87});
    channels.push_back({"adc1ch20", 3403.24, 1314.07});
    channels.push_back({"adc1ch21", 3409.28, 1301.83});
    channels.push_back({"adc1ch22", 3404.48, 1303.1});
    channels.push_back({"adc1ch23", 3497.1, 1341.72});
    channels.push_back({"adc1ch24", 3407.2, 1291.05});
    channels.push_back({"adc1ch25", 3538.0, 1336.78});
    channels.push_back({"adc1ch26", 3489.04, 1335.87});
    channels.push_back({"adc1ch27", 3420.23, 1295.93});
    channels.push_back({"adc1ch28", 3451.33, 1328.86});
    channels.push_back({"adc1ch29", 3371.27, 1284.73});
    channels.push_back({"adc1ch30", 3472.22, 1338.29});
    channels.push_back({"adc1ch31", 3464.51, 1325.54});

    // ADC2 数据
    channels.push_back({"adc2ch0", 3453.81, 1356.81});
    channels.push_back({"adc2ch1", 3395.05, 1325.85});
    channels.push_back({"adc2ch2", 3439.58, 1346.07});
    channels.push_back({"adc2ch3", 3412.15, 1331.44});
    channels.push_back({"adc2ch4", 3493.0, 1363.74});
    channels.push_back({"adc2ch5", 3414.32, 1334.07});
    channels.push_back({"adc2ch6", 3381.88, 1322.37});
    channels.push_back({"adc2ch7", 3521.43, 1385.53});
    channels.push_back({"adc2ch8", 3512.62, 1381.9});
    channels.push_back({"adc2ch9", 3446.48, 1336.52});
    channels.push_back({"adc2ch10", 3438.3, 1351.16});
    channels.push_back({"adc2ch11", 3368.74, 1325.17});
    channels.push_back({"adc2ch12", 3502.09, 1378.21});
    channels.push_back({"adc2ch13", 3488.11, 1385.06});
    channels.push_back({"adc2ch14", 3483.72, 1381.88});
    channels.push_back({"adc2ch15", 3455.57, 1353.37});
    channels.push_back({"adc2ch16", 3475.09, 1359.16});
    channels.push_back({"adc2ch17", 3510.19, 1375.89});
    channels.push_back({"adc2ch18", 3555.66, 1413.48});
    channels.push_back({"adc2ch19", 3426.6, 1360.74});
    channels.push_back({"adc2ch20", 3548.16, 1410.83});
    channels.push_back({"adc2ch21", 3466.18, 1366.45});
    channels.push_back({"adc2ch22", 3523.12, 1413.5});
    channels.push_back({"adc2ch23", 3467.39, 1369.21});
    channels.push_back({"adc2ch24", 3535.84, 1425.03});
    channels.push_back({"adc2ch25", 3462.29, 1382.8});
    channels.push_back({"adc2ch26", 3420.46, 1379.74});
    channels.push_back({"adc2ch27", 3415.19, 1364.18});
    channels.push_back({"adc2ch28", 3445.32, 1398.92});
    channels.push_back({"adc2ch29", 3339.47, 1311.94});
    channels.push_back({"adc2ch30", 3435.61, 1354.92});
    channels.push_back({"adc2ch31", 3571.04, 1374.74});

    // ADC3 数据
    channels.push_back({"adc3ch0", 3599.03, 1390.68});
    channels.push_back({"adc3ch1", 3647.38, 1401.47});
    channels.push_back({"adc3ch2", 3511.86, 1340.05});
    channels.push_back({"adc3ch3", 3691.05, 1429.19});
    channels.push_back({"adc3ch4", 3662.32, 1413.77});
    channels.push_back({"adc3ch5", 3596.09, 1368.1});
    channels.push_back({"adc3ch6", 3578.05, 1371.86});
    channels.push_back({"adc3ch7", 3683.8, 1404.89});
    channels.push_back({"adc3ch8", 3698.47, 1436.16});
    channels.push_back({"adc3ch9", 3606.83, 1402.98});
    channels.push_back({"adc3ch10", 3737.59, 1462.64});
    channels.push_back({"adc3ch11", 3718.04, 1454.93});
    channels.push_back({"adc3ch12", 3640.72, 1412.82});
    channels.push_back({"adc3ch13", 3763.94, 1456.89});
    channels.push_back({"adc3ch14", 3648.07, 1404.11});
    channels.push_back({"adc3ch15", 3727.80, 1419.78});
    channels.push_back({"adc3ch16", 3678.84, 1382.06});
    channels.push_back({"adc3ch17", 3621.85, 1383.41});
    channels.push_back({"adc3ch18", 3626.32, 1383.6});
    channels.push_back({"adc3ch19", 3577.02, 1380.0});
    channels.push_back({"adc3ch20", 3629.17, 1400.48});
    channels.push_back({"adc3ch21", 3518.79, 1373.53});
    channels.push_back({"adc3ch22", 3686.4, 1440.57});
    channels.push_back({"adc3ch23", 3648.37, 1425.73});
    channels.push_back({"adc3ch24", 3628.38, 1418.78});
    channels.push_back({"adc3ch25", 3680.28, 1443.31});
    channels.push_back({"adc3ch26", 3622.39, 1416.11});
    channels.push_back({"adc3ch27", 3545.41, 1363.23});
    channels.push_back({"adc3ch28", 3592.41, 1378.19});
    channels.push_back({"adc3ch29", 3698.41, 1411.46});
    channels.push_back({"adc3ch30", 3469.04, 1384.23});
    channels.push_back({"adc3ch31", 3638.55, 1389.0});

    return channels;
}

// 计算斜率和截距
void CalculateCalibrationCoefficients(std::vector<ChannelData>& channels) {
    for (auto& channel : channels) {
        // 检查数据是否有效
        if (channel.alpha_peak <= 0 || channel.sm147_peak <= 0) {
            channel.slope = 0.0;
            channel.intercept = 0.0;
            channel.quality = 0.0;
            continue;
        }

        // 解线性方程组：
        // E_alpha = slope * channel_alpha + intercept
        // E_sm147 = slope * channel_sm147 + intercept
        
        // 计算斜率
        channel.slope = (ALPHA_ENERGY - SM147_ENERGY) / 
                       (channel.alpha_peak - channel.sm147_peak);
        
        // 计算截距
        channel.intercept = ALPHA_ENERGY - channel.slope * channel.alpha_peak;
        
        // 计算质量因子
        double predicted_alpha = channel.slope * channel.alpha_peak + channel.intercept;
        double predicted_sm147 = channel.slope * channel.sm147_peak + channel.intercept;
        
        double error_alpha = std::abs(predicted_alpha - ALPHA_ENERGY) / ALPHA_ENERGY;
        double error_sm147 = std::abs(predicted_sm147 - SM147_ENERGY) / SM147_ENERGY;
        
        channel.quality = 1.0 - (error_alpha + error_sm147) / 2.0;
    }
}

// 输出结果到文件
void OutputResults(const std::vector<ChannelData>& channels, const std::string& filename) {
    std::ofstream outfile(filename);
    
    outfile << "Channel,Alpha_Peak,Sm147_Peak,Slope,Intercept,Quality" << std::endl;
    outfile << std::fixed << std::setprecision(6);
    
    for (const auto& channel : channels) {
        outfile << channel.name << ","
                << channel.alpha_peak << ","
                << channel.sm147_peak << ","
                << channel.slope << ","
                << channel.intercept << ","
                << channel.quality << std::endl;
    }
    
    outfile.close();
}

// 生成C++格式的刻度系数数组
void GenerateCCode(const std::vector<ChannelData>& channels) {
    std::ofstream cfile("calibration_coefficients.h");
    
    cfile << "// Auto-generated calibration coefficients\n";
    cfile << "// Based on alpha (5.48 MeV) and Sm147 (2.0 MeV) sources\n\n";
    
    cfile << "const int N_DETECTORS = 4;\n";
    cfile << "const int N_CHANNELS = 32;\n\n";
    
    // 斜率系数数组
    cfile << "Float_t Calk[4][32] = {\n";
    for (int det = 0; det < 4; det++) {
        cfile << "    {";
        for (int ch = 0; ch < 32; ch++) {
            int index = det * 32 + ch;
            cfile << channels[index].slope;
            if (ch < 31) cfile << ", ";
        }
        cfile << "}";
        if (det < 3) cfile << ",";
        cfile << "\n";
    }
    cfile << "};\n\n";
    
    // 截距系数数组
    cfile << "Float_t Calb[4][32] = {\n";
    for (int det = 0; det < 4; det++) {
        cfile << "    {";
        for (int ch = 0; ch < 32; ch++) {
            int index = det * 32 + ch;
            cfile << channels[index].intercept;
            if (ch < 31) cfile << ", ";
        }
        cfile << "}";
        if (det < 3) cfile << ",";
        cfile << "\n";
    }
    cfile << "};\n";
    
    cfile.close();
}

// 生成统计报告
void GenerateReport(const std::vector<ChannelData>& channels) {
    int valid_channels = 0;
    double avg_slope = 0.0;
    double avg_intercept = 0.0;
    double avg_quality = 0.0;
    
    for (const auto& channel : channels) {
        if (channel.quality > 0) {
            valid_channels++;
            avg_slope += channel.slope;
            avg_intercept += channel.intercept;
            avg_quality += channel.quality;
        }
    }
    
    if (valid_channels > 0) {
        avg_slope /= valid_channels;
        avg_intercept /= valid_channels;
        avg_quality /= valid_channels;
    }
    
    std::cout << "=== Calibration Report ===\n";
    std::cout << "Valid channels: " << valid_channels << "/" << channels.size() << "\n";
    std::cout << "Average slope: " << avg_slope << "\n";
    std::cout << "Average intercept: " << avg_intercept << "\n";
    std::cout << "Average quality: " << avg_quality << "\n";
    
    // 找出质量最差的通道
    double min_quality = 1.0;
    std::string worst_channel;
    for (const auto& channel : channels) {
        if (channel.quality > 0 && channel.quality < min_quality) {
            min_quality = channel.quality;
            worst_channel = channel.name;
        }
    }
    
    std::cout << "Worst channel: " << worst_channel << " (quality: " << min_quality << ")\n";
}

// 主函数
void CalculateCalibrationCoefficients() {
    std::cout << "Calculating calibration coefficients..." << std::endl;
    
    // 读取通道数据
    std::vector<ChannelData> channels = ReadChannelData();
    
    // 计算刻度系数
    CalculateCalibrationCoefficients(channels);
    
    // 输出结果到CSV文件
    OutputResults(channels, "calibration_results.csv");
    std::cout << "Results saved to calibration_results.csv" << std::endl;
    
    // 生成C++代码
    GenerateCCode(channels);
    std::cout << "C++ code generated in calibration_coefficients.h" << std::endl;
    
    // 生成统计报告
    GenerateReport(channels);
    
    std::cout << "Calibration completed successfully!" << std::endl;
}

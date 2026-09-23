void CompareTwoSpectra() {
    // 打开文件
    TFile *f1 = TFile::Open("10.19_1#with Pb.root");
    TFile *f2 = TFile::Open("10.22_1#with Pb.root");
    
    if (!f1 || !f2) {
        std::cout << "Error opening files!" << std::endl;
        return;
    }
    
    // 获取直方图
    TH1D *h1 = (TH1D*)f1->Get("h");
    TH1D *h2 = (TH1D*)f2->Get("h");
    
    // 克隆直方图以避免修改原文件
    TH1D *h1_clone = (TH1D*)h1->Clone("h1_clone");
    TH1D *h2_clone = (TH1D*)h2->Clone("h2_clone");
    
    // 设置样式
    h1_clone->SetLineColor(kRed);
    h1_clone->SetLineWidth(2);
    h1_clone->SetTitle("Comparison of Two Spectra (Log Scale);Energy (keV);Counts");
    
    h2_clone->SetLineColor(kBlue);
    h2_clone->SetLineWidth(2);
    
    // 创建画布并设置对数坐标
    TCanvas *c1 = new TCanvas("c1", "Spectra Comparison - Log Scale", 1000, 800);
    c1->SetLogy();  // 设置Y轴为对数坐标
    
    // 绘制
    h1_clone->Draw();
    h2_clone->Draw("SAME");
    
    // 添加图例
    TLegend *leg = new TLegend(0.75, 0.75, 0.9, 0.9);
    leg->AddEntry(h1_clone, "10.19_1#with Pb", "l");
    leg->AddEntry(h2_clone, "10.22_1#with Pb", "l");
    leg->Draw();
    
    // 保存图片
    c1->SaveAs("comparison_plot_log.png");
    
    // 清理
    //delete c1;
    //delete leg;
    //f1->Close();
    //f2->Close();
}

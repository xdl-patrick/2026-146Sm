void ReadDetectorData(int run) {
    // 读取四个探测器数据文件
    TString fname1 = TString::Format("%d_1.root", run);
    TString fname2 = TString::Format("%d_2.root", run);
    TString fname3 = TString::Format("%d_3.root", run);
    TString fname4 = TString::Format("%d_4.root", run);
    
    TFile* f1 = new TFile(fname1);
    TFile* f2 = new TFile(fname2);
    TFile* f3 = new TFile(fname3);
    TFile* f4 = new TFile(fname4);
    
    TTree* t1 = (TTree*)f1->Get("tm");
    TTree* t2 = (TTree*)f2->Get("tm");
    TTree* t3 = (TTree*)f3->Get("tm");
    TTree* t4 = (TTree*)f4->Get("tm");
    
    // 设置分支地址
    Float_t FE1, BE1, DET1, PX1, PY1;
    Float_t FE2, BE2, DET2, PX2, PY2;
    Float_t FE3, BE3, DET3, PX3, PY3;
    Float_t FE4, BE4, DET4, PX4, PY4;
    
    t1->SetBranchAddress("FE", &FE1);
    t1->SetBranchAddress("BE", &BE1);
    t1->SetBranchAddress("DET", &DET1);
    t1->SetBranchAddress("PX", &PX1);
    t1->SetBranchAddress("PY", &PY1);
    
    t2->SetBranchAddress("FE", &FE2);
    t2->SetBranchAddress("BE", &BE2);
    t2->SetBranchAddress("DET", &DET2);
    t2->SetBranchAddress("PX", &PX2);
    t2->SetBranchAddress("PY", &PY2);
    
    t3->SetBranchAddress("FE", &FE3);
    t3->SetBranchAddress("BE", &BE3);
    t3->SetBranchAddress("DET", &DET3);
    t3->SetBranchAddress("PX", &PX3);
    t3->SetBranchAddress("PY", &PY3);
    
    t4->SetBranchAddress("FE", &FE4);
    t4->SetBranchAddress("BE", &BE4);
    t4->SetBranchAddress("DET", &DET4);
    t4->SetBranchAddress("PX", &PX4);
    t4->SetBranchAddress("PY", &PY4);
    
    // 创建直方图
    TH1F* h_FE_all = new TH1F("h_FE_all", "All Detectors Front Energy", 400, 0, 8);
    TH1F* h_BE_all = new TH1F("h_BE_all", "All Detectors Back Energy", 400, 0, 8);
    TH1F* h_total_all = new TH1F("h_total_all", "All Detectors Total Energy", 400, 0, 8);
    
    // 分别处理每个探测器的数据
    Long64_t nentries = t1->GetEntries();
    
    for (Long64_t i=0; i<nentries; i++) {
        t1->GetEntry(i);
        t2->GetEntry(i);
        t3->GetEntry(i);
        t4->GetEntry(i);
        
        // 填充直方图
        if (FE1 > 0.5) h_FE_all->Fill(FE1);
        if (FE2 > 0.5) h_FE_all->Fill(FE2);
        if (FE3 > 0.5) h_FE_all->Fill(FE3);
        if (FE4 > 0.5) h_FE_all->Fill(FE4);
        
        if (BE1 > 0.5) h_BE_all->Fill(BE1);
        if (BE2 > 0.5) h_BE_all->Fill(BE2);
        if (BE3 > 0.5) h_BE_all->Fill(BE3);
        if (BE4 > 0.5) h_BE_all->Fill(BE4);
        
        // 计算总能量
        if (FE1 > 0.5 && BE1 > 0.5) h_total_all->Fill(FE1 + BE1);
        if (FE2 > 0.5 && BE2 > 0.5) h_total_all->Fill(FE2 + BE2);
        if (FE3 > 0.5 && BE3 > 0.5) h_total_all->Fill(FE3 + BE3);
        if (FE4 > 0.5 && BE4 > 0.5) h_total_all->Fill(FE4 + BE4);
    }
    
    // 绘制结果
    TCanvas* c1 = new TCanvas("c1", "Detector Data", 1200, 800);
    c1->Divide(2,2);
    
    c1->cd(1);
    h_FE_all->Draw();
    
    c1->cd(2);
    h_BE_all->Draw();
    
    c1->cd(3);
    h_total_all->Draw();
    
    c1->cd(4);
    
    // 显示统计信息
    TPaveText* pt = new TPaveText(0.1,0.1,0.9,0.9);
    pt->AddText(Form("Run: %d", run));
    pt->AddText(Form("Detector 1 entries: %lld", t1->GetEntries()));
    pt->AddText(Form("Detector 2 entries: %lld", t2->GetEntries()));
    pt->AddText(Form("Detector 3 entries: %lld", t3->GetEntries()));
    pt->AddText(Form("Detector 4 entries: %lld", t4->GetEntries()));
    pt->Draw();
    
    // 关闭文件
    f1->Close();
    f2->Close();
    f3->Close();
    f4->Close();
}

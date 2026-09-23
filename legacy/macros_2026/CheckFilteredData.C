void CheckFilteredData(int run) {
    // 验证生成的文件和数据质量
    cout << "=== 数据质量检查 ===" << endl;
    
    for (int det=1; det<=4; det++) {
        TString fname = Form("%d_%d.root", run, det);
        
        if (gSystem->AccessPathName(fname)) {
            cout << "❌ 文件不存在: " << fname << endl;
            continue;
        }
        
        TFile* f = new TFile(fname);
        TTree* t = (TTree*)f->Get("tm");
        
        Float_t FE, BE, DET, PX, PY;
        t->SetBranchAddress("FE", &FE);
        t->SetBranchAddress("BE", &BE);
        t->SetBranchAddress("DET", &DET);
        t->SetBranchAddress("PX", &PX);
        t->SetBranchAddress("PY", &PY);
        
        Long64_t nentries = t->GetEntries();
        Int_t good_events = 0;
        
        // 检查前几个事件
        cout << "--- 探测器 " << det << " ---" << endl;
        cout << "总事件数: " << nentries << endl;
        
        for (int i=0; i<5 && i<nentries; i++) {
            t->GetEntry(i);
            cout << "  事件 " << i << ": FE=" << FE << ", BE=" << BE 
                 << ", PX=" << PX << ", PY=" << PY << endl;
        }
        
        f->Close();
    }
}

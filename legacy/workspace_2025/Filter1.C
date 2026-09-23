void Filter1(const int run)
{
    TString fin=TString::Format("2025_146Sm%d.root",run);
	TFile* fRun = new TFile(fin);
	TTree *t = (TTree*)fRun->Get("Tree1");




        Long64_t ntot = t->GetEntriesFast();

   Float_t FE ;  
   Float_t BE ;  
   Float_t DET ;  
   Float_t PX ;  
   Float_t PY ;  
   Float_t xe ;  
   Float_t th=0.5;
   
	UInt_t bArray[32];
	UInt_t cArray[32];
	UInt_t dArray[32];
	UInt_t eArray[32];



    Float_t Calk[4][32]=
	{
        0.001563,0.001559,0.001579,0.001601,0.001537,0.001604,0.001603,0.001587,
		0.001525,0.001558,0.001544,0.001604,0.001552,0.001598,0.001562,0.001587,
		0.001679,0.001586,0.001592,0.001592,0.001603,0.001617,0.001524,0.001593,
		0.001558,0.001577,0.001554,0.001544,0.001576,0.001595,0.001604,0.001615,
		0.001691,0.001670,0.001743,0.001694,0.001725,0.001744,0.001740,0.001749,
		0.001689,0.001624,0.001667,0.001678,0.001703,0.001722,0.001701,0.001714,
		0.001765,0.001676,0.001739,0.001672,0.001634,0.001722,0.001626,0.001654,
		0.001672,0.001692,0.001645,0.001682,0.001690,0.001691,0.001707,0.001708,
		0.001596,0.001630,0.001612,0.001633,0.001604,0.001646,0.001670,0.001624,
		0.001588,0.001603,0.001620,0.001658,0.001596,0.001602,0.001606,0.001607,
		0.001665,0.001622,0.001599,0.001658,0.001590,0.001607,0.001600,0.001602,
		0.001611,0.001613,0.001632,0.001588,0.001597,0.001685,0.001651,0.001615,
		0.001827,0.001802,0.001843,0.001778,0.001782,0.001779,0.001799,0.001759,
		0.001811,0.001851,0.001797,0.001801,0.001815,0.001752,0.001795,0.001740,
		0.001671,0.001683,0.001616,0.001649,0.001619,0.001668,0.001614,0.001631,
		0.001621,0.001620,0.001663,0.001651,0.001626,0.001577,0.001651,0.001613 
	};

	Float_t Calb[4][32]=
	{

		-0.2291,-0.2352,-0.2099,-0.2279,-0.2264,-0.2532,-0.2331,-0.2227,
		-0.2223,-0.2738,-0.2600,-0.2570,-0.2566,-0.2246,-0.2637,-0.2378,
		-0.2707,-0.2342,-0.2511,-0.2728,-0.2937,-0.2605,-0.2459,-0.2329,
		-0.2780,-0.2191,-0.2740,-0.2667,-0.2736,-0.2848,-0.2822,-0.2749,
		-0.2018,-0.1726,-0.1756,-0.1536,-0.2050,-0.1921,-0.2162,-0.2150,
		-0.2350,-0.2040,-0.2432,-0.1918,-0.2372,-0.2323,-0.2272,-0.2194,
		-0.2977,-0.2050,-0.2823,-0.2315,-0.2215,-0.2066,-0.2058,-0.2409,
		-0.2244,-0.2151,-0.2632,-0.2072,-0.2401,-0.2045,-0.2488,-0.2415,
		-0.1888,-0.1800,-0.1736,-0.1724,-0.1601,-0.1670,-0.1669,-0.2108,
		-0.2162,-0.1535,-0.2003,-0.2027,-0.1962,-0.2218,-0.2265,-0.1932,
		-0.2519,-0.2227,-0.2540,-0.2375,-0.2354,-0.1819,-0.2406,-0.1820,
		-0.2734,-0.2267,-0.2410,-0.1583,-0.2336,-0.2093,-0.2385,-0.2141,
		-0.1662,-0.1520,-0.1096,-0.1696,-0.1577,-0.1269,-0.1475,-0.1344,
		-0.1801,-0.1749,-0.1899,-0.2172,-0.1734,-0.1770,-0.1706,-0.1239,
		-0.1599,-0.1733,-0.1467,-0.1622,-0.1389,-0.1569,-0.1780,-0.1678,
		-0.1492,-0.1841,-0.1661,-0.1170,-0.1328,-0.1400,-0.1784,-0.1748 

	};

    
    
	t->SetBranchAddress("adc0ch0", &bArray[0] );
	t->SetBranchAddress("adc0ch1", &bArray[1] );
	t->SetBranchAddress("adc0ch2", &bArray[2] );
	t->SetBranchAddress("adc0ch3", &bArray[3] );
	t->SetBranchAddress("adc0ch4", &bArray[4] );
	t->SetBranchAddress("adc0ch5", &bArray[5] );
	t->SetBranchAddress("adc0ch6", &bArray[6] );
	t->SetBranchAddress("adc0ch7", &bArray[7] );
	t->SetBranchAddress("adc0ch8", &bArray[8] );
	t->SetBranchAddress("adc0ch9", &bArray[9] );
	t->SetBranchAddress("adc0ch10",&bArray[10]);
	t->SetBranchAddress("adc0ch11",&bArray[11]);
	t->SetBranchAddress("adc0ch12",&bArray[12]);
	t->SetBranchAddress("adc0ch13",&bArray[13]);
	t->SetBranchAddress("adc0ch14",&bArray[14]);
	t->SetBranchAddress("adc0ch15",&bArray[15]);
	t->SetBranchAddress("adc0ch16",&bArray[16]);
	t->SetBranchAddress("adc0ch17",&bArray[17]);
	t->SetBranchAddress("adc0ch18",&bArray[18]); 
	t->SetBranchAddress("adc0ch19",&bArray[19]);
	t->SetBranchAddress("adc0ch20",&bArray[20]);
	t->SetBranchAddress("adc0ch21",&bArray[21]);
	t->SetBranchAddress("adc0ch22",&bArray[22]);
	t->SetBranchAddress("adc0ch23",&bArray[23]);
	t->SetBranchAddress("adc0ch24",&bArray[24]);
	t->SetBranchAddress("adc0ch25",&bArray[25]);
	t->SetBranchAddress("adc0ch26",&bArray[26]);
	t->SetBranchAddress("adc0ch27",&bArray[27]);
	t->SetBranchAddress("adc0ch28",&bArray[28]);
	t->SetBranchAddress("adc0ch29",&bArray[29]);
	t->SetBranchAddress("adc0ch30",&bArray[30]);
	t->SetBranchAddress("adc0ch31",&bArray[31]);


	t->SetBranchAddress("adc1ch0", &cArray[0] );
	t->SetBranchAddress("adc1ch1", &cArray[1] );
	t->SetBranchAddress("adc1ch2", &cArray[2] );
	t->SetBranchAddress("adc1ch3", &cArray[3] );
	t->SetBranchAddress("adc1ch4", &cArray[4] );
	t->SetBranchAddress("adc1ch5", &cArray[5] );
	t->SetBranchAddress("adc1ch6", &cArray[6] );
	t->SetBranchAddress("adc1ch7", &cArray[7] );
	t->SetBranchAddress("adc1ch8", &cArray[8] );
	t->SetBranchAddress("adc1ch9", &cArray[9] );
	t->SetBranchAddress("adc1ch10",&cArray[10]);
	t->SetBranchAddress("adc1ch11",&cArray[11]);
	t->SetBranchAddress("adc1ch12",&cArray[12]);
	t->SetBranchAddress("adc1ch13",&cArray[13]);
	t->SetBranchAddress("adc1ch14",&cArray[14]);
	t->SetBranchAddress("adc1ch15",&cArray[15]);
	t->SetBranchAddress("adc1ch16",&cArray[16]);
	t->SetBranchAddress("adc1ch17",&cArray[17]);
	t->SetBranchAddress("adc1ch18",&cArray[18]); 
	t->SetBranchAddress("adc1ch19",&cArray[19]);
	t->SetBranchAddress("adc1ch20",&cArray[20]);
	t->SetBranchAddress("adc1ch21",&cArray[21]);
	t->SetBranchAddress("adc1ch22",&cArray[22]);
	t->SetBranchAddress("adc1ch23",&cArray[23]);
	t->SetBranchAddress("adc1ch24",&cArray[24]);
	t->SetBranchAddress("adc1ch25",&cArray[25]);
	t->SetBranchAddress("adc1ch26",&cArray[26]);
	t->SetBranchAddress("adc1ch27",&cArray[27]);
	t->SetBranchAddress("adc1ch28",&cArray[28]);
	t->SetBranchAddress("adc1ch29",&cArray[29]);
	t->SetBranchAddress("adc1ch30",&cArray[30]);
	t->SetBranchAddress("adc1ch31",&cArray[31]);

	t->SetBranchAddress("adc2ch0", &dArray[0] );
	t->SetBranchAddress("adc2ch1", &dArray[1] );
	t->SetBranchAddress("adc2ch2", &dArray[2] );
	t->SetBranchAddress("adc2ch3", &dArray[3] );
	t->SetBranchAddress("adc2ch4", &dArray[4] );
	t->SetBranchAddress("adc2ch5", &dArray[5] );
	t->SetBranchAddress("adc2ch6", &dArray[6] );
	t->SetBranchAddress("adc2ch7", &dArray[7] );
	t->SetBranchAddress("adc2ch8", &dArray[8] );
	t->SetBranchAddress("adc2ch9", &dArray[9] );
	t->SetBranchAddress("adc2ch10",&dArray[10]);
	t->SetBranchAddress("adc2ch11",&dArray[11]);
	t->SetBranchAddress("adc2ch12",&dArray[12]);
	t->SetBranchAddress("adc2ch13",&dArray[13]);
	t->SetBranchAddress("adc2ch14",&dArray[14]);
	t->SetBranchAddress("adc2ch15",&dArray[15]);
	t->SetBranchAddress("adc2ch16",&dArray[16]);
	t->SetBranchAddress("adc2ch17",&dArray[17]);
	t->SetBranchAddress("adc2ch18",&dArray[18]); 
	t->SetBranchAddress("adc2ch19",&dArray[19]);
	t->SetBranchAddress("adc2ch20",&dArray[20]);
	t->SetBranchAddress("adc2ch21",&dArray[21]);
	t->SetBranchAddress("adc2ch22",&dArray[22]);
	t->SetBranchAddress("adc2ch23",&dArray[23]);
	t->SetBranchAddress("adc2ch24",&dArray[24]);
	t->SetBranchAddress("adc2ch25",&dArray[25]);
	t->SetBranchAddress("adc2ch26",&dArray[26]);
	t->SetBranchAddress("adc2ch27",&dArray[27]);
	t->SetBranchAddress("adc2ch28",&dArray[28]);
	t->SetBranchAddress("adc2ch29",&dArray[29]);
	t->SetBranchAddress("adc2ch30",&dArray[30]);
	t->SetBranchAddress("adc2ch31",&dArray[31]);

	t->SetBranchAddress("adc3ch0", &eArray[0] );
	t->SetBranchAddress("adc3ch1", &eArray[1] );
	t->SetBranchAddress("adc3ch2", &eArray[2] );
	t->SetBranchAddress("adc3ch3", &eArray[3] );
	t->SetBranchAddress("adc3ch4", &eArray[4] );
	t->SetBranchAddress("adc3ch5", &eArray[5] );
	t->SetBranchAddress("adc3ch6", &eArray[6] );
	t->SetBranchAddress("adc3ch7", &eArray[7] );
	t->SetBranchAddress("adc3ch8", &eArray[8] );
	t->SetBranchAddress("adc3ch9", &eArray[9] );
	t->SetBranchAddress("adc3ch10",&eArray[10]);
	t->SetBranchAddress("adc3ch11",&eArray[11]);
	t->SetBranchAddress("adc3ch12",&eArray[12]);
	t->SetBranchAddress("adc3ch13",&eArray[13]);
	t->SetBranchAddress("adc3ch14",&eArray[14]);
	t->SetBranchAddress("adc3ch15",&eArray[15]);
	t->SetBranchAddress("adc3ch16",&eArray[16]);
	t->SetBranchAddress("adc3ch17",&eArray[17]);
	t->SetBranchAddress("adc3ch18",&eArray[18]); 
	t->SetBranchAddress("adc3ch19",&eArray[19]);
	t->SetBranchAddress("adc3ch20",&eArray[20]);
	t->SetBranchAddress("adc3ch21",&eArray[21]);
	t->SetBranchAddress("adc3ch22",&eArray[22]);
	t->SetBranchAddress("adc3ch23",&eArray[23]);
	t->SetBranchAddress("adc3ch24",&eArray[24]);
	t->SetBranchAddress("adc3ch25",&eArray[25]);
	t->SetBranchAddress("adc3ch26",&eArray[26]);
	t->SetBranchAddress("adc3ch27",&eArray[27]);
	t->SetBranchAddress("adc3ch28",&eArray[28]);
	t->SetBranchAddress("adc3ch29",&eArray[29]);
	t->SetBranchAddress("adc3ch30",&eArray[30]);
	t->SetBranchAddress("adc3ch31",&eArray[31]);
        
        

    TString fout1=TString::Format("%d_1.root",run);
	TFile* fOut1 = new TFile(fout1,"RECREATE");
    TTree* Tnew1 = new TTree("tm", "Tree0");
        Tnew1->Branch("FE"  ,&FE  ,"FE  /F" );
        Tnew1->Branch("BE"  ,&BE  ,"BE  /F" );
        Tnew1->Branch("DET" ,&DET ,"DET /F" );
        Tnew1->Branch("PX"  ,&PX  ,"PX  /F" );
        Tnew1->Branch("PY"  ,&PY  ,"PY  /F" );

	for(Int_t i=0; i<ntot; i++)
	{

		t->GetEntry(i);

		FE=BE=DET=PX=PY=0.;

		for(Int_t j=0;j<16;j++)
		{
			xe=bArray[j]*Calk[0][j]+Calb[0][j];
            if(xe>th) {if(xe>FE) {DET=1; PX=j+1; FE=xe;}}
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=bArray[j]*Calk[0][j]+Calb[0][j];
            if(xe>th) {if(xe>BE) {PY=j-15; BE=xe+BE;}}
		}

         Tnew1->Fill();
    }
   
    fOut1->Write();
    fOut1->Close();



    TString fout2=TString::Format("%d_2.root",run);
	TFile* fOut2 = new TFile(fout2,"RECREATE");
    TTree* Tnew2 = new TTree("tm", "Tree0");
        Tnew2->Branch("FE"  ,&FE  ,"FE  /F" );
        Tnew2->Branch("BE"  ,&BE  ,"BE  /F" );
        Tnew2->Branch("DET" ,&DET ,"DET /F" );
        Tnew2->Branch("PX"  ,&PX  ,"PX  /F" );
        Tnew2->Branch("PY"  ,&PY  ,"PY  /F" );

	for(Int_t i=0; i<ntot; i++)
	{

		t->GetEntry(i);

		FE=BE=DET=PX=PY=0.;

		for(Int_t j=0;j<16;j++)
		{
			xe=cArray[j]*Calk[1][j]+Calb[1][j];
		    if(xe>th) {if(xe>FE) {DET=2; PX=j+1; FE=xe;}}
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=cArray[j]*Calk[1][j]+Calb[1][j];
            if(xe>th) {if(xe>BE) {PY=j-15; BE=xe+BE;}}
		}

         Tnew2->Fill();
    }
   
    fOut2->Write();
    fOut2->Close();

    
    TString fout3=TString::Format("%d_3.root",run);
	TFile* fOut3 = new TFile(fout3,"RECREATE");
    TTree* Tnew3 = new TTree("tm", "Tree0");
        Tnew3->Branch("FE"  ,&FE  ,"FE  /F" );
        Tnew3->Branch("BE"  ,&BE  ,"BE  /F" );
        Tnew3->Branch("DET" ,&DET ,"DET /F" );
        Tnew3->Branch("PX"  ,&PX  ,"PX  /F" );
        Tnew3->Branch("PY"  ,&PY  ,"PY  /F" );

	for(Int_t i=0; i<ntot; i++)
	{

		t->GetEntry(i);

		FE=BE=DET=PX=PY=0.;

		for(Int_t j=0;j<16;j++)
		{
			xe=dArray[j]*Calk[2][j]+Calb[2][j];
		    if(xe>th) {if(xe>FE) {DET=3; PX=j+1; FE=xe+FE;}}
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=dArray[j]*Calk[2][j]+Calb[2][j];
            if(xe>th) {if(xe>BE) {PY=j-15; BE=xe+BE;}}
		}

         Tnew3->Fill();
    }
   
    fOut3->Write();
    fOut3->Close();


    
    TString fout4=TString::Format("%d_4.root",run);
	TFile* fOut4 = new TFile(fout4,"RECREATE");
    TTree* Tnew4 = new TTree("tm", "Tree0");
        Tnew4->Branch("FE"  ,&FE  ,"FE  /F" );
        Tnew4->Branch("BE"  ,&BE  ,"BE  /F" );
        Tnew4->Branch("DET" ,&DET ,"DET /F" );
        Tnew4->Branch("PX"  ,&PX  ,"PX  /F" );
        Tnew4->Branch("PY"  ,&PY  ,"PY  /F" );

	for(Int_t i=0; i<ntot; i++)
	{

		t->GetEntry(i);

		FE=BE=DET=PX=PY=0.;

		for(Int_t j=0;j<16;j++)
		{
			xe=eArray[j]*Calk[3][j]+Calb[3][j];
		    if(xe>th) {if(xe>FE) {DET=4; PX=16-j; FE=xe+FE;}}
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=eArray[j]*Calk[3][j]+Calb[3][j];
            if(xe>th) {if(xe>BE) {PY=j-15; BE=xe+BE;}}
		}

         Tnew4->Fill();
    }
   
    fOut4->Write();
    fOut4->Close();



  fRun->Close();
}  
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Offline_146Sm2026_02(int run)
{
    TString fin=TString::Format("2025_146Sm%d.root",run);
	TFile* fRun = new TFile(fin);
	TTree* t = (TTree*)fRun->Get("Tree1");
	
	
	Float_t E1f,E1b,E2f,E2b,E3f,E3b,E4f,E4b,xe;

        Float_t Emin=0.5, Emax=8, th =0.06, thmax=8;
	Long64_t ntot = t->GetEntriesFast();

	UInt_t bArray[32];
	UInt_t cArray[32];
	UInt_t dArray[32];
	UInt_t eArray[32];


    Float_t Calk[4][32] = {
        {0.001852, 0.001942, 0.001896, 0.001951, 0.001909, 0.001878, 0.001766, 0.001860, 0.001786, 0.001865, 0.001826, 0.001869, 0.001848, 0.001909, 0.001848, 0.001914, 0.001932, 0.001826, 0.001786, 0.001900, 0.001896, 0.002105, 0.001831, 0.001951, 0.001882, 0.001856, 0.001810, 0.001852, 0.001860, 0.001905, 0.001848, 0.001923},
        {0.001810, 0.001782, 0.001794, 0.001810, 0.001782, 0.001814, 0.001852, 0.001818, 0.001831, 0.001839, 0.001848, 0.001843, 0.001843, 0.001774, 0.001810, 0.001865, 0.001743, 0.001684, 0.001810, 0.001762, 0.001810, 0.001882, 0.001869, 0.001782, 0.001918, 0.001874, 0.001843, 0.001956, 0.001887, 0.001798, 0.001874, 0.001818},
        {0.002402, 0.002094, -0.000319, 0.002597, 0.002299, 0.001882, 0.002210, 0.002222, 0.001735, 0.001923, 0.002417, -0.000298, 0.003687, 0.002477, 0.002286, 0.002073, 0.002094, 0.001831, 0.001928, 0.001699, 0.002133, 0.001856, 0.002367, 0.001782, 0.002105, 0.001774, 0.002010, 0.001891, 0.002597, 0.001629, 0.002083, 0.001656},
        {0.001758, 0.001735, 0.001766, 0.001717, 0.001720, 0.001751, 0.001747, 0.001706, 0.001754, 0.001774, 0.001782, 0.001778, 0.001732, 0.001699, 0.001743, 0.001677, 0.001747, 0.001810, 0.001874, 0.001896, 0.001790, 0.001937, 0.001782, 0.001806, 0.001790, 0.001826, 0.001806, 0.001878, 0.001778, 0.001663, 0.001728, 0.001684}
    };

    Float_t Calb[4][32] = {
        {-0.59630, -0.75534, -0.66351, -0.71805, -0.74368, -0.65540, -0.54834, -0.63814, -0.58393, -0.71142, -0.67945, -0.65981, -0.64758, -0.65776, -0.65497, -0.68134, -0.72464, -0.53516, -0.52857, -0.64893, -0.72796, -0.94947, -0.68375, -0.74927, -0.77082, -0.63759, -0.66425, -0.67963, -0.62326, -0.71619, -0.65681, -0.70385},
        {-0.31493, -0.27884, -0.24933, -0.30769, -0.32873, -0.30930, -0.45556, -0.38727, -0.38719, -0.41471, -0.46282, -0.34654, -0.39263, -0.30599, -0.34389, -0.42797, -0.20654, -0.14737, -0.31674, -0.31013, -0.38009, -0.40188, -0.47290, -0.38931, -0.49017, -0.43747, -0.50138, -0.50954, -0.48113, -0.31371, -0.45059, -0.36909},
        {-1.35135, -0.77906, 3.34505, -1.46234, -1.13563, -0.51671, -0.95912, -1.11333, -0.37570, -0.52500, -1.20483, 3.32935, -3.03226, -1.41300, -1.13600, -0.78549, -0.70785, -0.41098, -0.67181, -0.30998, -1.03787, -0.57262, -1.39172, -0.47305, -0.99789, -0.45854, -0.78593, -0.58156, -1.61818, -0.12953, -0.76458, -0.19959},
        {-0.43516, -0.42603, -0.35585, -0.45150, -0.42925, -0.39475, -0.38952, -0.37612, -0.50702, -0.46918, -0.59777, -0.58133, -0.42944, -0.46115, -0.43660, -0.36478, -0.40000, -0.47964, -0.54052, -0.57630, -0.50201, -0.54140, -0.53185, -0.52460, -0.50201, -0.49498, -0.44153, -0.41878, -0.35022, -0.31185, -0.35335, -0.27874}
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




    TH1F *myHisto_1 = new TH1F("myHisto_1", "E1f", 400, Emin, Emax);
    TH1F *myHisto_2 = new TH1F("myHisto_2", "E1b", 400, Emin, Emax);
    TH1F *myHisto_3 = new TH1F("myHisto_3", "E2f", 400, Emin, Emax);
    TH1F *myHisto_4 = new TH1F("myHisto_4", "E2b", 400, Emin, Emax);
    TH1F *myHisto_5 = new TH1F("myHisto_5", "E3f", 400, Emin, Emax);
    TH1F *myHisto_6 = new TH1F("myHisto_6", "E3b", 400, Emin, Emax);
    TH1F *myHisto_7 = new TH1F("myHisto_7", "E4f", 400, Emin, Emax);
    TH1F *myHisto_8 = new TH1F("myHisto_8", "E4b", 400, Emin, Emax);

    TH2F *myHisto_10 = new TH2F("myHisto_10", "E1f:E1b", 4000, Emin, Emax, 4000, Emin, Emax);
    TH2F *myHisto_11 = new TH2F("myHisto_11", "E2f:E2b", 4000, Emin, Emax, 4000, Emin, Emax);
    TH2F *myHisto_12 = new TH2F("myHisto_12", "E3f:E3b", 4000, Emin, Emax, 4000, Emin, Emax);
    TH2F *myHisto_13 = new TH2F("myHisto_13", "E4f:E4b", 4000, Emin, Emax, 4000, Emin, Emax);

	for(Int_t i=0; i<ntot; i++)
//	for(Int_t i=0; i<2464000; i++)
	{

		t->GetEntry(i);

		E1f=E1b=E2f=E2b=E3f=E3b=E4f=E4b=0.;
        
        Int_t nFired1f=0;
        for(Int_t j=0;j<16;j++)
		{
			xe=bArray[j]*Calk[0][j]+Calb[0][j];
			if(xe>th) { E1f=xe+E1f; nFired1f++; }
		}
		if(nFired1f>=2) E1f=0.;
		Int_t nFired1b=0;
		for(Int_t j=16;j<32;j++)
		{
			xe=bArray[j]*Calk[0][j]+Calb[0][j];
			if(xe>th) { E1b=xe+E1b; nFired1b++; }
		}
		if(nFired1b>=2) E1b=0.;

		Int_t nFired2f=0;
		for(Int_t j=0;j<16;j++)
		{
			xe=cArray[j]*Calk[1][j]+Calb[1][j];
			if(xe>th) { E2f=xe+E2f; nFired2f++; }
		}
		if(nFired2f>=2) E2f=0.;
		Int_t nFired2b=0;
		for(Int_t j=16;j<32;j++)
		{
			xe=cArray[j]*Calk[1][j]+Calb[1][j];
			if(xe>th) { E2b=xe+E2b; nFired2b++; }
		}
		if(nFired2b>=2) E2b=0.;


		Int_t nFired3f=0;
		for(Int_t j=0;j<0;j++)
		{
			xe=dArray[j]*Calk[2][j]+Calb[2][j];
			if(xe>th) { E3f=xe+E3f; nFired3f++; }
		}
		if(nFired3f>=2) E3f=0.;
		Int_t nFired3b=0;
		for(Int_t j=16;j<16;j++)
		{
			xe=dArray[j]*Calk[2][j]+Calb[2][j];
			if(xe>th) { E3b=xe+E3b; nFired3b++; }
		}
		if(nFired3b>=2) E3b=0.;


		Int_t nFired4f=0;
		for(Int_t j=0;j<16;j++)
		{
			xe=eArray[j]*Calk[3][j]+Calb[3][j];
			if(xe>th) { E4f=xe+E4f; nFired4f++; }
		}
		if(nFired4f>=2) E4f=0.;
		Int_t nFired4b=0;
		for(Int_t j=16;j<32;j++)
		{
			xe=eArray[j]*Calk[3][j]+Calb[3][j];
			if(xe>th) { E4b=xe+E4b; nFired4b++; }
		}
		if(nFired4b>=2) E4b=0.;


		if(E1f>th&&E1f<thmax) myHisto_1->Fill(E1f);
		if(E1b>th&&E1b<thmax) myHisto_2->Fill(E1b);
		if(E2f>th&&E2f<thmax) myHisto_3->Fill(E2f);
		if(E2b>th&&E2b<thmax) myHisto_4->Fill(E2b);
		if(E3f>th&&E3f<thmax) myHisto_5->Fill(E3f);
		if(E3b>th&&E3b<thmax) myHisto_6->Fill(E3b);
		if(E4f>th&&E4f<thmax) myHisto_7->Fill(E4f);
		if(E4b>th&&E4b<thmax) myHisto_8->Fill(E4b);


		if(E1f>th&&E1b>th&&E1f<thmax&&E1b<thmax) myHisto_10->Fill(E1f,E1b);
		if(E2f>th&&E2b>th&&E2f<thmax&&E2b<thmax) myHisto_11->Fill(E2f,E2b);
		if(E3f>th&&E3b>th&&E3f<thmax&&E3b<thmax) myHisto_12->Fill(E3f,E3b);
		if(E4f>th&&E4b>th&&E4f<thmax&&E4b<thmax) myHisto_13->Fill(E4f,E4b);





	}
	TCanvas *myc_1 =new TCanvas("myc_1","myc_1",0,0,1200,800);

	myc_1->Divide(4,3);
	myc_1->cd(1);
	myHisto_1->Draw();
	myc_1->cd(2);
	myHisto_2->Draw();
	myc_1->cd(3);
	myHisto_3->Draw();
	myc_1->cd(4);
	myHisto_4->Draw();
	myc_1->cd(5);
	myHisto_5->Draw();
	myc_1->cd(6);
	myHisto_6->Draw();
	myc_1->cd(7);
	myHisto_7->Draw();
	myc_1->cd(8);
	myHisto_8->Draw();
	myc_1->cd(9);
	myHisto_10->Draw();
	myc_1->cd(10);
	myHisto_11->Draw();
	myc_1->cd(11);
	myHisto_12->Draw();
	myc_1->cd(12);
	myHisto_13->Draw();


}

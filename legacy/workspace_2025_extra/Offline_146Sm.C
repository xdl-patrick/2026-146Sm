#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Offline_146Sm(int run)
{
    TString fin=TString::Format("146Sm%d.root",run);
	TFile* fRun = new TFile(fin);
	TTree* t = (TTree*)fRun->Get("Tree1");
	
	
	Float_t E1f,E1b,E2f,E2b,E3f,E3b,E4f,E4b,xe;

        Float_t Emin=0.5, Emax=8, th =0.5, thmax=8;
	Long64_t ntot = t->GetEntriesFast();

	UInt_t bArray[32];
	UInt_t cArray[32];
	UInt_t dArray[32];
	UInt_t eArray[32];


Float_t Calk[4][32]=
	{
         {0.001642774,0.001651835,0.001608468,0.001641367,0.001576684,0.001640239,0.002455224,0.001601934,
          0.001587938,0.001627246,0.001611986,0.00168047,0.001624153,0.001670524,0.001636543,0.001641251,
          0.001749851,0.001684131,0.001678819,0.002428388,0.003890221,0.001805366,0.001711475,0.001683147,
          0.001648897,0.002115124,0.001629071,0,0.001885935,0.001722358,0.001649225,0.001671942},
         {0.00158142, 0.00157801, 0.00163209, 0.00160516, 0.00158022, 0.00166706, 0.00158586, 0.00161678, 
         0.00165936, 0.00167786, 0.00158946, 0.00186188, 0.00163951, 0.00159054, 0.00160591, 0.00156772, 
         0.00165706, 0.0016083, 0.00167759, 0.00161145, 0.00166573, 0.00165128, 0.00165605, 0.00161456, 
         0.0016445, 0.00158094, 0.00161622, 0.00163819, 0.0016396, 0.00166783, 0.00163079, 0.00162695},
         {0.00165951, 0.00168181, 0.00166228, 0.00167251, 0.00163437, 0.00167288, 0.00168972, 0.00162929, 
          0.00163325, 0.00164932, 0.00166735, 0.0017029, 0.00163851, 0.00165474, 0.00165569, 0.00165541, 
          0.00164467, 0.00163051, 0.00162451, 0.00168453, 0.0016282, 0.00165736, 0.00164959, 0.00165858, 
          0.00164866, 0.00167349, 0.00170528, 0.00169673, 0.00170055, 0.00171637, 0.00167252, 0.00158448},
         {0.00157584, 0.00154948, 0.00160235, 0.00153856, 0.00154766, 0.00156195, 0.00157738, 0.00152705, 
          0.00153825, 0.00157905, 0.0015297, 0.00153771, 0.00156201, 0.00150842, 0.00155083, 0.00150779, 
          0.00151516, 0.00155465, 0.00155169, 0.00158396, 0.00156146, 0.00162218, 0.00154954, 0.00156571, 
          0.00157495, 0.00155568, 0.00157732, 0.00159474, 0.00157166, 0.00152168, 0.00166922, 0.00154698}
	};

	Float_t Calb[4][32]=
	{
          {-0.328435491,-0.278904899,-0.268454247,-0.315574318,-0.317141427,-0.316017503,-1.464001197,
           -0.298742845,-0.289806405,-0.371922391,-0.366959423,-0.391106758,-0.390996296,-0.340955822,
           -0.370696024,-0.309912598,-0.267508988,-0.238025103,-0.319187386,-1.19789601,-0.778423224,
           -0.381801814,-0.352011135,-0.332303527,-0.38984597,-0.77349919,-0.335810764,0,-0.42093706,
           -0.402912682,-0.317952841,-0.29958964},
          {0.0111801, 0.0151797, -0.0500475, -0.050221, -0.110727, -0.114079, -0.140961, -0.104585, 
           -0.184176, -0.160749, -0.174448, -0.316754, -0.118275, -0.0627836, -0.0596171, 0.000479147, 
           -0.142285, -0.118866, -0.18119, -0.154295, -0.18889, -0.149692, -0.158005, -0.166293, 
           -0.123126, -0.113371, -0.159062, -0.122975, -0.178798, -0.142715, -0.182475, -0.156589},
          {-0.251645, -0.229827, -0.237545, -0.226841, -0.228857, -0.231733, -0.234438, -0.25743, 
           -0.256989, -0.20435, -0.252861, -0.256635, -0.258212, -0.291914, -0.287968, -0.24038, 
           -0.235365, -0.243404, -0.296217, -0.292205, -0.297113, -0.264694, -0.33169, -0.270945, 
           -0.349385, -0.314098, -0.352844, -0.314638, -0.37893, -0.25178, -0.266134, -0.178252},
          {-0.191485, -0.171554, -0.147229, -0.19889, -0.188041, -0.136898, -0.163945, -0.145331, 
           -0.209174, -0.215382, -0.237406, -0.237256, -0.206838, -0.197602, -0.177536, -0.140724, 
           -0.0940485, -0.150724, -0.146914, -0.18587, -0.186787, -0.228114, -0.232219, -0.232274, 
           -0.234501, -0.245322, -0.233652, -0.173991, -0.166045, -0.147787, -0.31058, -0.14875}
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

                for(Int_t j=0;j<16;j++)
		{
			xe=bArray[j]*Calk[0][j]+Calb[0][j];
			if(xe>th) E1f=xe+E1f;
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=bArray[j]*Calk[0][j]+Calb[0][j];
			if(xe>th) E1b=xe+E1b;
		}

		for(Int_t j=0;j<16;j++)
		{
			xe=cArray[j]*Calk[1][j]+Calb[1][j];
			if(xe>th) E2f=xe+E2f;
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=cArray[j]*Calk[1][j]+Calb[1][j];
			if(xe>th) E2b=xe+E2b;
		}


		for(Int_t j=0;j<16;j++)
		{
			xe=dArray[j]*Calk[2][j]+Calb[2][j];
			if(xe>th) E3f=xe+E3f;
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=dArray[j]*Calk[2][j]+Calb[2][j];
			if(xe>th) E3b=xe+E3b;
		}


		for(Int_t j=0;j<16;j++)
		{
			xe=eArray[j]*Calk[3][j]+Calb[3][j];
			if(xe>th) E4f=xe+E4f;
		}
		for(Int_t j=16;j<32;j++)
		{
			xe=eArray[j]*Calk[3][j]+Calb[3][j];
			if(xe>th) E4b=xe+E4b;
		}


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

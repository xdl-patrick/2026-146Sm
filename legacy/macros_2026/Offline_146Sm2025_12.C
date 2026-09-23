#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Offline_146Sm2025_12(int run)
{
    TString fin=TString::Format("2025_146Sm%d.root",run);
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
        0.001928,0.001932,0.001831,0.001932,0.001702,0.001814,0.001887,0.001717,
        0.001802,0.001814,0.001843,0.001860,0.001848,0.001860,0.001848,0.001905,
        0.001887,0.001806,0.001914,0.001928,0.001980,0.001928,0.001766,0.001909,
        0.001914,0.001946,0.001786,0.001961,0.002083,0.002020,0.001839,0.002180,
        0.002180,0.001826,0.001848,0.001802,0.001835,0.001882,0.002532,0.002500,
        0.001743,0.001709,0.001831,0.001743,0.001818,0.001905,0.001843,0.001848,
        0.001874,0.001831,0.001822,0.001852,0.001942,0.001860,0.001923,0.001674,
        0.001831,0.001739,0.001942,0.001810,0.001758,0.001709,0.001724,0.001643,
        0.001709,0.001699,0.001778,0.001810,0.001754,0.001814,0.001770,0.001786,
        0.001874,0.001905,0.001843,0.002020,0.001900,0.001852,0.001835,0.001900,
        0.001878,0.001766,0.001810,0.001826,0.002036,0.002036,0.001966,0.001874,
        0.001878,0.001970,0.001905,0.002439,0.001966,0.001990,0.001626,0.001677,
        0.001831,0.001691,0.001852,0.001735,0.001713,0.001782,0.001603,0.001794,
        0.001739,0.001754,0.001702,0.001766,0.001717,0.001646,0.001822,0.001633,
        0.001684,0.001835,0.001826,0.001835,0.001766,0.001966,0.001874,0.001839,
        0.001782,0.001887,0.001758,0.001874,0.001852,0.001674,0.001762,0.001600,

	};

	Float_t Calb[4][32]=
	{

        -0.57735,-0.65121,-0.57757,-0.71884,-0.48681,-0.56508,-0.62830,-0.46867,
        -0.54955,-0.62857,-0.70783,-0.61767,-0.70670,-0.60279,-0.68637,-0.69714,
        -0.43208,-0.40722,-0.55311,-0.51181,-0.63960,-0.62361,-0.56954,-0.64057,
        -0.70431,-0.63942,-0.61429,-0.80784,-0.74583,-0.70101,-0.60782,-0.97112,
        -0.95368,-0.41461,-0.49977,-0.40721,-0.56697,-0.50541,-1.49620,-1.52750,
        -0.37734,-0.27863,-0.48970,-0.29542,-0.52545,-0.65905,-0.52535,-0.54596,
        -0.42998,-0.40549,-0.35444,-0.47037,-0.54369,-0.42791,-0.51346,-0.25272,
        -0.35973,-0.31478,-0.60388,-0.34208,-0.31560,-0.16752,-0.29655,-0.15031,
        -0.12137,-0.09766,-0.11733,-0.29864,-0.29123,-0.28571,-0.31681,-0.46786,
        -0.43934,-0.35619,-0.47005,-0.39596,-0.43230,-0.38889,-0.37431,-0.38290,
        -0.49390,-0.38057,-0.51403,-0.48584,-0.89873,-0.80509,-0.81081,-0.57986,
        -0.64977,-0.71527,-0.60571,-1.34878,-0.73415,-0.60498,-0.14472,-0.25912,
        -0.52632,-0.36110,-0.46111,-0.47115,-0.41199,-0.41604,-0.17876,-0.48969,
        -0.45565,-0.42632,-0.46468,-0.56424,-0.39657,-0.37202,-0.53850,-0.30204,
        -0.31579,-0.53211,-0.52055,-0.50826,-0.46534,-0.66339,-0.60422,-0.52138,
        -0.52472,-0.58679,-0.41758,-0.38314,-0.41667,-0.34644,-0.41586,-0.20160,

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

		for(Int_t j=11;j<16;j++)
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

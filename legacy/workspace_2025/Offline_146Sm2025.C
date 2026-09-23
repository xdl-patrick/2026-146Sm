#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Offline_146Sm2025(int run)
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
        0.001961,0.001956,0.001831,0.001860,0.001802,0.001860,0.001878,0.001802,
        0.001802,0.001843,0.001798,0.001860,0.001814,0.001839,0.001831,0.001790,
        0.001860,0.001806,0.001839,0.001966,0.001909,0.001905,0.001874,0.001932,
        0.001928,0.002128,0.001961,0.001946,0.002204,0.002030,0.001932,0.001990,
        0.001695,0.001717,0.001822,0.001739,0.001720,0.001814,0.000000,0.000000,
        0.001878,0.001928,0.001831,0.002073,0.001942,0.001860,0.001914,0.001831,
        0.001848,0.001778,0.001887,0.001806,0.001860,0.001798,0.001839,0.001818,
        0.001818,0.001774,0.001810,0.001865,0.001839,0.001839,0.001826,0.001810,
        0.001951,0.001975,0.001928,0.001956,0.001865,0.001928,0.001980,0.001970,
        0.001856,0.001865,0.001882,0.001937,0.001887,0.001923,0.001914,0.001882,
        0.001961,0.001975,0.001891,0.002005,0.001942,0.001966,0.001942,0.001942,
        0.001900,0.001946,0.001914,0.001848,0.001869,0.000000,0.000000,0.001732,
        0.001766,0.001786,0.001822,0.001751,0.001754,0.001798,0.001794,0.001732,
        0.001732,0.001782,0.001728,0.001728,0.001774,0.001728,0.001766,0.001720,
        0.001822,0.001874,0.001878,0.001878,0.001818,0.001946,0.001874,0.001951,
        0.001782,0.002041,0.000000,0.001882,0.001869,0.001702,0.001709,0.000000,

	};

	Float_t Calb[4][32]=
	{

        -0.61569,-0.65428,-0.57941,-0.61209,-0.64324,-0.63256,-0.60845,-0.56757,
        -0.56216,-0.67650,-0.62831,-0.63070,-0.66122,-0.56920,-0.65446,-0.52528,
        -0.37953,-0.41445,-0.47356,-0.59066,-0.62148,-0.60381,-0.66417,-0.67440,
        -0.70458,-0.71064,-0.72745,-0.75426,-0.79229,-0.74518,-0.69565,-0.71443,
        -0.11864,-0.13562,-0.19954,-0.20870,-0.28817,-0.29478,-0.00000,-0.00000,
        -0.43568,-0.45976,-0.48970,-0.46632,-0.44078,-0.41116,-0.42297,-0.31396,
        -0.37229,-0.32178,-0.43208,-0.39819,-0.41488,-0.30652,-0.38161,-0.42545,
        -0.33091,-0.35388,-0.41267,-0.41492,-0.42943,-0.33747,-0.42557,-0.39638,
        -0.63805,-0.62321,-0.59084,-0.59951,-0.53054,-0.56578,-0.62178,-0.73300,
        -0.55963,-0.47832,-0.52424,-0.55496,-0.59811,-0.66538,-0.64115,-0.54118,
        -0.62941,-0.70815,-0.64397,-0.70677,-0.72039,-0.67322,-0.73010,-0.64854,
        -0.69074,-0.67251,-0.61435,-0.49792,-0.56075,-0.00000,-0.00000,-0.35844,
        -0.44592,-0.51071,-0.44374,-0.49978,-0.47719,-0.46292,-0.44126,-0.40173,
        -0.46753,-0.47840,-0.50713,-0.51922,-0.50288,-0.52095,-0.48124,-0.44129,
        -0.51663,-0.58173,-0.56526,-0.57465,-0.53818,-0.60049,-0.60422,-0.59122,
        -0.52650,-0.52245,-0.00000,-0.46212,-0.40187,-0.37447,-0.31966,-0.00000,

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

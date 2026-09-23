#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Offline_146Sm(int run)
{
    TString fin=TString::Format("2025_146Sm-8.28%d.root",run);
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
        0.001560,0.001549,0.001539,0.001549,0.001512,0.001555,0.001560,0.001522,
		0.001535,0.001582,0.001514,0.001625,0.001444,0.001624,0.001546,0.001502,
		0.001958,0.001549,0.001607,0.001825,0.001644,0.001667,0.001578,0.001547,
		0.001570,0.001523,0.001549,0.001681,0.001843,0.003175,0.001493,0.001722,
		0.001671,0.001505,0.001527,0.001496,0.001476,0.001567,0.003175,0.001465,
		0.001505,0.001519,0.001482,0.001743,0.001575,0.001501,0.001469,0.001481,
		0.001558,0.001486,0.001598,0.001543,0.001528,0.001591,0.001595,0.001462,
		0.001518,0.001558,0.001556,0.001465,0.001485,0.001688,0.001525,0.001561,
		0.001525,0.001547,0.001510,0.001538,0.001493,0.001501,0.001562,0.001379,
                0.001514,0.001545,0.001537,0.001572,0.001486,0.001525,0.001530,0.001500,
                0.001441,0.001504,0.001469,0.001566,0.001507,0.001495,0.001493,0.001554,
                0.001550,0.001504,0.001563,0.001509,0.001572,0.001555,0.001545,0.001502,
                0.001457,0.001430,0.001461,0.001375,0.001399,0.001445,0.001402,0.001396,
                0.001418,0.001426,0.001351,0.001419,0.001422,0.001360,0.001380,0.001391,
                0.001386,0.001420,0.001398,0.001471,0.001446,0.001483,0.001351,0.001447,
                0.001399,0.001415,0.001459,0.001464,0.001404,0.001281,0.001458,0.001427
	};

	Float_t Calb[4][32]=
	{

		0.045,0.044,0.060,0.062,0.013,0.033,0.036,0.058,0.024,-0.054,
                0.025,-0.066,0.366,-0.067,0.005,0.283,-0.121,0.296,0.047,-0.239,
                0.218,-0.070,-0.020,0.356,0.064,0.290,0.005,-0.073,-0.069,-2.199,
                0.247,0.022,0.187,0.365,0.325,0.342,0.277,0.236,0.291,0.354,0.364,
                0.394,0.287,0.072,0.263,0.360,0.455,0.404,0.245,0.377,0.236,0.317,
                0.308,0.226,0.209,0.443,0.365,0.202,0.240,0.322,0.288,0.146,0.310,
                0.198,0.232,0.250,0.308,0.253,0.286,0.379,0.202,0.645,0.176,0.176,
                0.201,0.190,0.296,0.168,0.156,0.315,0.498,0.208,0.280,0.120,0.155,
                0.313,0.237,0.120,0.020,0.295,0.149,0.351,0.070,0.308,0.177,0.240,
                0.243,0.269,0.379,0.425,0.379,0.289,0.487,0.364,0.242,0.359,0.455,
                0.209,0.323,0.385,0.468,0.300,0.408,0.359,0.433,0.251,0.247,0.289,
                0.525,0.207,0.425,0.299,0.202,0.314,0.456,0.766,0.447,0.294
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

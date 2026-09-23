//每画幅16张图，粗略检查
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Cal15(const int run, const int Cmin, const int Cmax, const int num)
{
   TString fin=TString::Format("2026_146Sm%d.root",run);
	TFile* fRun = new TFile(fin);
	TTree *t = (TTree*)fRun->Get("Tree1");



   UInt_t Bnum=(Cmax-Cmin)/5.;

   TH1F *hx1 =new TH1F("hx1" ,"",Bnum,Cmin,Cmax);
   TH1F *hx2 =new TH1F("hx2" ,"",Bnum,Cmin,Cmax);
   TH1F *hx3 =new TH1F("hx3" ,"",Bnum,Cmin,Cmax);
   TH1F *hx4 =new TH1F("hx4" ,"",Bnum,Cmin,Cmax);
   TH1F *hx5 =new TH1F("hx5" ,"",Bnum,Cmin,Cmax);
   TH1F *hx6 =new TH1F("hx6" ,"",Bnum,Cmin,Cmax);
   TH1F *hx7 =new TH1F("hx7" ,"",Bnum,Cmin,Cmax);
   TH1F *hx8 =new TH1F("hx8" ,"",Bnum,Cmin,Cmax);
   TH1F *hx9 =new TH1F("hx9" ,"",Bnum,Cmin,Cmax);
   TH1F *hx10=new TH1F("hx10","",Bnum,Cmin,Cmax);
   TH1F *hx11=new TH1F("hx11","",Bnum,Cmin,Cmax);
   TH1F *hx12=new TH1F("hx12","",Bnum,Cmin,Cmax);
   TH1F *hx13=new TH1F("hx13","",Bnum,Cmin,Cmax);
   TH1F *hx14=new TH1F("hx14","",Bnum,Cmin,Cmax);
   TH1F *hx15=new TH1F("hx15","",Bnum,Cmin,Cmax);
   TH1F *hx16=new TH1F("hx16","",Bnum,Cmin,Cmax);

   TCanvas *myc_1 =new TCanvas("myc_1","myc_1",0,0,1400,800);
   myc_1->Divide(4,4);
	

if(num== 1   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch0 - (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch1 - (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch2 - (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch3 - (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch4 - (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch5 - (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch6 - (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch7 - (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch8 - (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch9 - (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch10- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch11- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch12- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch13- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch14- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch15- (0) >> hx16"); }
if(num== 2   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch16- (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch17- (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch18- (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch19- (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch20- (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch21- (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch22- (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch23- (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch24- (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch25- (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch26- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch27- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch28- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch29- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch30- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch31- (0) >> hx16"); }
if(num== 3   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch0 - (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch1 - (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch2 - (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch3 - (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch4 - (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch5 - (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch6 - (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch7 - (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch8 - (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch9 - (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch10- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch11- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch12- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch13- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch14- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch15- (0) >> hx16"); }
if(num== 4   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch16- (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch17- (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch18- (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch19- (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch20- (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch21- (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch22- (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch23- (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch24- (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch25- (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch26- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch27- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch28- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch29- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch30- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch31- (0) >> hx16"); }
if(num== 5   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch0 - (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch1 - (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch2 - (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch3 - (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch4 - (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch5 - (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch6 - (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch7 - (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch8 - (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch9 - (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch10- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch11- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch12- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch13- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch14- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch15- (0) >> hx16"); }
if(num== 6   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch16- (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch17- (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch18- (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch19- (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch20- (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch21- (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch22- (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch23- (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch24- (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch25- (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch26- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch27- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch28- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch29- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch30- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch31- (0) >> hx16"); }
if(num== 7   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch0 - (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch1 - (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch2 - (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch3 - (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch4 - (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch5 - (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch6 - (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch7 - (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch8 - (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch9 - (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch10- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch11- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch12- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch13- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch14- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch15- (0) >> hx16"); }
if(num== 8   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch16- (0) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch17- (0) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch18- (0) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch19- (0) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch20- (0) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch21- (0) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch22- (0) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch23- (0) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch24- (0) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch25- (0) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch26- (0) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch27- (0) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch28- (0) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch29- (0) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch30- (0) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch31- (0) >> hx16"); }

}

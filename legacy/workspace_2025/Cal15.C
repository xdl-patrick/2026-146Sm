#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Cal15(const int run, const int Cmin, const int Cmax, const int num)
{
   TString fin=TString::Format("2025_146Sm%d.root",run);
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
	

if(num== 1   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch0 - (3527) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch1 - (3478) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch2 - (3564) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch3 - (3521) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch4 - (3667) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch5 - (3525) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch6 - (2821) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch7 - (3595) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch8 - (3624) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch9 - (3588) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch10- (3616) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch11- (3483) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch12- (3602) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch13- (3473) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch14- (3564) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch15- (3511) >> hx16"); }
if(num== 2   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch16- (3275) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch17- (3384) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch18- (3446) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch19- (2743) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch20- (2814) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch21- (3240) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch22- (3400) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch23- (3446) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch24- (3552) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch25- (2951) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch26- (3563) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch27- (3481) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch28- (3122) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch29- (3408) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch30- (3505) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch31- (3445) >> hx16"); }
if(num== 3   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch0 - (3454) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch1 - (3421) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch2 - (3359) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch3 - (3437) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch4 - (3535) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch5 - (3353) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch6 - (3529) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch7 - (3433) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch8 - (3246) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch9 - (3230) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch10- (3544) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch11- (3133) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch12- (3307) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch13- (3466) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch14- (3430) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch15- (3470) >> hx16"); }
if(num== 4   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch16- (3382) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch17- (3469) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch18- (3360) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch19- (3479) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch20- (3389) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch21- (3394) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch22- (3388) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch23- (3478) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch24- (3396) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch25- (3530) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch26- (3486) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch27- (3414) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch28- (3444) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch29- (3369) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch30- (3468) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch31- (3459) >> hx16"); }
if(num== 5   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch0 - (3454) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch1 - (3389) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch2 - (3430) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch3 - (3399) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch4 - (3480) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch5 - (3402) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch6 - (3376) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch7 - (3511) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch8 - (3502) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch9 - (3435) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch10- (3425) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch11- (3347) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch12- (3493) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch13- (3476) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch14- (3468) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch15- (3449) >> hx16"); }
if(num== 6   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch16- (3460) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch17- (3477) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch18- (3535) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch19- (3409) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch20- (3522) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch21- (3451) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch22- (3494) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch23- (3452) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch24- (3500) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch25- (3432) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch26- (3423) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch27- (3483) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch28- (3436) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch29- (3337) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch30- (3355) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch31- (3559) >> hx16"); }
if(num== 7   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch0 - (3594) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch1 - (3635) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch2 - (3502) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch3 - (3681) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch4 - (3658) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch5 - (3584) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch6 - (3562) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch7 - (3687) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch8 - (3688) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch9 - (3597) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch10- (3730) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch11- (3715) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch12- (3637) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch13- (3755) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch14- (3642) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch15- (3722) >> hx16"); }
if(num== 8   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch16- (3649) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch17- (3601) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch18- (3597) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch19- (3541) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch20- (3615) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch21- (3117) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch22- (3626) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch23- (3467) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch24- (3627) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch25- (3631) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch26- (3592) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch27- (3533) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch28- (3464) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch29- (3664) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch30- (3571) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch31- (3615) >> hx16"); }

}

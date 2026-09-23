#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Cal14(const int run, const int Cmin, const int Cmax, const int num)
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
	

if(num== 1   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch0 - (1402) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch1 - (1419) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch2 - (1405) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch3 - (1393) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch4 - (1437) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch5 - (1414) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch6 - (1443) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch7 - (1418) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch8 - (1447) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch9 - (1454) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch10- (1467) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch11- (1423) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch12- (1433) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch13- (1392) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch14- (1437) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch15- (1401) >> hx16"); }
if(num== 2   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch16- (1410) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch17- (1388) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch18- (1416) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch19- (1394) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch20- (1439) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch21- (1401) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch22- (1466) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch23- (1409) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch24- (1472) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch25- (1421) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch26- (1472) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch27- (1447) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch28- (1410) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch29- (1426) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch30- (1438) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch31- (1406) >> hx16"); }
if(num== 3   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch0 - (1279) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch1 - (1279) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch2 - (1254) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch3 - (1275) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch4 - (1307) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch5 - (1273) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch6 - (1326) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch7 - (1313) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch8 - (1304) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch9 - (1313) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch10- (1333) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch11- (1273) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch12- (1298) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch13- (1300) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch14- (1295) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch15- (1302) >> hx16"); }
if(num== 4   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch16- (1266) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch17- (1275) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch18- (1280) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch19- (1311) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch20- (1315) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch21- (1276) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch22- (1323) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch23- (1341) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch24- (1298) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch25- (1301) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch26- (1357) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch27- (1283) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch28- (1315) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch29- (1287) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch30- (1308) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch31- (1303) >> hx16"); }
if(num== 5   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch0 - (1395) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch1 - (1327) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch2 - (4210) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch3 - (1333) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch4 - (1364) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch5 - (1337) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch6 - (1339) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch7 - (1401) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch8 - (1369) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch9 - (1313) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch10- (1326) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch11- (4455) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch12- (1365) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch13- (1378) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch14- (1372) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch15- (1344) >> hx16"); }
if(num== 6   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch16- (1293) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch17- (1317) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch18- (1386) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch19- (1360) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch20- (1424) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch21- (1386) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch22- (1433) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch23- (1388) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch24- (1424) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch25- (1386) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch26- (1386) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch27- (1365) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch28- (1393) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch29- (13077) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch30- (1327) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch31- (1328) >> hx16"); }
if(num== 7   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch0 - (1385) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch1 - (1398) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch2 - (1334) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch3 - (1428) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch4 - (1412) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch5 - (1368) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch6 - (1368) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch7 - (1393) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch8 - (1429) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch9 - (1392) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch10- (1458) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch11- (1452) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch12- (1403) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch13- (1449) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch14- (1398) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch15- (1410) >> hx16"); }
if(num== 8   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch16- (1374) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch17- (1370) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch18- (1356) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch19- (1359) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch20- (1398) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch21- (1312) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch22- (1421) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch23- (1398) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch24- (1398) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch25- (1366) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch26- (1352) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch27- (1288) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch28- (1322) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch29- (1390) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch30- (1362) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch31- (1353) >> hx16"); }

}

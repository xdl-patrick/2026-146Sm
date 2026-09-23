#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Cal12(const int run, const int run1, const int Cmin, const int Cmax, const int num)
{
   TString fin=TString::Format("2025_146Sm%d.root",run);
	TFile* fRun = new TFile(fin);
	TTree *t = (TTree*)fRun->Get("Tree1");


   TString fin1=TString::Format("2025_146Sm%d.root",run1);
	TFile* fRun1 = new TFile(fin1);
	TTree *t1 = (TTree*)fRun1->Get("Tree1");

   UInt_t Bnum=(Cmax-Cmin)/5.;

   TH1F *hx1 =new TH1F("hx1" ,"",Bnum,Cmin,Cmax);

   TCanvas *myc_1 =new TCanvas("myc_1","myc_1",0,0,800,400);
	

 if(num== 1   ) {   t->SetLineColor(1); t->Draw("adc0ch0 - (1334) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch0 - (1334)","","same");}
 if(num== 2   ) {   t->SetLineColor(1); t->Draw("adc0ch1 - (1357) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch1 - (1357)","","same");}
 if(num== 3   ) {   t->SetLineColor(1); t->Draw("adc0ch2 - (1409) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch2 - (1409)","","same");}
 if(num== 4   ) {   t->SetLineColor(1); t->Draw("adc0ch3 - (1404) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch3 - (1404)","","same");}
 if(num== 5   ) {   t->SetLineColor(1); t->Draw("adc0ch4 - (1467) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch4 - (1467)","","same");}
 if(num== 6   ) {   t->SetLineColor(1); t->Draw("adc0ch5 - (1415) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch5 - (1415)","","same");}
 if(num== 7   ) {   t->SetLineColor(1); t->Draw("adc0ch6 - (1389) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch6 - (1389)","","same");}
 if(num== 8   ) {   t->SetLineColor(1); t->Draw("adc0ch7 - (1425) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch7 - (1425)","","same");}
 if(num== 9   ) {   t->SetLineColor(1); t->Draw("adc0ch8 - (1422) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch8 - (1422)","","same");}
 if(num== 10  ) {   t->SetLineColor(1); t->Draw("adc0ch9 - (1452) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch9 - (1452)","","same");}
 if(num== 11  ) {   t->SetLineColor(1); t->Draw("adc0ch10- (1462) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch10- (1462)","","same");}
 if(num== 12  ) {   t->SetLineColor(1); t->Draw("adc0ch11- (1414) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch11- (1414)","","same");}
 if(num== 13  ) {   t->SetLineColor(1); t->Draw("adc0ch12- (1467) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch12- (1467)","","same");}
 if(num== 14  ) {   t->SetLineColor(1); t->Draw("adc0ch13- (1397) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch13- (1397)","","same");}
 if(num== 15  ) {   t->SetLineColor(1); t->Draw("adc0ch14- (1450) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch14- (1450)","","same");}
 if(num== 16  ) {   t->SetLineColor(1); t->Draw("adc0ch15- (1411) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch15- (1411)","","same");}
 if(num== 17  ) {   t->SetLineColor(1); t->Draw("adc0ch16- (1279) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch16- (1279)","","same");}
 if(num== 18  ) {   t->SetLineColor(1); t->Draw("adc0ch17- (1337) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch17- (1337)","","same");}
 if(num== 19  ) {   t->SetLineColor(1); t->Draw("adc0ch18- (1345) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch18- (1345)","","same");}
 if(num== 20  ) {   t->SetLineColor(1); t->Draw("adc0ch19- (1318) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch19- (1318)","","same");}
 if(num== 21  ) {   t->SetLineColor(1); t->Draw("adc0ch20- (1373) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch20- (1373)","","same");}
 if(num== 22  ) {   t->SetLineColor(1); t->Draw("adc0ch21- (1367) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch21- (1367)","","same");}
 if(num== 23  ) {   t->SetLineColor(1); t->Draw("adc0ch22- (1422) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch22- (1422)","","same");}
 if(num== 24  ) {   t->SetLineColor(1); t->Draw("adc0ch23- (1384) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch23- (1384)","","same");}
 if(num== 25  ) {   t->SetLineColor(1); t->Draw("adc0ch24- (1403) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch24- (1403)","","same");}
 if(num== 26  ) {   t->SetLineColor(1); t->Draw("adc0ch25- (1274) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch25- (1274)","","same");}
 if(num== 27  ) {   t->SetLineColor(1); t->Draw("adc0ch26- (1391) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch26- (1391)","","same");}
 if(num== 28  ) {   t->SetLineColor(1); t->Draw("adc0ch27- (1415) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch27- (1415)","","same");}
 if(num== 29  ) {   t->SetLineColor(1); t->Draw("adc0ch28- (1267) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch28- (1267)","","same");}
 if(num== 30  ) {   t->SetLineColor(1); t->Draw("adc0ch29- (1352) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch29- (1352)","","same");}
 if(num== 31  ) {   t->SetLineColor(1); t->Draw("adc0ch30- (1395) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch30- (1395)","","same");}
 if(num== 32  ) {   t->SetLineColor(1); t->Draw("adc0ch31- (1364) >> hx1"); t1->SetLineColor(2); t1->Draw("adc0ch31- (1364)","","same");}
 if(num== 33  ) {   t->SetLineColor(1); t->Draw("adc1ch0 - (1250) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch0 - (1250)","","same");}
 if(num== 34  ) {   t->SetLineColor(1); t->Draw("adc1ch1 - (1244) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch1 - (1244)","","same");}
 if(num== 35  ) {   t->SetLineColor(1); t->Draw("adc1ch2 - (1207) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch2 - (1207)","","same");}
 if(num== 36  ) {   t->SetLineColor(1); t->Draw("adc1ch3 - (1270) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch3 - (1270)","","same");}
 if(num== 37  ) {   t->SetLineColor(1); t->Draw("adc1ch4 - (1330) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch4 - (1330)","","same");}
 if(num== 38  ) {   t->SetLineColor(1); t->Draw("adc1ch5 - (1265) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch5 - (1265)","","same");}
 if(num== 39  ) {   t->SetLineColor(1); t->Draw("adc1ch6 - (1338) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch6 - (1338)","","same");}
 if(num== 40  ) {   t->SetLineColor(1); t->Draw("adc1ch7 - (1349) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch7 - (1349)","","same");}
 if(num== 41  ) {   t->SetLineColor(1); t->Draw("adc1ch8 - (1297) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch8 - (1297)","","same");}
 if(num== 42  ) {   t->SetLineColor(1); t->Draw("adc1ch9 - (1276) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch9 - (1276)","","same");}
 if(num== 43  ) {   t->SetLineColor(1); t->Draw("adc1ch10- (1360) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch10- (1360)","","same");}
 if(num== 44  ) {   t->SetLineColor(1); t->Draw("adc1ch11- (1190) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch11- (1190)","","same");}
 if(num== 45  ) {   t->SetLineColor(1); t->Draw("adc1ch12- (1257) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch12- (1257)","","same");}
 if(num== 46  ) {   t->SetLineColor(1); t->Draw("adc1ch13- (1296) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch13- (1296)","","same");}
 if(num== 47  ) {   t->SetLineColor(1); t->Draw("adc1ch14- (1266) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch14- (1266)","","same");}
 if(num== 48  ) {   t->SetLineColor(1); t->Draw("adc1ch15- (1264) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch15- (1264)","","same");}
 if(num== 49  ) {   t->SetLineColor(1); t->Draw("adc1ch16- (1284) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch16- (1284)","","same");}
 if(num== 50  ) {   t->SetLineColor(1); t->Draw("adc1ch17- (1306) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch17- (1306)","","same");}
 if(num== 51  ) {   t->SetLineColor(1); t->Draw("adc1ch18- (1289) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch18- (1289)","","same");}
 if(num== 52  ) {   t->SetLineColor(1); t->Draw("adc1ch19- (1328) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch19- (1328)","","same");}
 if(num== 53  ) {   t->SetLineColor(1); t->Draw("adc1ch20- (1298) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch20- (1298)","","same");}
 if(num== 54  ) {   t->SetLineColor(1); t->Draw("adc1ch21- (1283) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch21- (1283)","","same");}
 if(num== 55  ) {   t->SetLineColor(1); t->Draw("adc1ch22- (1295) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch22- (1295)","","same");}
 if(num== 56  ) {   t->SetLineColor(1); t->Draw("adc1ch23- (1334) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch23- (1334)","","same");}
 if(num== 57  ) {   t->SetLineColor(1); t->Draw("adc1ch24- (1282) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch24- (1282)","","same");}
 if(num== 58  ) {   t->SetLineColor(1); t->Draw("adc1ch25- (1327) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch25- (1327)","","same");}
 if(num== 59  ) {   t->SetLineColor(1); t->Draw("adc1ch26- (1333) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch26- (1333)","","same");}
 if(num== 60  ) {   t->SetLineColor(1); t->Draw("adc1ch27- (1295) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch27- (1295)","","same");}
 if(num== 61  ) {   t->SetLineColor(1); t->Draw("adc1ch28- (1321) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch28- (1321)","","same");}
 if(num== 62  ) {   t->SetLineColor(1); t->Draw("adc1ch29- (1271) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch29- (1271)","","same");}
 if(num== 63  ) {   t->SetLineColor(1); t->Draw("adc1ch30- (1328) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch30- (1328)","","same");}
 if(num== 64  ) {   t->SetLineColor(1); t->Draw("adc1ch31- (1324) >> hx1"); t1->SetLineColor(2); t1->Draw("adc1ch31- (1324)","","same");}
 if(num== 65  ) {   t->SetLineColor(1); t->Draw("adc2ch0 - (1352) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch0 - (1352)","","same");}
 if(num== 66  ) {   t->SetLineColor(1); t->Draw("adc2ch1 - (1328) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch1 - (1328)","","same");}
 if(num== 67  ) {   t->SetLineColor(1); t->Draw("adc2ch2 - (1344) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch2 - (1344)","","same");}
 if(num== 68  ) {   t->SetLineColor(1); t->Draw("adc2ch3 - (1329) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch3 - (1329)","","same");}
 if(num== 69  ) {   t->SetLineColor(1); t->Draw("adc2ch4 - (1357) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch4 - (1357)","","same");}
 if(num== 70  ) {   t->SetLineColor(1); t->Draw("adc2ch5 - (1331) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch5 - (1331)","","same");}
 if(num== 71  ) {   t->SetLineColor(1); t->Draw("adc2ch6 - (1324) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch6 - (1324)","","same");}
 if(num== 72  ) {   t->SetLineColor(1); t->Draw("adc2ch7 - (1387) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch7 - (1387)","","same");}
 if(num== 73  ) {   t->SetLineColor(1); t->Draw("adc2ch8 - (1379) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch8 - (1379)","","same");}
 if(num== 74  ) {   t->SetLineColor(1); t->Draw("adc2ch9 - (1329) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch9 - (1329)","","same");}
 if(num== 75  ) {   t->SetLineColor(1); t->Draw("adc2ch10- (1341) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch10- (1341)","","same");}
 if(num== 76  ) {   t->SetLineColor(1); t->Draw("adc2ch11- (1319) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch11- (1319)","","same");}
 if(num== 77  ) {   t->SetLineColor(1); t->Draw("adc2ch12- (1377) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch12- (1377)","","same");}
 if(num== 78  ) {   t->SetLineColor(1); t->Draw("adc2ch13- (1386) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch13- (1386)","","same");}
 if(num== 79  ) {   t->SetLineColor(1); t->Draw("adc2ch14- (1380) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch14- (1380)","","same");}
 if(num== 80  ) {   t->SetLineColor(1); t->Draw("adc2ch15- (1350) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch15- (1350)","","same");}
 if(num== 81  ) {   t->SetLineColor(1); t->Draw("adc2ch16- (1341) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch16- (1341)","","same");}
 if(num== 82  ) {   t->SetLineColor(1); t->Draw("adc2ch17- (1371) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch17- (1371)","","same");}
 if(num== 83  ) {   t->SetLineColor(1); t->Draw("adc2ch18- (1398) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch18- (1398)","","same");}
 if(num== 84  ) {   t->SetLineColor(1); t->Draw("adc2ch19- (1350) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch19- (1350)","","same");}
 if(num== 85  ) {   t->SetLineColor(1); t->Draw("adc2ch20- (1401) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch20- (1401)","","same");}
 if(num== 86  ) {   t->SetLineColor(1); t->Draw("adc2ch21- (1360) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch21- (1360)","","same");}
 if(num== 87  ) {   t->SetLineColor(1); t->Draw("adc2ch22- (1406) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch22- (1406)","","same");}
 if(num== 88  ) {   t->SetLineColor(1); t->Draw("adc2ch23- (1364) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch23- (1364)","","same");}
 if(num== 89  ) {   t->SetLineColor(1); t->Draw("adc2ch24- (1416) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch24- (1416)","","same");}
 if(num== 90  ) {   t->SetLineColor(1); t->Draw("adc2ch25- (1373) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch25- (1373)","","same");}
 if(num== 91  ) {   t->SetLineColor(1); t->Draw("adc2ch26- (1366) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch26- (1366)","","same");}
 if(num== 92  ) {   t->SetLineColor(1); t->Draw("adc2ch27- (1352) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch27- (1352)","","same");}
 if(num== 93  ) {   t->SetLineColor(1); t->Draw("adc2ch28- (1370) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch28- (1370)","","same");}
 if(num== 94  ) {   t->SetLineColor(1); t->Draw("adc2ch29- (1306) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch29- (1306)","","same");}
 if(num== 95  ) {   t->SetLineColor(1); t->Draw("adc2ch30- (1333) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch30- (1333)","","same");}
 if(num== 96  ) {   t->SetLineColor(1); t->Draw("adc2ch31- (1362) >> hx1"); t1->SetLineColor(2); t1->Draw("adc2ch31- (1362)","","same");}
 if(num== 97  ) {   t->SetLineColor(1); t->Draw("adc3ch0 - (1385) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch0 - (1385)","","same");}
 if(num== 98  ) {   t->SetLineColor(1); t->Draw("adc3ch1 - (1406) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch1 - (1406)","","same");}
 if(num== 99  ) {   t->SetLineColor(1); t->Draw("adc3ch2 - (1341) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch2 - (1341)","","same");}
 if(num== 100 ) {   t->SetLineColor(1); t->Draw("adc3ch3 - (1428) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch3 - (1428)","","same");}
 if(num== 101 ) {   t->SetLineColor(1); t->Draw("adc3ch4 - (1412) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch4 - (1412)","","same");}
 if(num== 102 ) {   t->SetLineColor(1); t->Draw("adc3ch5 - (1370) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch5 - (1370)","","same");}
 if(num== 103 ) {   t->SetLineColor(1); t->Draw("adc3ch6 - (1361) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch6 - (1361)","","same");}
 if(num== 104 ) {   t->SetLineColor(1); t->Draw("adc3ch7 - (1387) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch7 - (1387)","","same");}
 if(num== 105 ) {   t->SetLineColor(1); t->Draw("adc3ch8 - (1425) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch8 - (1425)","","same");}
 if(num== 106 ) {   t->SetLineColor(1); t->Draw("adc3ch9 - (1391) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch9 - (1391)","","same");}
 if(num== 107 ) {   t->SetLineColor(1); t->Draw("adc3ch10- (1451) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch10- (1451)","","same");}
 if(num== 108 ) {   t->SetLineColor(1); t->Draw("adc3ch11- (1458) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch11- (1458)","","same");}
 if(num== 109 ) {   t->SetLineColor(1); t->Draw("adc3ch12- (1411) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch12- (1411)","","same");}
 if(num== 110 ) {   t->SetLineColor(1); t->Draw("adc3ch13- (1459) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch13- (1459)","","same");}
 if(num== 111 ) {   t->SetLineColor(1); t->Draw("adc3ch14- (1405) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch14- (1405)","","same");}
 if(num== 112 ) {   t->SetLineColor(1); t->Draw("adc3ch15- (1419) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch15- (1419)","","same");}
 if(num== 113 ) {   t->SetLineColor(1); t->Draw("adc3ch16- (1381) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch16- (1381)","","same");}
 if(num== 114 ) {   t->SetLineColor(1); t->Draw("adc3ch17- (1378) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch17- (1378)","","same");}
 if(num== 115 ) {   t->SetLineColor(1); t->Draw("adc3ch18- (1366) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch18- (1366)","","same");}
 if(num== 116 ) {   t->SetLineColor(1); t->Draw("adc3ch19- (1371) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch19- (1371)","","same");}
 if(num== 117 ) {   t->SetLineColor(1); t->Draw("adc3ch20- (1396) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch20- (1396)","","same");}
 if(num== 118 ) {   t->SetLineColor(1); t->Draw("adc3ch21- (1336) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch21- (1336)","","same");}
 if(num== 119 ) {   t->SetLineColor(1); t->Draw("adc3ch22- (1390) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch22- (1390)","","same");}
 if(num== 120 ) {   t->SetLineColor(1); t->Draw("adc3ch23- (1328) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch23- (1328)","","same");}
 if(num== 121 ) {   t->SetLineColor(1); t->Draw("adc3ch24- (1418) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch24- (1418)","","same");}
 if(num== 122 ) {   t->SetLineColor(1); t->Draw("adc3ch25- (1236) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch25- (1236)","","same");}
 if(num== 123 ) {   t->SetLineColor(1); t->Draw("adc3ch26- (1384) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch26- (1384)","","same");}
 if(num== 124 ) {   t->SetLineColor(1); t->Draw("adc3ch27- (1308) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch27- (1308)","","same");}
 if(num== 125 ) {   t->SetLineColor(1); t->Draw("adc3ch28- (1285) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch28- (1285)","","same");}
 if(num== 126 ) {   t->SetLineColor(1); t->Draw("adc3ch29- (1395) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch29- (1395)","","same");}
 if(num== 127 ) {   t->SetLineColor(1); t->Draw("adc3ch30- (1357) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch30- (1357)","","same");}
 if(num== 128 ) {   t->SetLineColor(1); t->Draw("adc3ch31- (1376) >> hx1"); t1->SetLineColor(2); t1->Draw("adc3ch31- (1376)","","same");}


}

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TRandom.h"

void Cal13(const int run, const int Cmin  , const int Cmax, const int num)
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
	

if(num== 1   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch0 - (1834) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch1 - (1831) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch2 - (1827) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch3 - (1803) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch4 - (1856) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch5 - (1840) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch6 - (1896) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch7 - (1848) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch8 - (1895) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch9 - (1883) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch10- (1905) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch11- (1851) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch12- (1866) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch13- (1811) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch14- (1870) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch15- (1819) >> hx16"); }
if(num== 2   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc0ch16- (1824) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc0ch17- (1826) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc0ch18- (1864) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc0ch19- (1815) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc0ch20- (1861) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc0ch21- (1781) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc0ch22- (1903) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc0ch23- (1819) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc0ch24- (1897) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc0ch25- (1852) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc0ch26- (1914) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc0ch27- (1879) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc0ch28- (1840) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc0ch29- (1846) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc0ch30- (1871) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc0ch31- (1822) >> hx16"); }
if(num== 3   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch0 - (1721) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch1 - (1728) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch2 - (1700) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch3 - (1717) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch4 - (1756) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch5 - (1714) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch6 - (1758) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch7 - (1753) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch8 - (1741) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch9 - (1748) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch10- (1766) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch11- (1707) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch12- (1732) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch13- (1751) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch14- (1737) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch15- (1731) >> hx16"); }
if(num== 4   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc1ch16- (1725) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc1ch17- (1750) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc1ch18- (1722) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc1ch19- (1765) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc1ch20- (1757) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc1ch21- (1701) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc1ch22- (1751) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc1ch23- (1790) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc1ch24- (1715) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc1ch25- (1728) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc1ch26- (1791) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc1ch27- (1692) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc1ch28- (1739) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc1ch29- (1732) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc1ch30- (1735) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc1ch31- (1743) >> hx16"); }
if(num== 5   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch0 - (1728) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch1 - (1709) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch2 - (1706) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch3 - (1641) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch4 - (1712) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch5 - (1762) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch6 - (1701) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch7 - (1761) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch8 - (1830) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch9 - (1729) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch10- (1657) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch11- (1774) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch12- (1582) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch13- (1701) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch14- (1722) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch15- (1730) >> hx16"); }
if(num== 6   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc2ch16- (1675) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc2ch17- (1754) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc2ch18- (1801) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc2ch19- (1831) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc2ch20- (1799) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc2ch21- (1817) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc2ch22- (1771) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc2ch23- (1837) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc2ch24- (1804) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc2ch25- (1837) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc2ch26- (1784) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc2ch27- (1788) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc2ch28- (1701) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc2ch29- (1798) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc2ch30- (1711) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc2ch31- (1811) >> hx16"); }
if(num== 7   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch0 - (1840) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch1 - (1859) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch2 - (1787) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch3 - (1894) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch4 - (1877) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch5 - (1825) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch6 - (1826) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch7 - (1862) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch8 - (1885) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch9 - (1843) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch10- (1907) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch11- (1902) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch12- (1865) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch13- (1920) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch14- (1857) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch15- (1887) >> hx16"); }
if(num== 8   ){   t->SetLineColor(1); myc_1->cd( 1); t->Fit("gaus","adc3ch16- (1832) >> hx1 "); 
                  t->SetLineColor(1); myc_1->cd( 2); t->Fit("gaus","adc3ch17- (1812) >> hx2 "); 
                  t->SetLineColor(1); myc_1->cd( 3); t->Fit("gaus","adc3ch18- (1783) >> hx3 "); 
                  t->SetLineColor(1); myc_1->cd( 4); t->Fit("gaus","adc3ch19- (1781) >> hx4 "); 
                  t->SetLineColor(1); myc_1->cd( 5); t->Fit("gaus","adc3ch20- (1845) >> hx5 "); 
                  t->SetLineColor(1); myc_1->cd( 6); t->Fit("gaus","adc3ch21- (1725) >> hx6 "); 
                  t->SetLineColor(1); myc_1->cd( 7); t->Fit("gaus","adc3ch22- (1870) >> hx7 "); 
                  t->SetLineColor(1); myc_1->cd( 8); t->Fit("gaus","adc3ch23- (1841) >> hx8 "); 
                  t->SetLineColor(1); myc_1->cd( 9); t->Fit("gaus","adc3ch24- (1845) >> hx9 "); 
                  t->SetLineColor(1); myc_1->cd(10); t->Fit("gaus","adc3ch25- (1804) >> hx10"); 
                  t->SetLineColor(1); myc_1->cd(11); t->Fit("gaus","adc3ch26- (1795) >> hx11"); 
                  t->SetLineColor(1); myc_1->cd(12); t->Fit("gaus","adc3ch27- (1714) >> hx12"); 
                  t->SetLineColor(1); myc_1->cd(13); t->Fit("gaus","adc3ch28- (1772) >> hx13"); 
                  t->SetLineColor(1); myc_1->cd(14); t->Fit("gaus","adc3ch29- (1871) >> hx14"); 
                  t->SetLineColor(1); myc_1->cd(15); t->Fit("gaus","adc3ch30- (1825) >> hx15"); 
                  t->SetLineColor(1); myc_1->cd(16); t->Fit("gaus","adc3ch31- (1828) >> hx16"); }

}

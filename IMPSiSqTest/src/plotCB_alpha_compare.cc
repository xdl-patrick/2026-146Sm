// plotCB_alpha_compare.cc — Compare Crystal Ball lineshapes for different alpha values
//   mean=0, sigma=1, n=1; Red alpha=10, Green alpha=1, Yellow alpha=0.5, Blue alpha=0.1

#include <TCanvas.h>
#include <TH1D.h>
#include <TF1.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TStyle.h>
#include <Math/DistFunc.h>

#include <iostream>

void plotCB_alpha_compare(Bool_t bDraw = kTRUE)
{
    if (!bDraw)
        return;

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetFrameLineWidth(2);
    gStyle->SetTickLength(0.02, "XYZ");

    const Double_t dMean = 0.0;
    const Double_t dSigma = 1.0;
    const Double_t dN = 1.0;
    const Double_t dXmin = -20.0;
    const Double_t dXmax = 5.0;

    TCanvas *fC = new TCanvas("cCB", "Crystal Ball alpha comparison", 900, 600);
    fC->SetLeftMargin(0.12);
    fC->SetRightMargin(0.06);
    fC->SetBottomMargin(0.12);
    fC->SetTopMargin(0.10);

    TH1D *fH = new TH1D("hCB", ";x [arb.];y [arb.]", 500, dXmin, dXmax);
    fH->SetMaximum(1.2);
    fH->SetMinimum(0.0);
    fH->GetXaxis()->SetRangeUser(dXmin, dXmax);
    fH->GetXaxis()->SetLabelSize(0.045);
    fH->GetXaxis()->SetTitleSize(0.050);
    fH->GetXaxis()->SetTitleOffset(0.9);
    fH->GetYaxis()->SetLabelSize(0.045);
    fH->GetYaxis()->SetTitleSize(0.050);
    fH->GetYaxis()->SetTitleOffset(0.9);
    fH->GetXaxis()->CenterTitle();
    fH->GetYaxis()->CenterTitle();
    fH->Draw();

    const Int_t nPts = 4;
    Double_t aAlpha[nPts]   = {10.0, 1.0, 0.5, 0.1};
    Color_t aColor[nPts]    = {kRed + 1, kGreen + 2, kYellow + 2, kBlue + 1};
    TString aLabel[nPts]    = {"#alpha = 10", "#alpha = 1", "#alpha = 0.5", "#alpha = 0.1"};

    TF1 *afFunc[nPts];
    for (Int_t i = 0; i < nPts; ++i) {
        TString strName = TString::Format("fCB%d", i);
        afFunc[i] = new TF1(strName,
                             [&](Double_t *dX, Double_t *dP) {
                                  return dP[0] * ROOT::Math::crystalball_function(dX[0], dP[3], dP[4], dP[2], dP[1]);
                             },
                             dXmin, dXmax, 5);
        afFunc[i]->SetParameters(1.0, dMean, dSigma, aAlpha[i], dN);
        afFunc[i]->SetLineColor(aColor[i]);
        afFunc[i]->SetLineWidth(2);
        afFunc[i]->SetLineStyle(1);
        afFunc[i]->Draw("same");
    }

    TLegend *fLeg = new TLegend(0.18, 0.65, 0.40, 0.85);
    fLeg->SetBorderSize(1);
    fLeg->SetFillColor(kWhite);
    fLeg->SetTextSize(0.042);
    for (Int_t i = 0; i < nPts; ++i)
        fLeg->AddEntry(afFunc[i], aLabel[i], "L");
    fLeg->Draw();

    fC->Modified();
    fC->Update();

    fC->SaveAs("plot/cbAlpha_compare.pdf");
    fC->SaveAs("plot/cbAlpha_compare.png");
    std::cout << "Saved: plot/cbAlpha_compare.pdf / .png" << std::endl;
}

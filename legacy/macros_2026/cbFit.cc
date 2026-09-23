// cbFit.cc — Alpha peak fits from CoMPASS histogram data
// 1. Single Crystal Ball (cb)
// 2. Double Gaussian (dg) — shared sigma
// 3. Double Crystal Ball (dcb) — shared sigma, alpha, n
// Usage:
//   root -l -q 'cbFit.cc(71, "all", "adc0ch15")'
//   root -l -q 'cbFit.cc(71, "cb", "adc0ch15")'
//   root -l -q 'cbFit.cc(71, "dg", "adc4ch12")'
//   root -l -q 'cbFit.cc(71, "dcb", "adc0ch15")'

#include <TFile.h>
#include <TH1D.h>
#include <TH1I.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TMath.h>
#include <TSystem.h>
#include <Math/DistFunc.h>
#include <TPad.h>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// --- Nuclear data (keep as-is) ---
static const Double_t g_dE1 = 5442.80; // keV, 13.1%
static const Double_t g_dE2 = 5485.56; // keV, 84.8%
static const Double_t g_dI1 = 0.131;
static const Double_t g_dI2 = 0.848;

namespace {
    void EnsureResolutionCsvSchema(const TString &strCsvPath) {
        if (gSystem->AccessPathName(strCsvPath)) return;
        std::ifstream fCsvIn(strCsvPath.Data());
        if (!fCsvIn.is_open()) {
            std::cerr << "Warning: cannot open " << strCsvPath << " for schema check" << std::endl;
            return;
        }
        std::string strHeader;
        if (!std::getline(fCsvIn, strHeader)) return;
        if (strHeader.find("particle_fluence_per_cm2") != std::string::npos) return;
        std::vector<std::string> vLines;
        std::string strLine;
        while (std::getline(fCsvIn, strLine)) {
            if (!strLine.empty()) vLines.push_back(strLine);
        }
        fCsvIn.close();
        std::ofstream fCsvOut(strCsvPath.Data(), std::ios::trunc);
        if (!fCsvOut.is_open()) {
            std::cerr << "Warning: cannot rewrite " << strCsvPath << " for schema update" << std::endl;
            return;
        }
        fCsvOut << "macro,run_number,fit_type,resolution_percent,resolution_error_percent,particle_fluence_per_cm2\n";
        for (const std::string &strDataLine : vLines)
            fCsvOut << strDataLine << ",\n";
    }

    Bool_t LoadParticleFluence(Int_t /*nRunNumber*/, Double_t &dParticleFluence) {
        dParticleFluence = 0.0;
        return kFALSE;
    }

    void AppendResolutionCsv(const TString &strMacroName, Int_t nRunNumber, const TString &strFitType,
                             Double_t dResol, Double_t dResolErr, Bool_t bHasParticleFluence, Double_t dParticleFluence) {
        gSystem->mkdir("data/resolution", kTRUE);
        const TString strCsvPath = "data/resolution/resolution.csv";
        Bool_t bFileExists = !gSystem->AccessPathName(strCsvPath);
        if (bFileExists) EnsureResolutionCsvSchema(strCsvPath);
        std::ofstream fCsv(strCsvPath.Data(), std::ios::app);
        if (!fCsv.is_open()) {
            std::cerr << "Error: cannot open " << strCsvPath << " for writing" << std::endl;
            return;
        }
        if (!bFileExists)
            fCsv << "macro,run_number,fit_type,resolution_percent,resolution_error_percent,particle_fluence_per_cm2\n";
        fCsv << strMacroName.Data() << "," << nRunNumber << "," << strFitType.Data() << ","
             << std::fixed << std::setprecision(6) << dResol << "," << dResolErr << ",";
        if (bHasParticleFluence) fCsv << dParticleFluence;
        fCsv << "\n";
    }
}

// --- Fit Functions (unchanged) ---
Double_t SingleCB(Double_t *dX, Double_t *dPar) {
    return dPar[0] * ROOT::Math::crystalball_function(dX[0], dPar[3], dPar[4], dPar[2], dPar[1]);
}

Double_t DoubleGauss(Double_t *dX, Double_t *dPar) {
    Double_t g1 = dPar[0] * TMath::Gaus(dX[0], dPar[1], dPar[4]);
    Double_t g2 = dPar[2] * TMath::Gaus(dX[0], dPar[3], dPar[4]);
    return g1 + g2;
}

Double_t DoubleCB(Double_t *dX, Double_t *dPar) {
    Double_t cb1 = dPar[0] * ROOT::Math::crystalball_function(dX[0], dPar[5], dPar[6], dPar[4], dPar[1]);
    Double_t cb2 = dPar[2] * ROOT::Math::crystalball_function(dX[0], dPar[5], dPar[6], dPar[4], dPar[3]);
    return cb1 + cb2;
}

// =====================================================================
// MODIFIED: Added strHistoName parameter, replaced TTree with TH1I read
// =====================================================================
void cbFit(Int_t nRunNumber = 19, TString strFitType = "all",
           TString strHistoName = "adc0ch15",
           Int_t nBins = 1000, Double_t dXmin = 9500, Double_t dXmax = 11500,
           Bool_t bDraw = kTRUE) {

    strFitType.ToLower();
    Bool_t bFitCB  = (strFitType == "cb"  || strFitType == "all");
    Bool_t bFitDG  = (strFitType == "dg"  || strFitType == "all");
    Bool_t bFitDCB = (strFitType == "dcb" || strFitType == "all");

    if (!bFitCB && !bFitDG && !bFitDCB) {
        std::cerr << "Error: unknown fit type '" << strFitType << "'. Use cb, dg, dcb, or all." << std::endl;
        return;
    }

    // --- Locate ROOT file ---
    TString currentDir = gSystem->pwd();
    TString strFilePath = TString::Format("%s/2025_146Sm%d.root", currentDir.Data(), nRunNumber);

    Double_t dParticleFluence = 0.0;
    Bool_t bHasParticleFluence = LoadParticleFluence(nRunNumber, dParticleFluence);

    if (bHasParticleFluence)
        std::cout << "Particle fluence: " << dParticleFluence << " cm^{-2}" << std::endl;
    else
        std::cout << "Particle fluence: Not calculated (mock mode)." << std::endl;

    // --- Open input file ---
    TFile *fFile = TFile::Open(strFilePath);
    if (!fFile || fFile->IsZombie()) {
        std::cerr << "Error: cannot open " << strFilePath << std::endl;
        return;
    }

    // =========================================================
    // NEW: Read TH1I directly instead of TTree::Draw
    // =========================================================
    TH1I *hRaw = (TH1I *)fFile->Get(strHistoName);
    if (!hRaw || hRaw->GetEntries() == 0) {
        std::cerr << "Error: Histogram '" << strHistoName << "' not found or empty in " << strFilePath << std::endl;
        std::cerr << "Available keys:" << std::endl;
        fFile->ls();
        fFile->Close(); delete fFile;
        return;
    }

    // Convert TH1I -> TH1D for fitting precision
    TH1D *hSpec = new TH1D("hSpec", Form("Run %d - %s;Energy [ADC];Counts", nRunNumber, strHistoName.Data()),
                            hRaw->GetNbinsX(), hRaw->GetXaxis()->GetXmin(), hRaw->GetXaxis()->GetXmax());
    for (Int_t i = 1; i <= hRaw->GetNbinsX(); i++) {
        hSpec->SetBinContent(i, hRaw->GetBinContent(i));
        hSpec->SetBinError(i, hRaw->GetBinError(i));
    }
    hSpec->SetDirectory(0); // Detach from file immediately

    Long64_t nEntries = (Long64_t)hRaw->GetEntries();
    Double_t dBinWidth = hSpec->GetBinWidth(1);

    std::cout << "Loaded histogram: " << strHistoName
              << ", Entries: " << nEntries
              << ", Bins: " << hSpec->GetNbinsX()
              << ", Range: [" << hSpec->GetXaxis()->GetXmin() << ", " << hSpec->GetXaxis()->GetXmax() << "]"
              << std::endl;
    std::cout << "Mean: " << hSpec->GetMean() << ", RMS: " << hSpec->GetRMS() << std::endl;

    // Override dXmin/dXmax to histogram range if user defaults exceed it
    if (dXmin < hSpec->GetXaxis()->GetXmin()) dXmin = hSpec->GetXaxis()->GetXmin();
    if (dXmax > hSpec->GetXaxis()->GetXmax()) dXmax = hSpec->GetXaxis()->GetXmax();

    // --- Gaussian pre-fit for initial estimates (unchanged logic) ---
    Int_t nMaxBin = hSpec->GetMaximumBin();
    Double_t dPeakPos = hSpec->GetBinCenter(nMaxBin);
    Double_t dPeakAmp = hSpec->GetMaximum();
    Double_t dThreshold = 0.35 * dPeakAmp;

    Int_t nBinL = nMaxBin, nBinR = nMaxBin;
    for (Int_t i = nMaxBin; i >= 1; i--) {
        if (hSpec->GetBinContent(i) < dThreshold) { nBinL = i + 1; break; }
    }
    for (Int_t i = nMaxBin; i <= hSpec->GetNbinsX(); i++) {
        if (hSpec->GetBinContent(i) < 0.2 * dThreshold) { nBinR = i - 1; break; }
    }

    Double_t dFitL = hSpec->GetBinCenter(nBinL);
    Double_t dFitR = hSpec->GetBinCenter(nBinR);

    TF1 *fGausPre = new TF1("fGausPre", "gaus", dFitL, dFitR);
    fGausPre->SetParameters(dPeakAmp, dPeakPos, 100);

    TCanvas *cPre = new TCanvas("cPre", "Gaussian pre-fit", 800, 600);
    cPre->cd();
    TH1D *hSpecPre = (TH1D *)hSpec->Clone("hSpecPre");
    hSpecPre->SetTitle(TString::Format("Gaussian pre-fit (35%% peak range);Energy [ADC];Counts [ (%g ADC)^{-1}]", dBinWidth));
    hSpecPre->SetMarkerStyle(20);
    hSpecPre->SetMarkerSize(0.5);
    hSpecPre->GetXaxis()->CenterTitle();
    hSpecPre->GetYaxis()->CenterTitle();
    hSpecPre->Draw();

    hSpecPre->Fit(fGausPre, "RQLEN");
    fGausPre->SetLineColor(kRed);
    fGausPre->SetLineWidth(2);
    fGausPre->Draw("same");

    TLatex *fLblPre = new TLatex();
    fLblPre->SetNDC();
    fLblPre->SetTextSize(0.04);

    Double_t dMean0     = fGausPre->GetParameter(1);
    Double_t dSigma0    = fGausPre->GetParameter(2);
    Double_t dAmp0      = fGausPre->GetParameter(0);
    Double_t dMean0Err  = fGausPre->GetParError(1);
    Double_t dSigma0Err = fGausPre->GetParError(2);

    Double_t dPreFWHM    = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dSigma0;
    Double_t dPreFWHMErr = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dSigma0Err;
    Double_t dPreResol    = dPreFWHM / dMean0 * 100.0;
    Double_t dPreResolErr = dPreResol * TMath::Sqrt(TMath::Power(dSigma0Err / dSigma0, 2) + TMath::Power(dMean0Err / dMean0, 2));

    fLblPre->DrawLatex(0.16, 0.85, TString::Format("#mu = %.1f #pm %.1f", dMean0, dMean0Err));
    fLblPre->DrawLatex(0.16, 0.80, TString::Format("#sigma = %.1f #pm %.1f", dSigma0, dSigma0Err));
    fLblPre->DrawLatex(0.16, 0.75, TString::Format("FWHM = %.1f #pm %.1f", dPreFWHM, dPreFWHMErr));
    fLblPre->DrawLatex(0.16, 0.70, TString::Format("Res = %.2f%% #pm %.2f%%", dPreResol, dPreResolErr));

    cPre->Modified(); cPre->Update();
    gSystem->mkdir("plot", kTRUE);
    cPre->SaveAs(TString::Format("plot/prefit_run%d.pdf", nRunNumber));
    cPre->SaveAs(TString::Format("plot/prefit_run%d.png", nRunNumber));
    std::cout << "Pre-fit plots saved as plot/prefit_run" << nRunNumber << ".pdf / .png" << std::endl;

    delete fGausPre;
    delete hSpecPre;
    delete cPre;

    std::cout << "Gaussian pre-fit: mean=" << dMean0 << " sigma=" << dSigma0 << " amp=" << dAmp0 << std::endl;

    // --- ADC/keV conversion ---
    Double_t dAdcPerKev = dMean0 / g_dE2;
    Double_t dDeltaAdc  = (g_dE2 - g_dE1) * dAdcPerKev;
    std::cout << "ADC/keV: " << dAdcPerKev << ", doublet separation in ADC: " << dDeltaAdc << std::endl;

    // =====================================================================
    // Fit variables declaration (unchanged)
    // =====================================================================
    TF1 *fCb = nullptr, *fDG = nullptr, *fDCB = nullptr;
    Double_t dCbMean=0,dCbSigma=0,dCbAlpha=1.5,dCbN=2.0,dCbAmp=0;
    Double_t dCbMeanErr=0,dCbSigmaErr=0,dCbAlphaErr=0,dCbNErr=0,dCbAmpErr=0;
    Double_t dCbFWHM=0,dCbFWHMErr=0,dCbResol=0,dCbResolErr=0;

    Double_t dDGAmp1=0,dDGMean1=0,dDGAmp2=0,dDGMean2=0,dDGSigma=0;
    Double_t dDGAmp1Err=0,dDGMean1Err=0,dDGAmp2Err=0,dDGMean2Err=0,dDGSigmaErr=0;
    Double_t dDGFWHM=0,dDGFWHMErr=0,dDGResol=0,dDGResolErr=0;

    Double_t dDCBAmp1=0,dDCBMean1=0,dDCBAmp2=0,dDCBMean2=0;
    Double_t dDCBSigma=0,dDCBAlpha=0,dDCBN=0;
    Double_t dDCBAmp1Err=0,dDCBMean1Err=0,dDCBAmp2Err=0,dDCBMean2Err=0;
    Double_t dDCBSigmaErr=0,dDCBAlphaErr=0,dDCBNErr=0;
    Double_t dDCBFWHM=0,dDCBFWHMErr=0,dDCBResol=0,dDCBResolErr=0;

    // =====================================================================
    // Fit 1: Single Crystal Ball (unchanged logic)
    // =====================================================================
    if (bFitCB || bFitDCB) {
        fCb = new TF1("fCb", SingleCB, dXmin, dXmax, 5);
        fCb->SetParNames("Amp","mean","sigma","alpha","n");
        fCb->SetParameter(0, 1.1*dAmp0);
        fCb->SetParameter(1, 1.05*dMean0);
        fCb->SetParameter(2, dSigma0);
        fCb->SetParameter(3, 1.5);
        fCb->SetParameter(4, 2.0);
        fCb->SetParLimits(0, dPeakAmp*0.92, dPeakAmp*2.5);
        fCb->SetParLimits(2, 1.0, dSigma0*3.0);
        fCb->SetParLimits(3, 0.1, 10.0);
        fCb->SetParLimits(4, 0.5, 50.0);

        std::cout << "\n--- Fit 1: Single Crystal Ball ---" << std::endl;
        hSpec->Fit(fCb, "RQLN");

        dCbMean=fCb->GetParameter(1); dCbSigma=fCb->GetParameter(2);
        dCbAlpha=fCb->GetParameter(3); dCbN=fCb->GetParameter(4); dCbAmp=fCb->GetParameter(0);
        dCbMeanErr=fCb->GetParError(1); dCbSigmaErr=fCb->GetParError(2);
        dCbAlphaErr=fCb->GetParError(3); dCbNErr=fCb->GetParError(4); dCbAmpErr=fCb->GetParError(0);

        dCbFWHM=2.0*TMath::Sqrt(2.0*TMath::Log(2.0))*dCbSigma;
        dCbFWHMErr=2.0*TMath::Sqrt(2.0*TMath::Log(2.0))*dCbSigmaErr;
        dCbResol=dCbFWHM/dCbMean*100.0;
        dCbResolErr=dCbResol*TMath::Sqrt(TMath::Power(dCbSigmaErr/dCbSigma,2)+TMath::Power(dCbMeanErr/dCbMean,2));

        std::cout << " mean="<<dCbMean<<"±"<<dCbMeanErr<<" sigma="<<dCbSigma<<"±"<<dCbSigmaErr
                  <<" FWHM="<<dCbFWHM<<" alpha="<<dCbAlpha<<" n="<<dCbN
                  <<" chi2/ndf="<<fCb->GetChisquare()/fCb->GetNDF()<<std::endl;

        if (bFitCB) AppendResolutionCsv("cbFit",nRunNumber,"cb",dCbResol,dCbResolErr,bHasParticleFluence,dParticleFluence);
    }

    // =====================================================================
    // Fit 2: Double Gaussian (unchanged logic)
    // =====================================================================
    if (bFitDG) {
        Double_t dSigmaInit = dCbSigma>5 ? dCbSigma : dSigma0;
        Double_t dMean2Init = dCbMean>0  ? dCbMean  : dMean0;
        Double_t dMean1Init = dMean2Init - dDeltaAdc;

        fDG = new TF1("fDG", DoubleGauss, dXmin, dXmax, 5);
        fDG->SetParNames("Amp1","Mean1","Amp2","Mean2","Sigma");
        fDG->SetParameter(0, dPeakAmp*g_dI1/g_dI2);
        fDG->SetParameter(1, dMean1Init);
        fDG->SetParameter(2, dPeakAmp);
        fDG->SetParameter(3, dMean2Init);
        fDG->SetParameter(4, dSigmaInit);
        fDG->SetParLimits(1, dMean1Init-200, dMean1Init+200);
        fDG->SetParLimits(3, dMean2Init-200, dMean2Init+200);
        fDG->SetParLimits(4, 1.0, dSigmaInit*3.0);

        std::cout << "\n--- Fit 2: Double Gaussian ---" << std::endl;
        hSpec->Fit(fDG, "RQLN");

        dDGAmp1=fDG->GetParameter(0); dDGMean1=fDG->GetParameter(1);
        dDGAmp2=fDG->GetParameter(2); dDGMean2=fDG->GetParameter(3);
        dDGSigma=fDG->GetParameter(4);
        dDGAmp1Err=fDG->GetParError(0); dDGMean1Err=fDG->GetParError(1);
        dDGAmp2Err=fDG->GetParError(2); dDGMean2Err=fDG->GetParError(3);
        dDGSigmaErr=fDG->GetParError(4);

        dDGFWHM=2.0*TMath::Sqrt(2.0*TMath::Log(2.0))*dDGSigma;
        dDGFWHMErr=2.0*TMath::Sqrt(2.0*TMath::Log(2.0))*dDGSigmaErr;
        dDGResol=dDGFWHM/dDGMean2*100.0;
        dDGResolErr=dDGResol*TMath::Sqrt(TMath::Power(dDGSigmaErr/dDGSigma,2)+TMath::Power(dDGMean2Err/dDGMean2,2));

        std::cout << " Mean1="<<dDGMean1<<" Mean2="<<dDGMean2<<" Sigma="<<dDGSigma
                  <<" FWHM="<<dDGFWHM<<" chi2/ndf="<<fDG->GetChisquare()/fDG->GetNDF()<<std::endl;

        AppendResolutionCsv("cbFit",nRunNumber,"dg",dDGResol,dDGResolErr,bHasParticleFluence,dParticleFluence);
    }

    // =====================================================================
    // Fit 3: Double Crystal Ball (unchanged logic)
    // =====================================================================
    if (bFitDCB) {
        Double_t dSigmaInit = dCbSigma>5 ? dCbSigma : dSigma0;
        Double_t dAlphaInit = dCbAlpha>0.1 ? dCbAlpha : 1.5;
        Double_t dNInit     = dCbN>0.5 ? dCbN : 2.0;
        Double_t dMean2Init = dCbMean>0 ? dCbMean : dMean0;
        Double_t dMean1Init = dMean2Init - dDeltaAdc;

        fDCB = new TF1("fDCB", DoubleCB, dXmin, dXmax, 7);
        fDCB->SetParNames("Amp1","Mean1","Amp2","Mean2","Sigma","Alpha","N");
        fDCB->SetParameter(0, dPeakAmp*g_dI1/g_dI2);
        fDCB->SetParameter(1, dMean1Init);
        fDCB->SetParameter(2, dPeakAmp);
        fDCB->SetParameter(3, dMean2Init);
        fDCB->SetParameter(4, dSigmaInit);
        fDCB->SetParameter(5, dAlphaInit);
        fDCB->SetParameter(6, dNInit);
        fDCB->SetParLimits(1, dMean1Init-200, dMean1Init+200);
        fDCB->SetParLimits(3, dMean2Init-200, dMean2Init+200);
        fDCB->SetParLimits(4, 1.0, dSigmaInit*3.0);
        fDCB->SetParLimits(5, 0.1, 10.0);
        fDCB->SetParLimits(6, 0.5, 50.0);

        std::cout << "\n--- Fit 3: Double Crystal Ball ---" << std::endl;
        hSpec->Fit(fDCB, "RQLN");

        dDCBAmp1=fDCB->GetParameter(0); dDCBMean1=fDCB->GetParameter(1);
        dDCBAmp2=fDCB->GetParameter(2); dDCBMean2=fDCB->GetParameter(3);
        dDCBSigma=fDCB->GetParameter(4); dDCBAlpha=fDCB->GetParameter(5); dDCBN=fDCB->GetParameter(6);
        dDCBAmp1Err=fDCB->GetParError(0); dDCBMean1Err=fDCB->GetParError(1);
        dDCBAmp2Err=fDCB->GetParError(2); dDCBMean2Err=fDCB->GetParError(3);
        dDCBSigmaErr=fDCB->GetParError(4); dDCBAlphaErr=fDCB->GetParError(5); dDCBNErr=fDCB->GetParError(6);

        dDCBFWHM=2.0*TMath::Sqrt(2.0*TMath::Log(2.0))*dDCBSigma;
        dDCBFWHMErr=2.0*TMath::Sqrt(2.0*TMath::Log(2.0))*dDCBSigmaErr;
        dDCBResol=dDCBFWHM/dDCBMean2*100.0;
        dDCBResolErr=dDCBResol*TMath::Sqrt(TMath::Power(dDCBSigmaErr/dDCBSigma,2)+TMath::Power(dDCBMean2Err/dDCBMean2,2));

        std::cout << " Mean1="<<dDCBMean1<<" Mean2="<<dDCBMean2<<" Sigma="<<dDCBSigma
                  <<" Alpha="<<dDCBAlpha<<" N="<<dDCBN
                  <<" FWHM="<<dDCBFWHM<<" chi2/ndf="<<fDCB->GetChisquare()/fDCB->GetNDF()<<std::endl;

        AppendResolutionCsv("cbFit",nRunNumber,"dcb",dDCBResol,dDCBResolErr,bHasParticleFluence,dParticleFluence);
    }

    // =====================================================================
    // Drawing & Saving Plots (unchanged logic)
    // =====================================================================
    if (bDraw) {
        gSystem->mkdir("plot", kTRUE);
        TCanvas *cFit = new TCanvas("cFit", "Alpha Peak Fits", 1200, 900);
        cFit->Divide(2, 2);

        cFit->cd(1);
        gPad->SetFillColor(kWhite);
        TLatex *tSummary = new TLatex();
        tSummary->SetNDC(); tSummary->SetTextSize(0.06);
        tSummary->DrawLatex(0.1, 0.85, TString::Format("Run %d — %s", nRunNumber, strHistoName.Data()));
        tSummary->SetTextSize(0.045);
        tSummary->DrawLatex(0.1, 0.70, TString::Format("Entries: %lld", nEntries));
        tSummary->DrawLatex(0.1, 0.60, TString::Format("Pre-fit Res: %.2f%% #pm %.2f%%", dPreResol, dPreResolErr));
        if (bFitCB)  tSummary->DrawLatex(0.1, 0.50, TString::Format("CB Res: %.2f%% #pm %.2f%%", dCbResol, dCbResolErr));
        if (bFitDG)  tSummary->DrawLatex(0.1, 0.40, TString::Format("DG Res: %.2f%% #pm %.2f%%", dDGResol, dDGResolErr));
        if (bFitDCB) tSummary->DrawLatex(0.1, 0.30, TString::Format("DCB Res: %.2f%% #pm %.2f%%", dDCBResol, dDCBResolErr));

        if (bFitCB && fCb) {
            cFit->cd(2); hSpec->Draw();
            fCb->SetLineColor(kBlue); fCb->SetLineWidth(2); fCb->Draw("same");
            TLatex *t = new TLatex(); t->SetNDC(); t->SetTextSize(0.04);
            t->DrawLatex(0.15,0.85,"Single Crystal Ball");
            t->DrawLatex(0.15,0.78,TString::Format("Res=%.2f%%#pm%.2f%%",dCbResol,dCbResolErr));
        }
        if (bFitDG && fDG) {
            cFit->cd(3); hSpec->Draw();
            fDG->SetLineColor(kGreen+2); fDG->SetLineWidth(2); fDG->Draw("same");
            TLatex *t = new TLatex(); t->SetNDC(); t->SetTextSize(0.04);
            t->DrawLatex(0.15,0.85,"Double Gaussian");
            t->DrawLatex(0.15,0.78,TString::Format("Res=%.2f%%#pm%.2f%%",dDGResol,dDGResolErr));
        }
        if (bFitDCB && fDCB) {
            cFit->cd(4); hSpec->Draw();
            fDCB->SetLineColor(kRed); fDCB->SetLineWidth(2); fDCB->Draw("same");
            TLatex *t = new TLatex(); t->SetNDC(); t->SetTextSize(0.04);
            t->DrawLatex(0.15,0.85,"Double Crystal Ball");
            t->DrawLatex(0.15,0.78,TString::Format("Res=%.2f%%#pm%.2f%%",dDCBResol,dDCBResolErr));
        }

        cFit->SaveAs(TString::Format("plot/cbFit_run%d_%s_%s.pdf", nRunNumber, strHistoName.Data(), strFitType.Data()));
        cFit->SaveAs(TString::Format("plot/cbFit_run%d_%s_%s.png", nRunNumber, strHistoName.Data(), strFitType.Data()));
        std::cout << "\nPlots saved to plot/cbFit_run" << nRunNumber << "_" << strHistoName << "_" << strFitType << ".pdf/.png" << std::endl;
        delete cFit;
    }

    // =====================================================================
    // Cleanup
    // =====================================================================
    delete fCb;
    delete fDG;
    delete fDCB;
    delete hSpec; // Already detached via SetDirectory(0) above

    fFile->Close();
    delete fFile;

    std::cout << "\n=== cbFit completed successfully ===" << std::endl;
}
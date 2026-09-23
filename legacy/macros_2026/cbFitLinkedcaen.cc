// cbFitLinkedcaen.cc — 241Am alpha peak fits from CAEN DAQ data
//   1. Single Crystal Ball (cb)
//   2. Linked Double Gaussian (dg) — shared sigma, fixed amp/mean ratios
//   3. Linked Double Crystal Ball (dcb) — shared sigma, alpha, n, fixed amp/mean ratios
// Usage:
//   root -l -q 'src/cbFitLinkedcaen.cc(1)'              — run all fits (data_0001_notwave.root)
//   root -l -q 'src/cbFitLinkedcaen.cc(1, "cb")'        — single Crystal Ball only
//   root -l -q 'src/cbFitLinkedcaen.cc(1, "dg")'        — linked double Gaussian only
//   root -l -q 'src/cbFitLinkedcaen.cc(1, "dcb")'       — linked double Crystal Ball only
// Data: data/A1442B_PKUCAENDAQ/data_xxxx_notwave.root, tree="tree", ch==13, branch="energy"

#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TTree.h>
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

// --- 241Am alpha decay nuclear data
static const Double_t g_dE1 = 5442.80; // keV, 13.1%
static const Double_t g_dE2 = 5485.56; // keV, 84.8%
static const Double_t g_dI1 = 0.131;
static const Double_t g_dI2 = 0.848;
static const Double_t g_dAmpRatio = g_dI1 / g_dI2;
static const Double_t g_dMeanRatio = g_dE1 / g_dE2;
static const Double_t g_dDetectorArea = 1.0;

namespace {
void EnsureResolutionCsvSchema(const TString &strCsvPath)
{
    if (gSystem->AccessPathName(strCsvPath))
        return;

    std::ifstream fCsvIn(strCsvPath.Data());
    if (!fCsvIn.is_open()) {
        std::cerr << "Warning: cannot open " << strCsvPath << " for schema check" << std::endl;
        return;
    }

    std::string strHeader;
    if (!std::getline(fCsvIn, strHeader))
        return;

    if (strHeader.find("particle_fluence_per_cm2") != std::string::npos)
        return;

    std::vector<std::string> vLines;
    std::string strLine;
    while (std::getline(fCsvIn, strLine)) {
        if (!strLine.empty())
            vLines.push_back(strLine);
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

Bool_t LoadParticleFluence(Int_t nRunNumber, Double_t &dParticleFluence)
{
    dParticleFluence = 0.0;

    // TString strInfoPath = TString::Format("data/mystec_compass/run_%d/run_%d_info.txt", nRunNumber, nRunNumber);
    TString strInfoPath = TString::Format("data/A1442B_compass/run_%d/run_%d_info.txt", nRunNumber, nRunNumber);
    FILE *fInfo = fopen(strInfoPath.Data(), "r");
    if (!fInfo) {
        std::cerr << "Warning: cannot open " << strInfoPath << " for fluence calculation" << std::endl;
        return kFALSE;
    }

    Bool_t bInCH0 = kFALSE;
    Bool_t bFoundRealTime = kFALSE;
    Bool_t bFoundInputRate = kFALSE;
    Double_t dRealTimeSec = 0.0;
    Double_t dInputRate = 0.0;
    char acLine[512];

    while (fgets(acLine, sizeof(acLine), fInfo)) {
        char acStripped[512];
        Int_t nLen = strlen(acLine);
        while (nLen > 0 && (acLine[nLen - 1] == '\r' || acLine[nLen - 1] == '\n'))
            nLen--;
        strncpy(acStripped, acLine, nLen);
        acStripped[nLen] = '\0';

        if (strstr(acStripped, "CH0@")) {
            bInCH0 = kTRUE;
            continue;
        }
        if (bInCH0 && strstr(acStripped, "CH1@"))
            break;
        if (!bInCH0)
            continue;

        if (!bFoundRealTime) {
            Int_t nH = 0, nM = 0;
            Double_t dS = 0.0;
            const char *pcSkip = acStripped;
            while (*pcSkip && (*pcSkip == ' ' || *pcSkip == '\t'))
                pcSkip++;
            if (sscanf(pcSkip, "Real time = %d:%d:%lf", &nH, &nM, &dS) == 3) {
                dRealTimeSec = 3600.0 * nH + 60.0 * nM + dS;
                bFoundRealTime = kTRUE;
                continue;
            }
        }

        if (!bFoundInputRate && strstr(acStripped, "Input counts =")) {
            const char *pcRate = strstr(acStripped, "average rate (cps) = ");
            if (pcRate) {
                pcRate += strlen("average rate (cps) = ");
                char acNum[64] = {0};
                Int_t nIdx = 0;
                for (const char *pc = pcRate; *pc && nIdx < (Int_t)sizeof(acNum) - 1; ++pc) {
                    if ((*pc >= '0' && *pc <= '9') || *pc == '.' || *pc == '+' || *pc == '-' || *pc == 'e' || *pc == 'E') {
                        acNum[nIdx++] = *pc;
                    } else if (nIdx > 0) {
                        break;
                    }
                }
                if (nIdx > 0) {
                    dInputRate = atof(acNum);
                    bFoundInputRate = kTRUE;
                }
            }
        }

        if (bFoundRealTime && bFoundInputRate)
            break;
    }

    fclose(fInfo);

    if (!bFoundRealTime || !bFoundInputRate) {
        std::cerr << "Warning: incomplete fluence metadata in " << strInfoPath
                  << " (rt=" << bFoundRealTime << " rate=" << bFoundInputRate << ")" << std::endl;
        return kFALSE;
    }

    dParticleFluence = dRealTimeSec * dInputRate / g_dDetectorArea;
    return kTRUE;
}

void AppendResolutionCsv(const TString &strMacroName,
                         Int_t nRunNumber,
                         const TString &strFitType,
                         Double_t dResol,
                         Double_t dResolErr,
                         Bool_t bHasParticleFluence,
                         Double_t dParticleFluence)
{
    gSystem->mkdir("data/resolution", kTRUE);

    const TString strCsvPath = "data/resolution/resolution.csv";
    Bool_t bFileExists = !gSystem->AccessPathName(strCsvPath);
    if (bFileExists)
        EnsureResolutionCsvSchema(strCsvPath);

    std::ofstream fCsv(strCsvPath.Data(), std::ios::app);
    if (!fCsv.is_open()) {
        std::cerr << "Error: cannot open " << strCsvPath << " for writing" << std::endl;
        return;
    }

    if (!bFileExists) {
        fCsv << "macro,run_number,fit_type,resolution_percent,resolution_error_percent,particle_fluence_per_cm2\n";
    }

    fCsv << strMacroName.Data() << ","
         << nRunNumber << ","
         << strFitType.Data() << ","
         << std::fixed << std::setprecision(6)
         << dResol << ","
         << dResolErr << ",";

    if (bHasParticleFluence)
        fCsv << dParticleFluence;

    fCsv << "\n";
}
}

// --- Single Crystal Ball: amp, mean, sigma, alpha, n
Double_t SingleCB(Double_t *dX, Double_t *dPar)
{
    return dPar[0] * ROOT::Math::crystalball_function(dX[0], dPar[3], dPar[4], dPar[2], dPar[1]);
}

// --- Linked Double Gaussian: amp, mean, shared-sigma
Double_t DoubleGaussLinked(Double_t *dX, Double_t *dPar)
{
    Double_t dAmp2 = dPar[0];
    Double_t dMean2 = dPar[1];
    Double_t dSigma = dPar[2];
    Double_t dAmp1 = dAmp2 * g_dAmpRatio;
    Double_t dMean1 = dMean2 * g_dMeanRatio;
    Double_t g1 = dAmp1 * TMath::Gaus(dX[0], dMean1, dSigma);
    Double_t g2 = dAmp2 * TMath::Gaus(dX[0], dMean2, dSigma);
    return g1 + g2;
}

// --- Linked Double Crystal Ball: amp, mean, shared-sigma, alpha, n
Double_t DoubleCBLinked(Double_t *dX, Double_t *dPar)
{
    Double_t dAmp2 = dPar[0];
    Double_t dMean2 = dPar[1];
    Double_t dSigma = dPar[2];
    Double_t dAlpha = dPar[3];
    Double_t dN = dPar[4];
    Double_t dAmp1 = dAmp2 * g_dAmpRatio;
    Double_t dMean1 = dMean2 * g_dMeanRatio;
    Double_t cb1 = dAmp1 * ROOT::Math::crystalball_function(dX[0], dAlpha, dN, dSigma, dMean1);
    Double_t cb2 = dAmp2 * ROOT::Math::crystalball_function(dX[0], dAlpha, dN, dSigma, dMean2);
    return cb1 + cb2;
}

void cbFitLinkedcaen(Int_t nRunNumber = 9,
                 TString strFitType = "all",
                 Int_t nBins = 500,
                 Double_t dXmin = 18500,
                 Double_t dXmax = 24500,
                 Bool_t bDraw = kTRUE)
{
    strFitType.ToLower();
    Bool_t bFitCB = (strFitType == "cb" || strFitType == "all");
    Bool_t bFitDG = (strFitType == "dg" || strFitType == "all");
    Bool_t bFitDCB = (strFitType == "dcb" || strFitType == "all");

    if (!bFitCB && !bFitDG && !bFitDCB) {
        std::cerr << "Error: unknown fit type '" << strFitType << "'. Use cb, dg, dcb, or all." << std::endl;
        return;
    }

    // TString strFilePath = TString::Format("data/mystec_compass/run_%d/RAW/DataR_run_%d.root", nRunNumber, nRunNumber);
    // TString strFilePath = TString::Format("data/A1442B_compass/run_%d/RAW/DataR_CH0@DT5780N_1068_run_%d.root", nRunNumber, nRunNumber);
    TString strFilePath = TString::Format("data/A1442B_V2745B_PKUCAENDAQ/data_%04d_notwave.root", nRunNumber);
    const char *strTreeName = "tree";
    const char *strBranchName = "energy";
    Double_t dParticleFluence = 0.0;
    Bool_t bHasParticleFluence = LoadParticleFluence(nRunNumber, dParticleFluence);
    if (bHasParticleFluence)
        std::cout << "Particle fluence: " << dParticleFluence << " cm^{-2}" << std::endl;

    // --- Open input file
    TFile *fFile = TFile::Open(strFilePath);
    if (!fFile || fFile->IsZombie())
    {
        std::cerr << "Error: cannot open " << strFilePath << std::endl;
        return;
    }

    // --- Get TTree
    TTree *fTree = (TTree *)fFile->Get(strTreeName);
    if (!fTree)
    {
        std::cerr << "Error: TTree '" << strTreeName << "' not found" << std::endl;
        fFile->Close();
        return;
    }

    if (!fTree->GetBranch(strBranchName))
    {
        std::cerr << "Error: Branch '" << strBranchName << "' not found" << std::endl;
        fFile->Close();
        return;
    }

    // --- Build histogram via TTree::Draw (goff keeps it in memory)
    // Filter: ch==13 (A1442B detector channel)
    Double_t dBinWidth = (dXmax - dXmin) / nBins;
    TString strDrawExpr = TString::Format("%s>>hSpec(%d,%g,%g)", strBranchName, nBins, dXmin, dXmax);
    Long64_t nEntries = fTree->Draw(strDrawExpr, "ch==13", "goff");
    TH1D *hSpec = (TH1D *)gDirectory->Get("hSpec");
    hSpec->SetDirectory(0);
    hSpec->SetTitle(TString::Format("Run %d ^{241}Am spectrum;Energy [ADC];Counts [ (%g ADC)^{-1}]", nRunNumber, dBinWidth));
    std::cout << "Run " << nRunNumber << " — Entries drawn: " << nEntries << std::endl;
    std::cout << "Mean: " << hSpec->GetMean() << ", RMS: " << hSpec->GetRMS() << std::endl;

    // --- Gaussian pre-fit for initial estimates
    Int_t nMaxBin = hSpec->GetMaximumBin();
    Double_t dPeakPos = hSpec->GetBinCenter(nMaxBin);
    Double_t dPeakAmp = hSpec->GetMaximum();
    Double_t dThreshold = 0.35 * dPeakAmp;

    Int_t nBinL = nMaxBin, nBinR = nMaxBin;
    for (Int_t i = nMaxBin; i >= 1; i--) {
        if (hSpec->GetBinContent(i) < dThreshold) { nBinL = i + 1; break; }
    }
    for (Int_t i = nMaxBin; i <= nBins; i++) {
        if (hSpec->GetBinContent(i) < 0.2 * dThreshold) { nBinR = i - 1; break; }
    }
    Double_t dFitL = hSpec->GetBinCenter(nBinL);
    Double_t dFitR = hSpec->GetBinCenter(nBinR);

    TF1 *fGausPre = new TF1("fGausPre", "gaus", dFitL, dFitR);
    fGausPre->SetParameters(dPeakAmp, dPeakPos, 100);

    TCanvas *cPre = new TCanvas("cPreLinked", "Gaussian pre-fit", 800, 600);
    cPre->cd();
    TH1D *hSpecPre = (TH1D*)hSpec->Clone("hSpecPre");
    hSpecPre->SetTitle(TString::Format("Gaussian pre-fit (30%% peak range);Energy [ADC];Counts [ (%g ADC)^{-1}]", dBinWidth));
    hSpecPre->SetMarkerStyle(20);
    hSpecPre->SetMarkerSize(0.5);
    hSpecPre->GetXaxis()->CenterTitle();
    hSpecPre->GetYaxis()->CenterTitle();
    hSpecPre->Draw();
    // hSpecPre->Fit(fGausPre, "RQLEIMN");
    hSpecPre->Fit(fGausPre, "RQLN");
    fGausPre->SetLineColor(kRed);
    fGausPre->SetLineWidth(2);
    fGausPre->Draw("same");

    TLatex *fLblPre = new TLatex();
    fLblPre->SetNDC();
    fLblPre->SetTextSize(0.04);
    Double_t dMean0 = fGausPre->GetParameter(1);
    Double_t dSigma0 = fGausPre->GetParameter(2);
    Double_t dAmp0 = fGausPre->GetParameter(0);
    Double_t dMean0Err = fGausPre->GetParError(1);
    Double_t dSigma0Err = fGausPre->GetParError(2);
    Double_t dPreFWHM = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dSigma0;
    Double_t dPreFWHMErr = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dSigma0Err;
    Double_t dPreResol = dPreFWHM / dMean0 * 100.0;
    Double_t dPreResolErr = dPreResol * TMath::Sqrt(TMath::Power(dSigma0Err / dSigma0, 2) + TMath::Power(dMean0Err / dMean0, 2));
    fLblPre->DrawLatex(0.16, 0.85, TString::Format("#mu = %.1f #pm %.1f", dMean0, dMean0Err));
    fLblPre->DrawLatex(0.16, 0.80, TString::Format("#sigma = %.1f #pm %.1f", dSigma0, dSigma0Err));
    fLblPre->DrawLatex(0.16, 0.75, TString::Format("FWHM = %.1f #pm %.1f", dPreFWHM, dPreFWHMErr));
    fLblPre->DrawLatex(0.16, 0.70, TString::Format("Res = %.2f%% #pm %.2f%%", dPreResol, dPreResolErr));

    cPre->Modified();
    cPre->Update();
    cPre->SaveAs(TString::Format("plot/prefit_linked_run%d.pdf", nRunNumber));
    cPre->SaveAs(TString::Format("plot/prefit_linked_run%d.png", nRunNumber));
    std::cout << "Pre-fit plots saved as plot/prefit_linked_run" << nRunNumber << ".pdf / .png" << std::endl;

    delete fGausPre;
    delete hSpecPre;
    delete cPre;
    std::cout << "Gaussian pre-fit: mean=" << dMean0 << " sigma=" << dSigma0 << " amp=" << dAmp0 << std::endl;

    // --- ADC/keV conversion from single-peak estimate
    Double_t dAdcPerKev = dMean0 / g_dE2;
    std::cout << "ADC/keV: " << dAdcPerKev
              << ", expected linked mean ratio: " << g_dMeanRatio
              << ", expected linked amp ratio: " << g_dAmpRatio << std::endl;

    TF1 *fCb = nullptr;
    Double_t dCbMean = 0, dCbSigma = 0, dCbAlpha = 1.5, dCbN = 2.0, dCbAmp = 0;
    Double_t dCbMeanErr = 0, dCbSigmaErr = 0, dCbAlphaErr = 0, dCbNErr = 0, dCbAmpErr = 0;
    Double_t dCbFWHM = 0, dCbFWHMErr = 0, dCbResol = 0, dCbResolErr = 0;

    TF1 *fDG = nullptr;
    Double_t dDGAmp1 = 0, dDGMean1 = 0, dDGAmp2 = 0, dDGMean2 = 0, dDGSigma = 0;
    Double_t dDGAmp1Err = 0, dDGMean1Err = 0, dDGAmp2Err = 0, dDGMean2Err = 0, dDGSigmaErr = 0;
    Double_t dDGFWHM = 0, dDGFWHMErr = 0, dDGResol = 0, dDGResolErr = 0;

    TF1 *fDCB = nullptr;
    Double_t dDCBAmp1 = 0, dDCBMean1 = 0, dDCBAmp2 = 0, dDCBMean2 = 0;
    Double_t dDCBSigma = 0, dDCBAlpha = 0, dDCBN = 0;
    Double_t dDCBAmp1Err = 0, dDCBMean1Err = 0, dDCBAmp2Err = 0, dDCBMean2Err = 0;
    Double_t dDCBSigmaErr = 0, dDCBAlphaErr = 0, dDCBNErr = 0;
    Double_t dDCBFWHM = 0, dDCBFWHMErr = 0, dDCBResol = 0, dDCBResolErr = 0;

    // =====================================================================
    // Fit 1: Single Crystal Ball
    // =====================================================================
    if (bFitCB || bFitDCB) {
        fCb = new TF1("fCb", SingleCB, dXmin, dXmax, 5);
        fCb->SetParNames("Amp", "mean", "sigma", "alpha", "n");
        fCb->SetParameter(0, 1.1 * dAmp0);
        fCb->SetParameter(1, 1.05 * dMean0);
        fCb->SetParameter(2, dSigma0);
        fCb->SetParameter(3, 1.5);
        fCb->SetParameter(4, 2.0);
        fCb->SetParLimits(0, dPeakAmp * 0.92, dPeakAmp * 2.5);
        fCb->SetParLimits(2, 1.0, dSigma0 * 3.0);
        fCb->SetParLimits(3, 0.1, 10.0);
        fCb->SetParLimits(4, 0.5, 50.0);
        std::cout << "dPeakAmp = " << dPeakAmp << std::endl;

        std::cout << "\n--- Fit 1: Single Crystal Ball ---" << std::endl;
        std::cout << "  Init: mean=" << dMean0 << " sigma=" << dSigma0 << " amp=" << dAmp0
                  << " alpha=1.5 n=2.0" << std::endl;
        hSpec->Fit(fCb, "RQLN");
        // hSpec->Fit(fCb, "RQLEIMN");

        dCbMean = fCb->GetParameter(1);
        dCbSigma = fCb->GetParameter(2);
        dCbAlpha = fCb->GetParameter(3);
        dCbN = fCb->GetParameter(4);
        dCbAmp = fCb->GetParameter(0);
        dCbMeanErr = fCb->GetParError(1);
        dCbSigmaErr = fCb->GetParError(2);
        dCbAlphaErr = fCb->GetParError(3);
        dCbNErr = fCb->GetParError(4);
        dCbAmpErr = fCb->GetParError(0);
        dCbFWHM = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dCbSigma;
        dCbFWHMErr = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dCbSigmaErr;
        dCbResol = dCbFWHM / dCbMean * 100.0;
        dCbResolErr = dCbResol * TMath::Sqrt(TMath::Power(dCbSigmaErr / dCbSigma, 2) + TMath::Power(dCbMeanErr / dCbMean, 2));

        std::cout << "  mean=" << dCbMean << "±" << dCbMeanErr
                  << "  sigma=" << dCbSigma << "±" << dCbSigmaErr
                  << "  amp=" << dCbAmp << "±" << dCbAmpErr
                  << "  FWHM=" << dCbFWHM << " ADC"
                  << "  alpha=" << dCbAlpha << "  n=" << dCbN
                  << "  chi2/ndf=" << fCb->GetChisquare() / fCb->GetNDF()
                  << "  P=" << fCb->GetProb() << std::endl;

        if (bFitCB)
            AppendResolutionCsv("cbFitLinked", nRunNumber, "cb", dCbResol, dCbResolErr, bHasParticleFluence, dParticleFluence);
    }

    // =====================================================================
    // Fit 2: Linked Double Gaussian (shared sigma)
    // =====================================================================
    if (bFitDG) {
        Double_t dSigmaInit = dCbSigma > 5 ? dCbSigma : dSigma0;
        Double_t dMean2Init = dCbMean > 0 ? dCbMean : dMean0;
        Double_t dAmp2Init = dPeakAmp;

        fDG = new TF1("fDG", DoubleGaussLinked, dXmin, dXmax, 3);
        fDG->SetParNames("Amp2", "mean2", "sigma");
        fDG->SetParameter(0, dAmp2Init);
        fDG->SetParameter(1, dMean2Init);
        fDG->SetParameter(2, dSigmaInit);
        fDG->SetParLimits(0, dAmp2Init * 0.75, dAmp2Init * 1.25);
        fDG->SetParLimits(1, dMean2Init * 0.95, dMean2Init * 1.05);
        fDG->SetParLimits(2, dSigmaInit * 0.25, dSigmaInit * 1.25);

        std::cout << "\n--- Fit 2: Linked Double Gaussian (shared sigma) ---" << std::endl;
        std::cout << "  Init: mean2=" << dMean2Init << " sigma=" << dSigmaInit
                  << " amp2=" << dAmp2Init
                  << " -> mean1=" << dMean2Init * g_dMeanRatio
                  << " amp1=" << dAmp2Init * g_dAmpRatio << std::endl;
        hSpec->Fit(fDG, "RQLN");
        // hSpec->Fit(fDG, "RQLEIMN");

        dDGAmp2 = fDG->GetParameter(0);
        dDGMean2 = fDG->GetParameter(1);
        dDGSigma = fDG->GetParameter(2);
        dDGAmp2Err = fDG->GetParError(0);
        dDGMean2Err = fDG->GetParError(1);
        dDGSigmaErr = fDG->GetParError(2);
        dDGAmp1 = dDGAmp2 * g_dAmpRatio;
        dDGMean1 = dDGMean2 * g_dMeanRatio;
        dDGAmp1Err = dDGAmp2Err * g_dAmpRatio;
        dDGMean1Err = dDGMean2Err * g_dMeanRatio;
        dDGFWHM = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dDGSigma;
        dDGFWHMErr = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dDGSigmaErr;
        dDGResol = dDGFWHM / dDGMean2 * 100.0;
        dDGResolErr = dDGResol * TMath::Sqrt(TMath::Power(dDGSigmaErr / dDGSigma, 2) + TMath::Power(dDGMean2Err / dDGMean2, 2));

        std::cout << "  mean2=" << dDGMean2 << "±" << dDGMean2Err
                  << "  mean1=" << dDGMean1 << "±" << dDGMean1Err
                  << "  sigma=" << dDGSigma << "±" << dDGSigmaErr
                  << "  amp2=" << dDGAmp2 << "±" << dDGAmp2Err
                  << "  amp1=" << dDGAmp1 << "±" << dDGAmp1Err
                  << "  FWHM=" << dDGFWHM << " ADC"
                  << "  chi2/ndf=" << fDG->GetChisquare() / fDG->GetNDF()
                  << "  P=" << fDG->GetProb() << std::endl;

        AppendResolutionCsv("cbFitLinked", nRunNumber, "dg", dDGResol, dDGResolErr, bHasParticleFluence, dParticleFluence);
    }

    // =====================================================================
    // Fit 3: Linked Double Crystal Ball (shared sigma, alpha, n)
    // =====================================================================
    if (bFitDCB) {
        Double_t dSigmaInit = dCbSigma > 5 ? dCbSigma : dSigma0;
        Double_t dAlphaInit = dCbAlpha > 0 ? dCbAlpha : 1.5;
        Double_t dNInit = dCbN > 0 ? dCbN : 2.0;
        Double_t dDCBMean2Init = dCbMean > 0 ? dCbMean : dMean0;
        Double_t dDCBAmp2Init = dPeakAmp;

        fDCB = new TF1("fDCB", DoubleCBLinked, dXmin, dXmax, 5);
        fDCB->SetParNames("Amp2", "mean2", "sigma", "alpha", "n");
        fDCB->SetParameter(0, dDCBAmp2Init);
        fDCB->SetParameter(1, dDCBMean2Init);
        fDCB->SetParameter(2, dSigmaInit);
        fDCB->SetParameter(3, dAlphaInit);
        fDCB->SetParameter(4, dNInit);
        fDCB->SetParLimits(0, dDCBAmp2Init * 0.75, dDCBAmp2Init * 1.25);
        fDCB->SetParLimits(1, dDCBMean2Init * 0.95, dDCBMean2Init * 1.05);
        fDCB->SetParLimits(2, dSigmaInit * 0.75, dSigmaInit * 1.25);
        fDCB->SetParLimits(3, 0.3, 10.0);
        fDCB->SetParLimits(4, 0.5, 50.0);

        std::cout << "\n--- Fit 3: Linked Double Crystal Ball (shared sigma, alpha, n) ---" << std::endl;
        std::cout << "  Init: mean2=" << dDCBMean2Init << " sigma=" << dSigmaInit
                  << " alpha=" << dAlphaInit << " n=" << dNInit
                  << " amp2=" << dDCBAmp2Init
                  << " -> mean1=" << dDCBMean2Init * g_dMeanRatio
                  << " amp1=" << dDCBAmp2Init * g_dAmpRatio << std::endl;
        hSpec->Fit(fDCB, "RQLN");
        // hSpec->Fit(fDCB, "RQLEIMN");

        dDCBAmp2 = fDCB->GetParameter(0);
        dDCBMean2 = fDCB->GetParameter(1);
        dDCBSigma = fDCB->GetParameter(2);
        dDCBAlpha = fDCB->GetParameter(3);
        dDCBN = fDCB->GetParameter(4);
        dDCBAmp2Err = fDCB->GetParError(0);
        dDCBMean2Err = fDCB->GetParError(1);
        dDCBSigmaErr = fDCB->GetParError(2);
        dDCBAlphaErr = fDCB->GetParError(3);
        dDCBNErr = fDCB->GetParError(4);
        dDCBAmp1 = dDCBAmp2 * g_dAmpRatio;
        dDCBMean1 = dDCBMean2 * g_dMeanRatio;
        dDCBAmp1Err = dDCBAmp2Err * g_dAmpRatio;
        dDCBMean1Err = dDCBMean2Err * g_dMeanRatio;
        dDCBFWHM = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dDCBSigma;
        dDCBFWHMErr = 2.0 * TMath::Sqrt(2.0 * TMath::Log(2.0)) * dDCBSigmaErr;
        dDCBResol = dDCBFWHM / dDCBMean2 * 100.0;
        dDCBResolErr = dDCBResol * TMath::Sqrt(TMath::Power(dDCBSigmaErr / dDCBSigma, 2) + TMath::Power(dDCBMean2Err / dDCBMean2, 2));

        std::cout << "  amp2=" << dDCBAmp2 << "±" << dDCBAmp2Err
                  << "  amp1=" << dDCBAmp1 << "±" << dDCBAmp1Err
                  << "  mean2=" << dDCBMean2 << "±" << dDCBMean2Err
                  << "  mean1=" << dDCBMean1 << "±" << dDCBMean1Err
                  << "  sigma=" << dDCBSigma << "±" << dDCBSigmaErr
                  << "  alpha=" << dDCBAlpha << "  n=" << dDCBN
                  << "  FWHM=" << dDCBFWHM << " ADC"
                  << "  chi2/ndf=" << fDCB->GetChisquare() / fDCB->GetNDF()
                  << "  P=" << fDCB->GetProb() << std::endl;

        AppendResolutionCsv("cbFitLinked", nRunNumber, "dcb", dDCBResol, dDCBResolErr, bHasParticleFluence, dParticleFluence);
    }

    // =====================================================================
    // Summary comparison
    // =====================================================================
    if (strFitType == "all") {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  Fit Comparison" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  Single CB    : chi2/ndf = " << fCb->GetChisquare() / fCb->GetNDF()
                  << "  P = " << fCb->GetProb() << std::endl;
        std::cout << "  Double Gauss : chi2/ndf = " << fDG->GetChisquare() / fDG->GetNDF()
                  << "  P = " << fDG->GetProb() << std::endl;
        std::cout << "  Double CB    : chi2/ndf = " << fDCB->GetChisquare() / fDCB->GetNDF()
                  << "  P = " << fDCB->GetProb() << std::endl;

        Double_t dDCB_adc_per_kev = dDCBMean2 / g_dE2;
        std::cout << "\n  Double CB ADC/keV: " << dDCB_adc_per_kev << std::endl;
        std::cout << "  Double CB linked mean ratio: " << dDCBMean1 / dDCBMean2
                  << " (" << g_dMeanRatio << " fixed)" << std::endl;
        std::cout << "  Double CB linked intensity ratio: " << dDCBAmp1 / dDCBAmp2
                  << " (" << g_dAmpRatio << " fixed)" << std::endl;
        std::cout << "========================================" << std::endl;
    }

    // =====================================================================
    // Draw
    // =====================================================================
    if (bDraw)
    {
        gStyle->SetOptFit(0);

        Int_t nPads = (strFitType == "all") ? 3 : 1;
        Int_t nCanvasWidth = (strFitType == "all") ? 1200 : 800;
        TCanvas *fCanvas = new TCanvas("cAllLinked", TString::Format("Run %d — 241Am linked fits", nRunNumber), nCanvasWidth, 600);
        if (strFitType == "all")
            fCanvas->Divide(3, 1);

        auto DrawPad = [&](Int_t nPad, TF1 *fFit, const char *strTitle, Color_t eColor) {
            fCanvas->cd(nPad);
            // gPad->SetLeftMargin(0.13);
            // gPad->SetBottomMargin(0.12);
            hSpec->SetTitle(strTitle);
            hSpec->SetMarkerStyle(20);
            hSpec->SetMarkerSize(0.5);
            hSpec->GetXaxis()->CenterTitle();
            hSpec->GetYaxis()->CenterTitle();
            hSpec->Draw();
            fFit->SetLineColor(eColor);
            fFit->SetLineWidth(2);
            fFit->Draw("same");
        };

        if (strFitType == "all") {
            // Pad 1: Single CB
            DrawPad(1, fCb, "Single Crystal Ball", kRed);
            TLatex *fLbl1 = new TLatex();
            fLbl1->SetNDC();
            fLbl1->SetTextSize(0.04);
            fLbl1->DrawLatex(0.16, 0.85, TString::Format("#mu = %.1f #pm %.1f", dCbMean, dCbMeanErr));
            fLbl1->DrawLatex(0.16, 0.80, TString::Format("FWHM = %.1f #pm %.1f", dCbFWHM, dCbFWHMErr));
            fLbl1->DrawLatex(0.16, 0.75, TString::Format("Res = %.2f%% #pm %.2f%%", dCbResol, dCbResolErr));
            fLbl1->DrawLatex(0.16, 0.70, TString::Format("#alpha = %.2f, n = %.2f", dCbAlpha, dCbN));
            fLbl1->DrawLatex(0.16, 0.65, TString::Format("#chi^{2}/ndf = %.2f", fCb->GetChisquare() / fCb->GetNDF()));

            // Pad 2: Linked Double Gauss
            DrawPad(2, fDG, "Linked Double Gaussian", kRed);
            TLatex *fLbl2 = new TLatex();
            fLbl2->SetNDC();
            fLbl2->SetTextSize(0.04);
            fLbl2->DrawLatex(0.16, 0.85, TString::Format("#mu_{1} = %.1f, #mu_{2} = %.1f", dDGMean1, dDGMean2));
            fLbl2->DrawLatex(0.16, 0.80, TString::Format("#sigma = %.1f #pm %.1f", dDGSigma, dDGSigmaErr));
            fLbl2->DrawLatex(0.16, 0.75, TString::Format("FWHM = %.1f #pm %.1f", dDGFWHM, dDGFWHMErr));
            fLbl2->DrawLatex(0.16, 0.70, TString::Format("Res = %.2f%% #pm %.2f%%", dDGResol, dDGResolErr));
            fLbl2->DrawLatex(0.16, 0.65, TString::Format("#chi^{2}/ndf = %.2f", fDG->GetChisquare() / fDG->GetNDF()));

            fCanvas->cd(2);
            TF1 *fDG1 = new TF1("fDG1", "gaus", dXmin, dXmax);
            fDG1->SetParameters(dDGAmp1, dDGMean1, dDGSigma);
            fDG1->SetLineColor(kMagenta);
            fDG1->SetLineStyle(2);
            fDG1->SetLineWidth(1);
            fDG1->Draw("same");

            TF1 *fDG2 = new TF1("fDG2", "gaus", dXmin, dXmax);
            fDG2->SetParameters(dDGAmp2, dDGMean2, dDGSigma);
            fDG2->SetLineColor(kCyan + 1);
            fDG2->SetLineStyle(2);
            fDG2->SetLineWidth(1);
            fDG2->Draw("same");

            // Pad 3: Linked Double CB
            DrawPad(3, fDCB, "Linked Double Crystal Ball", kRed);
            TLatex *fLbl3 = new TLatex();
            fLbl3->SetNDC();
            fLbl3->SetTextSize(0.04);
            fLbl3->DrawLatex(0.16, 0.85, TString::Format("#mu_{1} = %.1f, #mu_{2} = %.1f", dDCBMean1, dDCBMean2));
            fLbl3->DrawLatex(0.16, 0.80, TString::Format("FWHM = %.1f #pm %.1f", dDCBFWHM, dDCBFWHMErr));
            fLbl3->DrawLatex(0.16, 0.75, TString::Format("Res = %.2f%% #pm %.2f%%", dDCBResol, dDCBResolErr));
            fLbl3->DrawLatex(0.16, 0.70, TString::Format("#alpha = %.2f, n = %.2f", dDCBAlpha, dDCBN));
            fLbl3->DrawLatex(0.16, 0.65, TString::Format("#chi^{2}/ndf = %.2f", fDCB->GetChisquare() / fDCB->GetNDF()));

            fCanvas->cd(3);
            TF1 *fDCB1 = new TF1("fDCB1", SingleCB, dXmin, dXmax, 5);
            fDCB1->SetParameters(dDCBAmp1, dDCBMean1, dDCBSigma, dDCBAlpha, dDCBN);
            fDCB1->SetLineColor(kMagenta);
            fDCB1->SetLineStyle(2);
            fDCB1->SetLineWidth(1);
            fDCB1->Draw("same");

            TF1 *fDCB2 = new TF1("fDCB2", SingleCB, dXmin, dXmax, 5);
            fDCB2->SetParameters(dDCBAmp2, dDCBMean2, dDCBSigma, dDCBAlpha, dDCBN);
            fDCB2->SetLineColor(kCyan + 1);
            fDCB2->SetLineStyle(2);
            fDCB2->SetLineWidth(1);
            fDCB2->Draw("same");
        }
        else if (strFitType == "cb") {
            DrawPad(0, fCb, "Single Crystal Ball", kRed);
            TLatex *fLbl = new TLatex();
            fLbl->SetNDC();
            fLbl->SetTextSize(0.04);
            fLbl->DrawLatex(0.16, 0.85, TString::Format("#mu = %.1f #pm %.1f", dCbMean, dCbMeanErr));
            fLbl->DrawLatex(0.16, 0.80, TString::Format("FWHM = %.1f #pm %.1f", dCbFWHM, dCbFWHMErr));
            fLbl->DrawLatex(0.16, 0.75, TString::Format("Res = %.2f%% #pm %.2f%%", dCbResol, dCbResolErr));
            fLbl->DrawLatex(0.16, 0.70, TString::Format("#alpha = %.2f, n = %.2f", dCbAlpha, dCbN));
            fLbl->DrawLatex(0.16, 0.65, TString::Format("#chi^{2}/ndf = %.2f", fCb->GetChisquare() / fCb->GetNDF()));
        }
        else if (strFitType == "dg") {
            DrawPad(0, fDG, "Linked Double Gaussian", kRed);
            TLatex *fLbl = new TLatex();
            fLbl->SetNDC();
            fLbl->SetTextSize(0.04);
            fLbl->DrawLatex(0.16, 0.85, TString::Format("#mu_{1} = %.1f, #mu_{2} = %.1f", dDGMean1, dDGMean2));
            fLbl->DrawLatex(0.16, 0.80, TString::Format("#sigma = %.1f #pm %.1f", dDGSigma, dDGSigmaErr));
            fLbl->DrawLatex(0.16, 0.75, TString::Format("FWHM = %.1f #pm %.1f", dDGFWHM, dDGFWHMErr));
            fLbl->DrawLatex(0.16, 0.70, TString::Format("Res = %.2f%% #pm %.2f%%", dDGResol, dDGResolErr));
            fLbl->DrawLatex(0.16, 0.65, TString::Format("#chi^{2}/ndf = %.2f", fDG->GetChisquare() / fDG->GetNDF()));

            fCanvas->cd(0);
            TF1 *fDG1 = new TF1("fDG1", "gaus", dXmin, dXmax);
            fDG1->SetParameters(dDGAmp1, dDGMean1, dDGSigma);
            fDG1->SetLineColor(kMagenta);
            fDG1->SetLineStyle(2);
            fDG1->SetLineWidth(1);
            fDG1->Draw("same");

            TF1 *fDG2 = new TF1("fDG2", "gaus", dXmin, dXmax);
            fDG2->SetParameters(dDGAmp2, dDGMean2, dDGSigma);
            fDG2->SetLineColor(kCyan + 1);
            fDG2->SetLineStyle(2);
            fDG2->SetLineWidth(1);
            fDG2->Draw("same");
        }
        else if (strFitType == "dcb") {
            DrawPad(0, fDCB, "Linked Double Crystal Ball", kRed);
            TLatex *fLbl = new TLatex();
            fLbl->SetNDC();
            fLbl->SetTextSize(0.04);
            fLbl->DrawLatex(0.16, 0.85, TString::Format("#mu_{1} = %.1f, #mu_{2} = %.1f", dDCBMean1, dDCBMean2));
            fLbl->DrawLatex(0.16, 0.80, TString::Format("FWHM = %.1f #pm %.1f", dDCBFWHM, dDCBFWHMErr));
            fLbl->DrawLatex(0.16, 0.75, TString::Format("Res = %.2f%% #pm %.2f%%", dDCBResol, dDCBResolErr));
            fLbl->DrawLatex(0.16, 0.70, TString::Format("#alpha = %.2f, n = %.2f", dDCBAlpha, dDCBN));
            fLbl->DrawLatex(0.16, 0.65, TString::Format("#chi^{2}/ndf = %.2f", fDCB->GetChisquare() / fDCB->GetNDF()));

            fCanvas->cd(0);
            TF1 *fDCB1 = new TF1("fDCB1", SingleCB, dXmin, dXmax, 5);
            fDCB1->SetParameters(dDCBAmp1, dDCBMean1, dDCBSigma, dDCBAlpha, dDCBN);
            fDCB1->SetLineColor(kMagenta);
            fDCB1->SetLineStyle(2);
            fDCB1->SetLineWidth(1);
            fDCB1->Draw("same");

            TF1 *fDCB2 = new TF1("fDCB2", SingleCB, dXmin, dXmax, 5);
            fDCB2->SetParameters(dDCBAmp2, dDCBMean2, dDCBSigma, dDCBAlpha, dDCBN);
            fDCB2->SetLineColor(kCyan + 1);
            fDCB2->SetLineStyle(2);
            fDCB2->SetLineWidth(1);
            fDCB2->Draw("same");
        }

        fCanvas->Modified();
        fCanvas->Update();
        TString strSuffix = (strFitType == "all") ? "" : TString::Format("_%s", strFitType.Data());
        fCanvas->SaveAs(TString::Format("plot/cbFitLinked_run%d%s.pdf", nRunNumber, strSuffix.Data()));
        fCanvas->SaveAs(TString::Format("plot/cbFitLinked_run%d%s.png", nRunNumber, strSuffix.Data()));
        std::cout << "Plots saved as plot/cbFitLinked_run" << nRunNumber << strSuffix << ".pdf / .png" << std::endl;
    }

    fFile->Close();
}

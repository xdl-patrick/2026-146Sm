// plotResolutionVsFluence.cc — Resolution (%) vs accumulated particle fluence
//   Reads data/resolution/resolution.csv and plots resolution vs fluence
//   with error bars for a specified macro and fit type.
// Usage:
//   root -l -q 'src/plotResolutionVsFluence.cc()'                  — plot all fit types
//   root -l -q 'src/plotResolutionVsFluence.cc("cbFit","cb")'        — cbFit + cb only
//   root -l -q 'src/plotResolutionVsFluence.cc("cbFitLinked","dcb")' — cbFitLinked + dcb

#if !defined(__CINT__) && !defined(__CLING__)
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TPad.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TGaxis.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#endif

// --- Data structure for one CSV row
struct ResoEntry {
    TString strMacro;
    Int_t nRun;
    TString strFitType;
    Double_t dResol;
    Double_t dResolErr;
    Double_t dFluence;     // per-run particle fluence (cm^-2)
    Double_t dCumFluence;  // accumulated fluence (cm^-2), filled after filtering
};

// --- Marker / colour scheme per fit type
struct PlotStyle {
    Int_t eColor;
    Int_t eMarker;
    TString strLabel;
};

static PlotStyle g_aStyle[] = {
    {kRed + 1,     20, "CB  (Crystal Ball)"},
    {kBlue + 1,    21, "DG  (Double Gaussian)"},
    {kGreen + 2,   22, "DCB (Double Crystal Ball)"},
    {kMagenta + 2, 23, "Linked DG"},
    {kCyan + 2,    24, "Linked DCB"},
};

// --- Read all rows from resolution.csv
Bool_t LoadResolutionData(const TString &strCsvPath,
                           std::vector<ResoEntry> &vEntries)
{
    vEntries.clear();

    std::ifstream fCsv(strCsvPath.Data());
    if (!fCsv.is_open()) {
        std::cerr << "Error: cannot open " << strCsvPath << std::endl;
        return kFALSE;
    }

    std::string strLine;
    // Skip header
    if (!std::getline(fCsv, strLine))
        return kFALSE;

    while (std::getline(fCsv, strLine)) {
        // Remove trailing CR
        while (!strLine.empty() && (strLine.back() == '\r' || strLine.back() == '\n'))
            strLine.pop_back();
        if (strLine.empty())
            continue;

        std::istringstream iss(strLine);
        std::string strMacro, strRun, strFitType, strResol, strResolErr, strFluence;
        if (!std::getline(iss, strMacro, ',')) continue;
        if (!std::getline(iss, strRun, ',')) continue;
        if (!std::getline(iss, strFitType, ',')) continue;
        if (!std::getline(iss, strResol, ',')) continue;
        if (!std::getline(iss, strResolErr, ',')) continue;
        if (!std::getline(iss, strFluence, ',')) continue;

        ResoEntry st;
        st.strMacro = strMacro.c_str();
        st.nRun = std::stoi(strRun);
        st.strFitType = strFitType.c_str();
        st.dResol = std::stod(strResol);
        st.dResolErr = std::stod(strResolErr);

        // Skip rows without fluence data
        if (strFluence.empty() || std::stod(strFluence) <= 0.0)
            continue;

        st.dFluence = std::stod(strFluence);
        vEntries.push_back(st);
    }

    fCsv.close();

    std::sort(vEntries.begin(), vEntries.end(),
              [](const ResoEntry &a, const ResoEntry &b) {
                  if (a.nRun != b.nRun) return a.nRun < b.nRun;
                  if (a.strMacro != b.strMacro) return a.strMacro < b.strMacro;
                  return a.strFitType < b.strFitType;
              });

    return kTRUE;
}

// --- Filter entries and compute accumulated fluence (data is already sorted by run)
void FilterEntries(const std::vector<ResoEntry> &vAll,
                  const TString &strMacro,
                  const TString &strFitTypePattern,
                  std::vector<ResoEntry> &vFiltered)
{
    vFiltered.clear();
    for (const auto &st : vAll) {
        Bool_t bMatchMacro = (strMacro == "all") || (st.strMacro == strMacro);
        Bool_t bMatchType = (strFitTypePattern == "all")
                            || (st.strFitType == strFitTypePattern);
        if (bMatchMacro && bMatchType)
            vFiltered.push_back(st);
    }

    // Accumulate fluence in run order
    Double_t dCum = 0.0;
    for (auto &st : vFiltered)
        dCum += st.dFluence, st.dCumFluence = dCum;
}

// --- Return style index for a given fit-type string (0-based)
Int_t FitTypeToStyleIndex(const TString &strFitType)
{
    if (strFitType == "cb")   return 0;
    if (strFitType == "dg")   return 1;
    if (strFitType == "dcb")  return 2;
    if (strFitType == "cbFit")       return 0;
    if (strFitType == "cbFitLinked") return 3;
    return 0;
}

// --- Build a TGraphErrors from filtered entries
TGraphErrors *MakeGraph(const std::vector<ResoEntry> &vEntries,
                        const TString &strLabel,
                        Int_t eColor,
                        Int_t eMarker)
{
    const Int_t nN = vEntries.size();
    if (nN == 0)
        return nullptr;

    Double_t *aX  = new Double_t[nN];
    Double_t *aY  = new Double_t[nN];
    Double_t *aEX = new Double_t[nN];
    Double_t *aEY = new Double_t[nN];

    for (Int_t i = 0; i < nN; ++i) {
        aX[i]  = vEntries[i].dCumFluence;
        aY[i]  = vEntries[i].dResol;
        aEX[i] = 0.0; // fluence has no per-row error currently
        aEY[i] = vEntries[i].dResolErr;
    }

    TGraphErrors *fGr = new TGraphErrors(nN, aX, aY, aEX, aEY);
    fGr->SetName(strLabel);
    fGr->SetTitle(strLabel);
    fGr->SetMarkerColor(eColor);
    fGr->SetMarkerStyle(eMarker);
    fGr->SetLineColor(eColor);
    fGr->SetLineWidth(2);
    fGr->SetMarkerSize(1.4);

    delete[] aX;
    delete[] aY;
    delete[] aEX;
    delete[] aEY;

    return fGr;
}

// =============================================================================
// Main entry
// =============================================================================
void plotResolutionVsFluence(TString strMacro = "cbFitLinked",
                             TString strFitType = "dcb",
                             Bool_t bDraw = kTRUE)
{
    const TString strCsvPath = "data/resolution/resolution.csv";

    // --- Load all CSV rows
    std::vector<ResoEntry> vAll;
    if (!LoadResolutionData(strCsvPath, vAll)) {
        std::cerr << "Error: failed to load " << strCsvPath << std::endl;
        return;
    }
    std::cout << "Loaded " << vAll.size() << " rows with valid fluence." << std::endl;

    // --- Collect unique (macro, fit-type) pairs present in the data
    std::map<std::pair<TString, TString>, Int_t> mPairCount;
    for (const auto &st : vAll)
        mPairCount[{st.strMacro, st.strFitType}]++;

    // --- If user specified a concrete macro+type, check it exists
    if (strMacro != "all" && strFitType != "all") {
        auto it = mPairCount.find({strMacro, strFitType});
        if (it == mPairCount.end()) {
            std::cerr << "Warning: no data for macro='" << strMacro
                      << "', fit_type='" << strFitType << "'" << std::endl;
            return;
        }
        std::cout << "Found " << it->second << " rows for "
                  << strMacro << " / " << strFitType << std::endl;
    }

    // --- Determine which series to plot
    std::vector<ResoEntry> vPlot;
    if (strMacro == "all" && strFitType == "all") {
        vPlot = vAll; // all data, will group by type in legend
    } else {
        FilterEntries(vAll, strMacro, strFitType, vPlot);
    }

    if (vPlot.empty()) {
        std::cerr << "Error: no data matches the selection." << std::endl;
        return;
    }

    // --- Collect all unique series to draw
    std::vector<TGraphErrors*> vGraphs;

    if (strMacro == "all" && strFitType == "all") {
        // Plot cbFit-cb, cbFit-dg, cbFit-dcb, cbFitLinked-cb, cbFitLinked-dg, cbFitLinked-dcb
        const char *aMacros[] = {"cbFit", "cbFitLinked"};
        const char *aTypes[]  = {"cb", "dg", "dcb"};
        Int_t aStyleIdx[]     = {0, 1, 2, 0, 3, 4};
        const char *aLabels[] = {"cbFit / CB", "cbFit / DG", "cbFit / DCB",
                                 "cbFitLinked / CB", "cbFitLinked / DG", "cbFitLinked / DCB"};

        for (Int_t i = 0; i < 6; ++i) {
            std::vector<ResoEntry> vSeries;
            TString strM(aMacros[i % 2]);
            TString strT(aTypes[i / 2]);
            FilterEntries(vAll, strM, strT, vSeries);
            if (vSeries.empty()) continue;

            TGraphErrors *fGr = MakeGraph(vSeries, aLabels[i],
                                          g_aStyle[aStyleIdx[i]].eColor,
                                          g_aStyle[aStyleIdx[i]].eMarker);
            if (fGr) vGraphs.push_back(fGr);
        }
    } else {
        // Single series: determine colour/marker from fit type string
        Int_t nStyleIdx = FitTypeToStyleIndex(strFitType);
        TString strLabel = strMacro + " / " + strFitType;
        TGraphErrors *fGr = MakeGraph(vPlot, strLabel,
                                      g_aStyle[nStyleIdx].eColor,
                                      g_aStyle[nStyleIdx].eMarker);
        if (fGr) vGraphs.push_back(fGr);
    }

    if (vGraphs.empty()) {
        std::cerr << "Error: no graphs could be built." << std::endl;
        return;
    }

    // =========================================================================
    // Draw
    // =========================================================================
    if (!bDraw)
        return;

    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    gStyle->SetPadGridX(1);
    gStyle->SetPadGridY(1);
    gStyle->SetGridColor(kGray + 1);
    gStyle->SetTickLength(0.02, "XYZ");
    gStyle->SetFrameLineWidth(1);

    TCanvas *fCanvas = new TCanvas("cResvsFlu",
                                   "Resolution vs Fluence",
                                   1000, 600);
    fCanvas->SetLeftMargin(0.13);
    fCanvas->SetRightMargin(0.06);
    fCanvas->SetTopMargin(0.10);
    fCanvas->SetBottomMargin(0.13);

    // --- Build TMultiGraph for "all" case, else use single graph
    TMultiGraph *fMg = nullptr;
    TGraphErrors *fSingleGr = nullptr;

    if (vGraphs.size() > 1) {
        fMg = new TMultiGraph();
        for (auto *fGr : vGraphs)
            fMg->Add(fGr, "P");
        fMg->Draw("A");
        fMg->SetTitle(";Particle fluence #Phi [cm^{-2}];Resolution [%]");
    } else {
        fSingleGr = vGraphs[0];
        fSingleGr->Draw("AP");
        fSingleGr->SetTitle(";Particle fluence #Phi [cm^{-2}];Resolution [%]");
    }

    // --- Axis formatting
    TAxis *fXAxis = (fMg ? fMg->GetXaxis() : fSingleGr->GetXaxis());
    TAxis *fYAxis = (fMg ? fMg->GetYaxis() : fSingleGr->GetYaxis());

    fXAxis->SetLabelSize(0.045);
    fXAxis->SetTitleSize(0.050);
    fXAxis->SetTitleOffset(0.9);
    fYAxis->SetLabelSize(0.045);
    fYAxis->SetTitleSize(0.050);
    fYAxis->SetTitleOffset(0.9);
    fXAxis->CenterTitle();
    fYAxis->CenterTitle();

    // --- Grid
    fCanvas->SetGridx(1);
    fCanvas->SetGridy(1);

    // --- Y range — leave headroom
    Double_t dYMin = 0.0;
    Double_t dYMax = (fMg ? fMg->GetYaxis()->GetXmax() : fSingleGr->GetYaxis()->GetXmax());
    if (fMg)
        fMg->GetYaxis()->SetRangeUser(dYMin, dYMax * 1.15);
    else
        fSingleGr->GetYaxis()->SetRangeUser(dYMin, dYMax * 1.15);

    // --- Title
    TLatex *fTitle = new TLatex();
    fTitle->SetNDC();
    fTitle->SetTextSize(0.050);
    fTitle->SetTextFont(42);
    fTitle->DrawLatex(0.13, 0.93,
                       "^{241}Am #alpha resolution vs accumulated fluence");

    // --- Legend
    TLegend *fLeg = new TLegend(0.70, 0.60, 0.93, 0.88);
    fLeg->SetBorderSize(1);
    fLeg->SetFillColor(kWhite);
    fLeg->SetTextSize(0.038);
    if (fMg) {
        for (auto *fGr : vGraphs)
            fLeg->AddEntry(fGr, fGr->GetName(), "P");
    } else {
        fLeg->AddEntry(fSingleGr, fSingleGr->GetName(), "P");
    }
    fLeg->Draw();

    fCanvas->Modified();
    fCanvas->Update();

    // --- Save
    TString strSuffixMacro = (strMacro == "all") ? "all" : strMacro;
    TString strSuffixType  = (strFitType == "all") ? "all" : strFitType;
    TString strSaveBase = TString::Format("plot/resoVsFluence_%s_%s",
                                           strSuffixMacro.Data(),
                                           strSuffixType.Data());
    fCanvas->SaveAs(strSaveBase + ".pdf");
    fCanvas->SaveAs(strSaveBase + ".png");
    std::cout << "Saved: " << strSaveBase << ".pdf / .png" << std::endl;

    // --- Print summary table
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Loaded data points (" << vPlot.size() << " rows)" << std::endl;
    std::cout << "========================================" << std::endl;
    for (const auto &st : vPlot) {
        std::cout << "  run=" << st.nRun
                  << "  Phi=" << st.dFluence << " cm^{-2}"
                  << "  cumPhi=" << st.dCumFluence << " cm^{-2}"
                  << "  Res=" << st.dResol << " #pm " << st.dResolErr
                  << std::endl;
    }
    std::cout << "========================================" << std::endl;
}

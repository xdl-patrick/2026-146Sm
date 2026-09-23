#include <fstream>
#include "TH1D.h"
#include "TFile.h"
#include "TSpectrum.h"

void SpeToRoot(const char* speFile, const char* rootFile)
{
    std::ifstream file(speFile);
    if (!file.is_open()){
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    //check wether there is an old TH1F
    if(gDirectory->FindObject("spectrum")){
        delete
        gDirectory->FindObject("spectrum");
    }
    
    const int nChannels = 16384;
    TH1D* hist = new TH1D("spectrum","SPE Spectrum;Channel;Counts",nChannels,0,nChannels);
    std::string line;
    for (int i = 0; i < 12; i++) {
        std::getline(file, line);
    }
    int channel = 0, count;
    while (file >> count && channel <nChannels){
        hist -> SetBinContent(channel +1, count);
        channel++;
    }

    TFile out(rootFile, "RECREATE");
    hist->Write();
    out.Close();
    delete hist;
} 

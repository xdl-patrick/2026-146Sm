# AGENTS.md — IMPSiSqTest

²⁴¹Am alpha peak fitting from CoMPASS RAW and CAEN DAQ data using Crystal Ball and Gaussian functions. Python diagram tools included.

## Build / Run / Test

No build system — all code runs via interpreter directly. No unit tests, no linter, no formatter. Verification is visual (PDF/PNG plot output) and numerical (chi²/ndf, P-value printed to stdout).

### ROOT Macros — CoMPASS Data (cbFit.cc)

```bash
# Free amplitude/mean ratio fits
root -l -q 'src/cbFit.cc(9)'                         # all fits
root -l -q 'src/cbFit.cc(9, "cb")'                   # single Crystal Ball
root -l -q 'src/cbFit.cc(9, "dg")'                   # Double Gaussian (shared sigma)
root -l -q 'src/cbFit.cc(9, "dcb")'                  # Double Crystal Ball
root -l -q 'src/cbFit.cc(9, "all", 500, 8000, 13000)' # custom binning
root -l -b -q 'src/cbFit.cc(9, "cb", 1000, 8000, 13000, kFALSE)' # batch mode
```

### ROOT Macros — CAEN DAQ Data (cbFitLinkedcaen.cc)

```bash
# Fixed amp/mean ratios from nuclear data (241Am decay branching ratios)
root -l -q 'src/cbFitLinkedcaen.cc(1)'               # run all fits (data_0001_notwave.root)
root -l -q 'src/cbFitLinkedcaen.cc(56, "cb")'        # single CB on run 56
root -l -q 'src/cbFitLinkedcaen.cc(1, "dg")'         # linked double Gaussian
root -l -q 'src/cbFitLinkedcaen.cc(1, "dcb")'        # linked double Crystal Ball
root -l -b -q 'src/cbFitLinkedcaen.cc(1, "all", 500, 9500, 11500, kFALSE)' # batch
```

### ROOT Macros — Analysis & Plotting

```bash
# Resolution vs cumulative fluence (from data/resolution/resolution.csv)
root -l -q 'src/plotResolutionVsFluence.cc()'
root -l -q 'src/plotResolutionVsFluence.cc("cbFit","dcb")'
root -l -b -q 'src/plotResolutionVsFluence.cc("cbFit","dcb",kFALSE)' # print only

# Crystal Ball alpha parameter comparison
root -l -q 'src/plotCB_alpha_compare.cc'
```

### Python Scripts

```bash
# Detector signal chain schematic (outputs to plot/)
/usr/bin/python3 src/draw_chain_diagram.py
```

### Output Files

| File | Description |
|------|-------------|
| `plot/prefit_run{N}.pdf/.png` | Gaussian pre-fit |
| `plot/cbFit_run{N}[_{type}].pdf/.png` | cbFit fit results |
| `plot/cbFitLinked_run{N}[_{type}].pdf/.png` | cbFitLinked (CoMPASS) |
| `plot/cbFitLinkedcaen_run{N}[_{type}].pdf/.png` | cbFitLinkedcaen (CAEN) |
| `plot/resoVsFluence_{macro}_{type}.pdf/.png` | Resolution vs fluence |
| `plot/cbAlpha_compare.pdf/.png` | Crystal Ball alpha comparison |
| `plot/detector_chain_diagram.pdf/.png` | Signal chain schematic |
| `data/resolution/resolution.csv` | Fit results + fluence (auto-created) |

All plot output goes to `plot/` (gitignored).

## Project Structure

```
src/
  cbFit.cc                   — Free amplitude/mean ratio fits (CoMPASS)
  cbFitLinked.cc             — Fixed ratios, CoMPASS data
  cbFitLinkedcaen.cc         — Fixed ratios, CAEN DAQ data (ch==13)
  plotResolutionVsFluence.cc — Resolution vs fluence from CSV
  plotCB_alpha_compare.cc    — CB alpha parameter comparison
  draw_chain_diagram.py      — Detector chain schematic (matplotlib)
data/
  mystec_compass/run_{N}/RAW/DataR_run_{N}.root   — CoMPASS RAW (TTree "Data_R", branch "Energy")
  mystec_compass/run_{N}/run_{N}_info.txt          — Run metadata for fluence
  A1442B_PKUCAENDAQ/data_xxxx_notwave.root — CAEN DAQ (TTree "tree", branch "energy", ch==13)
  resolution/resolution.csv              — Fit results (auto-created)
plot/   — All output figures (gitignored)
run.sh  — Batch runner for multiple runs
```

## Code Style & Conventions

### Naming

| Element | Convention | Example |
|---------|-----------|---------|
| Functions | PascalCase | `SingleCB()`, `cbFit()`, `LoadParticleFluence()` |
| Variables | Hungarian prefix | `dMean0`, `nBins`, `bDraw` |
| Files | PascalCase `.cc`, lowercase `.py` | `cbFit.cc`, `draw_chain_diagram.py` |
| Global constants | `g_` + type prefix | `g_dE1`, `g_dI2` |

### Hungarian Type Prefixes

| Type | Prefix | Example |
|------|--------|---------|
| `Int_t` | `n` | `nRunNumber`, `nBins`, `nMaxBin` |
| `Double_t` | `d` | `dMean0`, `dSigma0`, `dPeakAmp` |
| `Bool_t` | `b` | `bDraw`, `bFitCB` |
| `TString` | `str` | `strFilePath`, `strFitType` |
| Pointer (any) | `f` | `fFile`, `fTree`, `fCb`, `fDG` |
| `TH1D*` | `h` | `hSpec`, `hSpecPre` |
| `TF1*` | `f` | `fGausPre`, `fCb`, `fDCB` |
| Loop index | `i` | `i` in for-loops |
| Global static | `g_` | `g_dE1`, `g_dE2` |

### Includes — ROOT C++ (ordered groups, blank line between)

1. ROOT headers (`#include <TFile.h>`, `<TH1D.h>`, `<TF1.h>`, ...)
2. ROOT math headers (`#include <Math/DistFunc.h>`)
3. Standard library (`#include <iostream>`, `<fstream>`, `<sstream>`)

Each group sorted alphabetically. Angle brackets for ROOT and stdlib.

### Types

- Prefer ROOT types: `Int_t`, `Double_t`, `Bool_t`, `Long64_t` over C++ primitives.
- Use `kTRUE` / `kFALSE` for `Bool_t`.
- Use `TString` for ROOT-interop strings; `TString::Format(...)` for paths.

### Error Handling

- Errors to `std::cerr`, info to `std::cout`.
- Return early on failure: check `TFile::IsZombie()`, null pointers, missing branches.
- Always `fFile->Close()` before returning on error.
- Detach histograms with `SetDirectory(0)` before closing files.

### Memory Management

- ROOT objects heap-allocated with `new`.
- `delete` cloned histograms and temporary `TF1` objects after use.
- `fFile->Close(); delete fFile;` when done.
- Histograms used after file closure: `hSpec->SetDirectory(0)` BEFORE `fFile->Close()`.

### Formatting

- 4-space indentation, no tabs.
- 120-character column limit.
- Opening brace on same line for functions and control structures.
- Single-statement `if` bodies may omit braces.
- `// ---` section dividers for major logical blocks.

### Comments

- English for code logic.
- Chinese acceptable for domain-specific annotations (physics notes, color descriptions).

## Physics / Domain Notes

### ²⁴¹Am Alpha Decay Data (hardcoded)

| Constant | Value | Description |
|----------|-------|-------------|
| `g_dE1` | 5442.80 keV | First alpha peak energy (13.1% intensity) |
| `g_dE2` | 5485.56 keV | Second alpha peak energy (84.8% intensity) |
| `g_dI1` | 0.131 | First peak relative intensity |
| `g_dI2` | 0.848 | Second peak relative intensity |
| `g_dAmpRatio` | `g_dI1 / g_dI2` | Fixed amplitude ratio (cbFitLinked/cbFitLinkedcaen) |
| `g_dMeanRatio` | `g_dE1 / g_dE2` | Fixed mean ratio (cbFitLinked/cbFitLinkedcaen) |

### Fit Models

- **cb** (Single Crystal Ball): 5 parameters — amp, mean, sigma, alpha, n
- **dg** (Double Gaussian): 5 parameters — amp1, mean1, amp2, mean2, shared-sigma
- **dcb** (Double Crystal Ball): 7 parameters — amp1, mean1, amp2, mean2, shared-sigma, shared-alpha, shared-n

### Input Data Format

| Source | File | Tree | Branch | Filter |
|--------|------|------|--------|--------|
| CoMPASS | `data/root/run_{N}/RAW/DataR_run_{N}.root` | `Data_R` | `Energy` | none |
| CAEN DAQ | `data/A1442B_PKUCAENDAQ/data_xxxx_notwave.root` | `tree` | `energy` | `ch==13` |

### resolution.csv Schema

```
macro,run_number,fit_type,resolution_percent,resolution_error_percent,particle_fluence_per_cm2
```

## Key Dependencies

- **CERN ROOT 6** — All I/O, histogramming, fitting, and plotting
- **ROOT::Math** — `crystalball_function` from `Math/DistFunc.h`
- **Python 3 / matplotlib** — Diagram script (use `/usr/bin/python3`; project's venv has no packages)
- **Noto Sans CJK** — Chinese font for matplotlib diagrams (auto-detected)

## Common Tasks

```bash
# Check if CoMPASS data exists
ls data/root/run_9/RAW/DataR_run_9.root
ls data/root/run_9/run_9_info.txt

# Check if CAEN DAQ data exists
ls data/A1442B_PKUCAENDAQ/data_0001_notwave.root

# Batch run fits
bash run.sh
```

### Add a New Fit Type

1. Define the model function: `Double_t MyModel(Double_t *dX, Double_t *dPar)`
2. Add `bFitMyModel` flag parsing in `cbFit()`
3. Create `TF1*` with initial parameters and limits
4. Run `hSpec->Fit()` twice (coarse then fine)
5. Extract and print results, append to CSV via `AppendResolutionCsv()`
6. Add drawing code in the `bDraw` block

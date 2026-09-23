#!/usr/bin/env bash
set -euo pipefail

for nRun in {7..8}; do
    echo "=== Run ${nRun}: cb ==="
    root -l -b -q "src/cbFit.cc(${nRun},\"cb\",1000,8500,11500)"

    echo "=== Run ${nRun}: dcb ==="
    root -l -b -q "src/cbFit.cc(${nRun},\"dcb\",1000,8500,11500)"

    echo "=== Run ${nRun}: dg ==="
    root -l -b -q "src/cbFit.cc(${nRun},\"dg\",1000,8500,11500)"
    
    echo "=== Run ${nRun}: link dcb ==="
    root -l -b -q "src/cbFitLinked.cc(${nRun}, \"dcb\",1000,8500,11500)"

    echo "=== Run ${nRun}: link dg ==="
    root -l -b -q "src/cbFitLinked.cc(${nRun}, \"dg\",1000,8500,11500)"
done

for nRun in 9 {11..22}; do
    echo "=== Run ${nRun}: cb ==="
    root -l -b -q "src/cbFit.cc(${nRun},\"cb\")"

    echo "=== Run ${nRun}: dcb ==="
    root -l -b -q "src/cbFit.cc(${nRun},\"dcb\")"

    echo "=== Run ${nRun}: dg ==="
    root -l -b -q "src/cbFit.cc(${nRun},\"dg\")"
    
    echo "=== Run ${nRun}: link dcb ==="
    root -l -b -q "src/cbFitLinked.cc(${nRun}, \"dcb\")"

    echo "=== Run ${nRun}: link dg ==="
    root -l -b -q "src/cbFitLinked.cc(${nRun}, \"dg\")"
done

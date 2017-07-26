#!/usr/bin/env sh
echo "*** Run EM validation test     ***"


echo "*** Make histograms            ***"
python $SIMCHECKSROOT/options/EmValidation/GenerateRootFiles.py


echo "*** Make histograms for LHCbPR ***"
python $SIMCHECKSROOT/python/EmValidation/analysisLHCbPR_Histograms.py
#!/usr/bin/env sh

OUTDIR=$PWD
RUNDIR=`mktemp -d`
cd $RUNDIR
echo "Continuing in: `pwd`"

# Generate rootFiles
cp $SIMCHECKSROOT/options/EmValidation/GenerateRootFiles.py .
cp $SIMCHECKSROOT/options/EmValidation/configurations.py .
cp $SIMCHECKSROOT/options/EmValidation/runTest.py .
python GenerateRootFiles.py

# Convert rootFiles to LHCbPR rootFiles
cp $SIMCHECKSROOT/python/EmValidation/analysisLHCbPR_Histograms.py .
python analysisLHCbPR_Histograms.py

mv *root ${OUTPUTDIR:-$OUTDIR}
cd ${OUTDIR}

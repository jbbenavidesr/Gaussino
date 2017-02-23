#!/usr/bin/env sh
echo "Running Target Test with All Options"

make_events_script=$SIMCHECKSROOT/options/Target/MakeEvents.py

python $make_events_script --physList "[\'QGSP_BERT\', \'FTFP_BERT\']" --materialList "[\'Al\',\'Si\',\'Be\']" --thicknessList "[1,5,10]" --pgunList "[\'Piminus\', \'Piplus\', \'Kminus\', \'Kplus\', \'p\', \'pbar\']" --energyList "[1,2,5,10,50,100]"
echo "WARNING: THIS TEST RETURNS 0 REGARDLESS OF ERRORS. FIX REQUIRED."
exit 0

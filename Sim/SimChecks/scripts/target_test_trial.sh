#!/usr/bin/env sh
echo "Running Target Test with Shorter Option List for Testing"

make_events_script=$SIMCHECKSROOT/options/Target/MakeEvents.py

python $make_events_script --physList "[\'QGSP_BERT\']" --materialList "[\'Al\']" --thicknessList "[1,5]" --pgunList "[\'Piminus\', \'Piplus\']" --energyList "[1,100]" --use-giga-geo
echo "WARNING: THIS TEST RETURNS 0 REGARDLESS OF ERRORS. FIX REQUIRED."
exit 0

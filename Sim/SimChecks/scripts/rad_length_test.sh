#!/usr/bin/env sh

echo "Running RadLength Test Part 1/2"
python $SIMCHECKSROOT/scripts/rad_length_scan.py --giga-geo
echo "Running RadLength Test Part 2/2"
python $SIMCHECKSROOT/scripts/rad_length_scan_velo_z.py --giga-geo

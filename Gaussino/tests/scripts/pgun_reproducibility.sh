#!/bin/bash

gaudirun.py $GAUSSINOROOT/tests/options/pgun_reproducibility_A.py
gaudirun.py $GAUSSINOROOT/tests/options/pgun_reproducibility_B.py
compareHepMCEvents.exe FirstRun-HepMC.root SecondRun-HepMC.root

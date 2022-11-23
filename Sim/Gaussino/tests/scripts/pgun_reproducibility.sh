#!/bin/bash
###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

gaudirun.py \
    $GAUSSINOOPTS/General/Events-1k.py \
    $GAUSSINOOPTS/General/OnlyGeneration.py \
    $GAUSSINOOPTS/General/Threads-20.py \
    $GAUSSINOROOT/tests/options/pgun_reproducibility.py \
    $GAUSSINOROOT/tests/options/pgun_reproducibility_A.py

gaudirun.py \
    $GAUSSINOOPTS/General/Events-1k.py \
    $GAUSSINOOPTS/General/OnlyGeneration.py \
    $GAUSSINOOPTS/General/Threads-20.py \
    $GAUSSINOROOT/tests/options/pgun_reproducibility.py \
    $GAUSSINOROOT/tests/options/pgun_reproducibility_B.py

compareHepMCEvents FirstRun-HepMC.txt SecondRun-HepMC.txt

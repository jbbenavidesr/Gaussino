###############################################################################
# (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Gaussino.pytest.helpers import *
from Gaussino.pytest.options import *


@reset_configurables
@events_10
@only_generation
@minbias
@pp
@debug
def test_minbias():
    ex = run_gaudi()
    assert "Registering tool Generation.MinimumBias.Pythia8ProductionMT" in ex.stdout
    assert "Beginning dump of converter" in ex.stdout
    assert ex.returncode == 0

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


@events_10
@only_generation
@minbias
@pp_7_tev
@debug
@flat
def test_flat_optics():
    ex = run_gaudi()
    assert (
        "Generation.Beam...  DEBUG Luminous Region : [0.00915283mm, 0.0158532mm, 38.7853mm, 0.179494ns ]"
        in ex.stdout
    )
    assert ex.returncode == 0

###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Gaussino.pytest.helpers import reset_configurables, run_gaudi
from Gaussino.pytest.options import cube, debug, em_physics, events_1, photon

EXPECTED_STRINGS = [
    "#Hits=    1 Energy=       1[GeV] #Particles=    1 in ExternalDetectorEmbedder_0_CubeSDet",
]


@reset_configurables
@events_1
@photon
@em_physics
@cube
@debug
def test_immediate_deposit_cube():
    ex = run_gaudi(
        # additional options
        "$CUSTOMSIMULATIONROOT/tests/options/immediate_deposit_cube.py",
    )
    assert ex.returncode == 0
    for exp in EXPECTED_STRINGS:
        assert exp in ex.stdout

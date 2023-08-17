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
@events_1
@debug
@edm
@em_physics
@photon
def test_mixed_geometry():
    ex = run_gaudi(
        # additional options
        "$PARALLELGEOMETRYROOT/tests/options/mixed_geometry.py",
    )
    assert ex.returncode == 0
    options = [
        "#Hits=    1 Energy=       0[GeV] #Particles=    1 in MassPlaneSDet",
        "#Hits=    1 Energy=       0[GeV] #Particles=    1 in ParallelPlane1SDet",
        "#Hits=    1 Energy=       0[GeV] #Particles=    1 in ParallelPlane2SDet",
    ]
    for option in options:
        if option not in ex.stdout:
            raise AssertionError(f"Missing string: '{option}'")

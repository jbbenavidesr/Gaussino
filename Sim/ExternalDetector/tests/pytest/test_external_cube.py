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

EXPECTED_STRINGS = [
    'Successfully embedded ExternalDetectorEmbedder_0_CubePVol in its mothers volume!',
    'Registered sensitive ExternalDetectorEmbedder_0_CubeSDet for ExternalDetectorEmbedder_0_CubePVol',
    'External world created!',
]


@reset_configurables
@events_1
@photon
@em_physics
@cube
@debug
def test_external_cube():
    ex = run_gaudi(
        # additional options
        "$GAUSSINOOPTS/Geometry/GDMLExport-Extended.py",
    )
    assert ex.returncode == 0
    for exp in EXPECTED_STRINGS:
        assert exp in ex.stdout

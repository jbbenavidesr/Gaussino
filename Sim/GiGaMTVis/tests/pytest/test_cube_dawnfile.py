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
from Gaussino.pytest.helpers import run_gaudi
from Gaussino.pytest.options import (
    debug,
    events_1,
    em_physics,
    photon,
    cube,
)


@events_1
@debug
@em_physics
@photon
@cube
def test_cube_dawnfile():
    ex = run_gaudi(
        # additional options
        "$GIGAMTVISROOT/tests/options/cube_dawnfile.py", )
    assert ex.returncode == 0
    expected_strings = [
        "Graphics system set to DAWNFILE (DAWNFILE)",
        "execute '/vis/filtering/trajectories/gaussinoIMagF/setAttribute IMag'",
        "execute '/vis/filtering/trajectories/gaussinoIMagF/addInterval 0.0 keV 2.0 MeV'",
        "execute '/vis/scene/add/hits '",
        ".prim  is generated.",
    ]
    for expected_string in expected_strings:
        assert expected_string in ex.stdout

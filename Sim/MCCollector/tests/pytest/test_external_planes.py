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

import re

from Gaussino.pytest.helpers import reset_configurables, run_gaudi
from Gaussino.pytest.options import (
    debug,
    edm,
    em_physics,
    empty_world,
    events_1,
    photon,
)


@reset_configurables
@events_1
@empty_world
@em_physics
@photon
@edm
@debug
def test_external_planes():
    ex = run_gaudi(
        # additional options
        "$MCCOLLECTORROOT/tests/options/external_planes.py",
    )
    assert ex.returncode == 0
    for plane_no in range(1, 5):
        plane_regex = rf"#Hits\W*1\W*Energy=\W*0\[GeV\]\W*#Particles=\W*1\W*in\W+Plane{plane_no}SDet"
        if not re.findall(plane_regex, ex.stdout):
            raise AssertionError(f"Hit registration went wrong for plane #{plane_no}!")
    if "NTuples saved successfully" not in ex.stdout:
        raise AssertionError("NTuples were not created!")

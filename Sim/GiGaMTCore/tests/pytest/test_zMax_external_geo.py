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

from Gaussino.pytest.helpers import *
from Gaussino.pytest.options import *


@reset_configurables
@events_1
@em_physics
@edm
def test_zMax_external_geo():
    ex = run_gaudi(
        # additional options
        "$GIGAMTCOREROOT/tests/options/zMax_external_geo.py",
    )
    assert ex.returncode == 0
    # expect more than one particles stored in the north plane (behind zMax)
    north_plane_regex = r"Particles=\W*\b(?![1]\b)\d{1,7}\b\W*in\W*NorthPlaneSDet"
    if not re.findall(north_plane_regex, ex.stdout):
        raise AssertionError("NorthPlane should store more than one particle!")

    # expect exactly one particle stored in the south plane
    south_plane_regex = r"Particles=\W*1\W*in\W*SouthPlaneSDet"
    if not re.findall(south_plane_regex, ex.stdout):
        raise AssertionError("SouthPlane should store exactly one particle!")

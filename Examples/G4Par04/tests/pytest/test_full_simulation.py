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

import re

from Gaussino.pytest.helpers import reset_configurables, run_gaudi
from Gaussino.pytest.options import edm, events_1

patterns = [
    r"\"\#FullSimHits\"\s+\|\s+1\s+\|\s+1124\s+\|\s+1124\.0\s+\|\s+0\.0000",
    r"Particle Energy [MeV]\s+\|\s+1\s+|\s+10000\s+|\s+10000\.0\s+|\s+0\.0000\s+|\s+10000\.0\s+|\s+10000\.0",
]


@reset_configurables
@events_1
@edm
def test_full_simulation():
    ex = run_gaudi("$G4PAR04ROOT/options/full_simulation.py")
    for pattern in patterns:
        if not re.findall(pattern, ex.stdout):
            raise AssertionError("Unexpected output")
    assert ex.returncode == 0

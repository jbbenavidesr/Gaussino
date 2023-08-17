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
from subprocess import run

import pytest


def test_newrnd_grandom_hack():
    ex = run(
        ["GaussinoTestgRandom"],
        capture_output=True,
        text=True,
    )
    print(ex.stdout)  # will print with `pytest ... -s`
    assert ex.returncode == 0

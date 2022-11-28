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
import pytest
from subprocess import run
from Gaussino.pytest.helpers import *

@pytest.mark.parametrize(
    "threads", [1, 4, 20]
)
def test_pgun_reproducibility(threads):
    reset_configurables_now()
    ex_A = run_gaudi(
        "$GAUSSINOOPTS/General/Events-1k.py",
        "$GAUSSINOOPTS/General/OnlyGeneration.py",
        f"$GAUSSINOOPTS/General/Threads-{threads}.py",
        "$HEPMCUSERROOT/tests/options/pgun_reproducibility.py",
        "$HEPMCUSERROOT/tests/options/pgun_reproducibility_A.py",
    )
    assert ex_A.returncode == 0
    reset_configurables_now()
    ex_B = run_gaudi(
        "$GAUSSINOOPTS/General/Events-1k.py",
        "$GAUSSINOOPTS/General/OnlyGeneration.py",
        f"$GAUSSINOOPTS/General/Threads-{threads}.py",
        "$HEPMCUSERROOT/tests/options/pgun_reproducibility.py",
        "$HEPMCUSERROOT/tests/options/pgun_reproducibility_B.py",
    )
    assert ex_B.returncode == 0
    comparison = run(
        [
            "compareHepMCEvents",
            "FirstRun-HepMC.txt",
            "SecondRun-HepMC.txt",
        ],
        capture_output=True,
        text=True,
    )
    print(comparison.stdout) # will print with `pytest ... -s`
    assert comparison.returncode == 0

###############################################################################
# (c) Copyright 2024 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

import multiprocessing

from config import run_test
from Gaussino.pytest.helpers import reset_configurables

max_threads = multiprocessing.cpu_count()


@reset_configurables
def test_cylindrical_g4_mt():
    run_test("geant4", "cylindrical", max_threads)

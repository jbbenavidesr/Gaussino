###############################################################################
# (c) Copyright 2025 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

import os
from Configurables import Gaussino
from TargetTracker.tracker_simulation import set_full_simulation

# Configure standard Geant4 simulation
set_full_simulation()

# General Gaussino configuration
Gaussino().EvtMax = int(os.environ.get("NUMBER_OF_EVENTS", 10))
Gaussino().Phases = ["Generator", "Simulation"]
Gaussino().EnableHive = True
Gaussino().ThreadPoolSize = int(os.environ.get("NUMBER_OF_THREADS", 1))
Gaussino().EventSlots = int(os.environ.get("NUMBER_OF_THREADS", 1))

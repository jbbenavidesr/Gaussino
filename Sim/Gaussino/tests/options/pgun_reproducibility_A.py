###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Configurables import Gaussino
from Gaussino.Generation import GenPhase
from Configurables import HepMCWriter
Gaussino().Phases = ['Generator']
GenPhase().ParticleGun = True
Gaussino().EvtMax=1000
Gaussino().EnableHive = True

Gaussino().ThreadPoolSize = 20
Gaussino().EventSlots = 20

HepMCWriter().Writer = 'WriterAscii'
GenPhase().WriteHepMC = True
Gaussino().DatasetName =  'FirstRun'
Gaussino().DatasetNameForced = True

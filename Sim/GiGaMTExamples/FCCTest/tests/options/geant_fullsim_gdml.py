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
from multiprocessing import cpu_count
from Configurables import GiGaMT, G4GdmlTestDetector, G4SaveTestCalHits
giga = GiGaMT()
giga.NumberOfWorkerThreads = cpu_count()

# Set up the GDML detector construction tool and point it to the
giga.DetectorConstruction = "G4GdmlTestDetector"
dettool = giga.addTool(G4GdmlTestDetector,
                       "G4GdmlTestDetector")
dettool.GDML = '$FCCTESTROOT/data/gflashDetector.xml'
# monitool = giga.addTool(G4SaveTestCalHits,
                        # "G4SaveTestCalHits")
# monitool.caloType = "ECal"
# giga.MonitorTools = ["G4SaveTestCalHits"]


def giga_debugs(name):
    import Configurables
    giga = GiGaMT()
    conf = getattr(Configurables, name)
    inst = giga.addTool(conf, name)
    inst.OutputLevel = -10

# giga_debugs("GiGaMTRunManagerFAC")
# giga_debugs("GiGaMT_FTFP_BERT")
# giga_debugs("GiGaActionInitializer")
# giga_debugs("GiGaWorkerPilotFAC")
# giga_debugs("GiGaMTDetectorConstructionFAC")
# giga.OutputLevel = -10
# monitool.OutputLevel = -10
# dettool.OutputLevel = -10

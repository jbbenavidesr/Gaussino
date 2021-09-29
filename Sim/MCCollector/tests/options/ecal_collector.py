###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Gaudi.Configuration import importOptions
importOptions("$GAUSSROOT/tests/options/testGauss-NoOutput.py")
importOptions("$GAUSSROOT/tests/options/testGauss-QMTests.py")

from Configurables import CondDB
CondDB().Upgrade = True

from Configurables import LHCbApp
LHCbApp().DDDBtag = "upgrade/dddb-20210617"
LHCbApp().CondDBtag = "upgrade/sim-20210617-vc-mu100"
LHCbApp().Simulation = True
LHCbApp().EvtMax = 1

from Configurables import Gauss
Gauss().DataType = "Upgrade"

# only ECAL & NS in LHCb
from Configurables import Gauss
Gauss().DetectorGeo  = { "Detectors": ['Ecal', 'Shield'] }
Gauss().DetectorSim  = { "Detectors": ['Ecal'] }
Gauss().DetectorMoni = { "Detectors": ['Ecal'] }
Gauss().BeamPipe = "BeamPipeOff"

# Particle Gun On
from Gaudi.Configuration import importOptions
importOptions("$LBPGUNSROOT/options/PGuns.py")
from Configurables import ParticleGun
pgun = ParticleGun("ParticleGun")
from Configurables import FlatPtRapidity
pgun.ParticleGunTool = "FlatPtRapidity"
pgun.addTool(FlatPtRapidity, name="FlatPtRapidity")
from GaudiKernel.SystemOfUnits import GeV
pgun.FlatPtRapidity.PtMin = .5 * GeV
pgun.FlatPtRapidity.PtMax = .5 * GeV
pgun.FlatPtRapidity.RapidityMin = 2.5  # ensure in ECAL
pgun.FlatPtRapidity.RapidityMax = 4.8  # ensure in ECAL
pgun.FlatPtRapidity.PdgCodes = [22]
from Configurables import FlatNParticles
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 10
pgun.FlatNParticles.MaxNParticles = 10

# Disable spill-over
from Configurables import Gauss
Gauss().SpilloverPaths = []

# adding external detectors
from Configurables import ExternalDetectorEmbedder
external = ExternalDetectorEmbedder("EcalCollectorEmbedder")

# generating ecal collector plane
from GaudiKernel.SystemOfUnits import m, mm, degree
from Gaudi.Configuration import DEBUG
shapes = {
    'EcalCollector': {
        "Type": "Cuboid",
        "xPos": 0. * m,
        "yPos": 0. * m,
        "zPos": (11948. - .005) * mm,  # zMax - thickness/2!
        "xSize": 20. * m,
        "ySize": 20. * m,
        "zSize": .01 * m,
        "xAngle": -.207 * degree,
        "OutputLevel": DEBUG,
    },
}

sensitive = {
    'EcalCollector': {
        "Type": "MCCollectorSensDet",
        "RequireEDep": False,
        "OnlyForward": True,
        "OnlyAtBoundary": True,
        "OutputLevel": DEBUG,
    },
}

hit = {
    'EcalCollector': {
        'Type': 'GetMCCollectorHitsAlg',
        'OutputLevel': DEBUG,
    },
}

external.Shapes = shapes
external.Sensitive = sensitive
external.Hit = hit

# embedding of the geometry in GaussGeo
from Configurables import Gauss
Gauss().ExternalDetectorEmbedder = "EcalCollectorEmbedder"

import GaudiPython as GP


class CollectorMatch(GP.PyAlgorithm):
    def execute(self):
        evt = appMgr.evtsvc()
        collector_ids = [
            hit.mcParticle().index() for hit in evt['MC/EcalCollector/Hits']
        ]
        collector_ids.sort()
        ecal_ids = list(
            set([hit.particle().index() for hit in evt['MC/Ecal/Hits']]))
        if collector_ids == ecal_ids and len(ecal_ids) == 10:
            print("EcalCollector matched with Ecal hits!")
        return True


appMgr = GP.AppMgr()
appMgr.addAlgorithm(CollectorMatch())
appMgr.run(1)

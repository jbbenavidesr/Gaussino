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

from Configurables import ExternalDetectorEmbedder
from Gaudi.Configuration import DEBUG
from GaudiKernel import SystemOfUnits as units

# number of planes to generate
PLANES_NO = 4

# adding external detectors
external = ExternalDetectorEmbedder("ExternalDetectorEmbedder_0")
# generating planes
shapes = {}
sensitive = {}
hit = {}
moni = {}
for plane in range(PLANES_NO):
    shapes["Plane{}".format(plane + 1)] = {
        "Type": "Cuboid",
        "xPos": 0.0 * units.m,
        "yPos": 0.0 * units.m,
        "zPos": (1.0 + plane) * units.m,
        "xSize": 10.0 * units.m,
        "ySize": 10.0 * units.m,
        "zSize": 0.01 * units.mm,
        "OutputLevel": DEBUG,
    }
    sensitive["Plane{}".format(plane + 1)] = {
        "Type": "MCCollectorSensDet",
        "RequireEDep": False,
        "PrintStats": True,
        "OutputLevel": DEBUG,
    }
    hit["Plane{}".format(plane + 1)] = {
        "Type": "GetMCCollectorHitsAlg",
        "OutputLevel": DEBUG,
    }
    moni["Plane{}".format(plane + 1)] = {
        "Type": "MCCollector",
        "HitsPropertyName": "CollectorHits",
        "OutputLevel": DEBUG,
    }
external.Shapes = shapes
external.Sensitive = sensitive
external.Hit = hit
external.Moni = moni

from Configurables import NTupleSvc

NTupleSvc().Output = ["FILE1 DATAFILE='CaloCollector.root' TYP='ROOT' OPT='NEW'"]

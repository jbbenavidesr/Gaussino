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
# TODO: This test should check also hit extraction, but we're missing
# a generic sensitive detector factory. It can be updated with the
# MCCollector package.

# number of planes to generate
PLANES_NO = 4

from Configurables import CondDB
CondDB().Upgrade = True

from Configurables import LHCbApp
LHCbApp().DDDBtag = "upgrade/dddb-20210617"
LHCbApp().CondDBtag = "upgrade/sim-20210617-vc-mu100"
LHCbApp().Simulation = True
LHCbApp().EvtMax = 1

# Particle Gun On
from Gaudi.Configuration import importOptions
importOptions("$LBPGUNSROOT/options/PGuns.py")

# empty LHCb
from Configurables import Gauss
Gauss().DetectorGeo = {"Detectors": []}
Gauss().DetectorSim = {"Detectors": []}
Gauss().DetectorMoni = {"Detectors": []}
Gauss().BeamPipe = "BeamPipeOff"
Gauss().DataType = "Upgrade"

# adding external detectors
from Configurables import ExternalDetectorEmbedder
external = ExternalDetectorEmbedder("Testing")

# generating planes
from GaudiKernel.SystemOfUnits import m
from Gaudi.Configuration import DEBUG

shapes = {}
sensitive = {}
# hit = {}

for plane in range(PLANES_NO):
    shapes['Plane{}'.format(plane + 1)] = {
        "Type": "Cuboid",
        "xPos": 0. * m,
        "yPos": 0. * m,
        "zPos": (1. + plane) * m,
        "xSize": 10. * m,
        "ySize": 10. * m,
        "zSize": .01 * m,
    }
    sensitive['Plane{}'.format(plane + 1)] = {
        "Type": "GiGaSensDetTracker",
        "RequireEDep": False,
        "OutputLevel": DEBUG,
    }
    # TODO: This is not yet possible without adding
    # a more generic hit alg
    # hit['Plane{}'.format(plane + 1)] = {
    #     "Type:": "GetTrackerHitsAlg"
    # }

external.Shapes = shapes
external.Sensitive = sensitive
# external.Hit = hit

# embedding of the geometry in GaussGeo
from Configurables import Gauss
Gauss().ExternalDetectorEmbedder = "Testing"

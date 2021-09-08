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
from Configurables import CondDB
CondDB().Upgrade = True

from Configurables import LHCbApp
LHCbApp().DataType = "Upgrade"
LHCbApp().DDDBtag = "upgrade/dddb-20210617"
LHCbApp().CondDBtag = "upgrade/sim-20210617-vc-mu100"
LHCbApp().Simulation = True

# setting up external geometry service
from Configurables import ExternalDetectorWorldCreator
from Gaudi.Configuration import DEBUG
creator = ExternalDetectorWorldCreator()
creator.OutputLevel = DEBUG
#creator.WriteGDML = True

# adding external detectors
from Configurables import ExternalDetectorEmbedder
external = ExternalDetectorEmbedder("Testing")
from GaudiKernel.SystemOfUnits import m
from Gaudi.Configuration import DEBUG
external.Shapes = {
    "MyCube": {
        "Type": "Cuboid",
        "xSize": 1. * m,
        "ySize": 1. * m,
        "zSize": 1. * m,
    },
}

external.Sensitive = {
    "MyCube": {
        "Type": "GiGaSensDetTracker",
    },
}

# here embedding of the geometry takes place
external.embed(creator)

from Configurables import ApplicationMgr
ApplicationMgr().TopAlg += [creator]

import GaudiPython as GP
appMgr = GP.AppMgr()
appMgr.initialize()

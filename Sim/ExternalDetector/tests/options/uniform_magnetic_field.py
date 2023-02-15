###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Configurables import ExternalDetectorEmbedder
from GaudiKernel import SystemOfUnits as units
from Gaudi.Configuration import DEBUG
external = ExternalDetectorEmbedder("ExternalDetectorEmbedder_0")
external.MagneticField = {
    "Type": "UniformMagneticField",
    "B_x": 1. * units.tesla,
    "B_y": 0. * units.tesla,
    "B_z": 0. * units.tesla,
    "OutputLevel": DEBUG,
}

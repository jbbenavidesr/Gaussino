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
from Configurables import ExternalDetectorEmbedder, GaussinoGeometry
from ExternalDetector.Materials import OUTER_SPACE
from GaudiKernel import SystemOfUnits as units

emb_id = len(ExternalDetectorEmbedder.configurables)
emb_name = f"ExternalDetectorEmbedder_{emb_id}"

GaussinoGeometry().ExternalDetectorEmbedder = emb_name
external = ExternalDetectorEmbedder(emb_name)

# plain/testing geometry service
external.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
    "WorldLogicalVolumeName": f"{emb_name}_WorldLvol",
    "WorldPhysicalVolumeName": f"{emb_name}_WorldPvol",
}

# material needed for the external world
external.Materials = {
    "OuterSpace": OUTER_SPACE,
}

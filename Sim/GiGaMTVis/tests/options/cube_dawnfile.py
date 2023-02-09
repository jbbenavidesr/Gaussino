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
from ExternalDetector.Materials import SILICA
from GaudiKernel import SystemOfUnits as units
from Configurables import (
    ExternalDetectorEmbedder,
    GaussinoVisualization,
)

external = ExternalDetectorEmbedder("ExternalDetectorEmbedder_0")
external.Shapes["ExternalDetectorEmbedder_0_Cube"]["MaterialName"] = "Si"
external.Materials["Si"] = SILICA

GaussinoVisualization(
    Framework=["Geant4"],
    Driver="DAWNFILE",
    DrawGeometry=True,
    DrawTrajectories=True,
    DrawG4Hits=True,
    CameraPhi=205,
    CameraTheta=40,
    TrajectoryModel="drawByParticleID",
    TrajectoryType="smooth",
    TrajectoryFilters=[{
        "FilterType": "momentumMagnitudeFilter",
        "MinValue": 2 * units.MeV
    }],
    StoreTrajectories="All",
    Debug=True,
)

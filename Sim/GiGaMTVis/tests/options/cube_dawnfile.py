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
    GaussinoSimulation,
    ExternalDetectorEmbedder,
    Geant4Visualization,
)

external = ExternalDetectorEmbedder("ExternalDetectorEmbedder_0")
external.Shapes["ExternalDetectorEmbedder_0_Cube"]["MaterialName"] = "Si"
external.Materials["Si"] = SILICA

GaussinoSimulation().Visualization = True
g4vis = Geant4Visualization()
g4vis.Driver = "DAWNFILE"
g4vis.DrawGeometry = True
g4vis.DrawTrajectories = True
g4vis.DrawG4Hits = True
g4vis.CameraPhi = 205
g4vis.CameraTheta = 40
g4vis.TrajectoryModel = "drawByParticleID"
g4vis.TrajectoryType = "smooth"
g4vis.TrajectoryFilters = [{
    "FilterType": "momentumMagnitudeFilter",
    "MinValue": 2 * units.MeV
}]
g4vis.StoreTrajectories = "All"
g4vis.DrawG4Hits = True
g4vis.Debug = True

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
"""
Test that checks TruthFlaggingTrackAction options combined with the external
geometry. Two 100 MeV photons are produced in opposite directions along the
y-axis. There are 2 external sensitive detectors made out of lead: NorthPlane
at (0, 100m, 0) and SouthPlane at (0, -100m, 0). ZMaxPlane is set at the origin
with a 45 degrees tilt. As a result, the NorthPlane should store information
about all particles and tracks created in a shower and all of the SouthPlane's
hits should be assigned to an incoming photon.
"""
import math

from Configurables import (
    ExternalDetectorEmbedder,
    FlatNParticles,
    GaussinoGeneration,
    GaussinoGeometry,
    MaterialEval,
    ParticleGun,
)
from ExternalDetector.Materials import LEAD, OUTER_SPACE
from Gaudi.Configuration import DEBUG, appendPostConfigAction
from GaudiKernel import SystemOfUnits as units

GaussinoGeneration().ParticleGun = True
pgun = ParticleGun("ParticleGun")
pgun.ParticleGunTool = "MaterialEval"
pgun.addTool(MaterialEval, name="MaterialEval")
pgun.MaterialEval.EtaPhi = True
pgun.MaterialEval.UseGrid = True
pgun.MaterialEval.MinEta = 0.0
pgun.MaterialEval.MaxEta = 0.0
pgun.MaterialEval.MinPhi = 0.0 * units.radian
pgun.MaterialEval.MaxPhi = 2 * math.pi * units.radian
# produce only 2 gammas
pgun.MaterialEval.PdgCode = 22
pgun.MaterialEval.NStepsInEta = 1
pgun.MaterialEval.NStepsInPhi = 2
pgun.MaterialEval.ModP = 100.0 * units.MeV
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 2
pgun.FlatNParticles.MaxNParticles = 2

# External detector embedders in mass & parallel geometry
mass_embedder = ExternalDetectorEmbedder("NorthSouthPlanesEmbedder")
# here embedding of the geometry takes place
GaussinoGeometry().ExternalDetectorEmbedder = "NorthSouthPlanesEmbedder"
# plain/testing geometry service
mass_embedder.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
    "WorldSizeX": 150 * units.m,
    "WorldSizeY": 150 * units.m,
    "WorldSizeZ": 20 * units.m,
}
# material needed for the external world
mass_embedder.Materials = {
    "OuterSpace": OUTER_SPACE,
    "Pb": LEAD,
}
# Generic options for all detectors
mass_embedder.Shapes = {
    "NorthPlane": {
        "Type": "Cuboid",
        "MaterialName": "Pb",
        "xPos": 0.0 * units.m,
        "yPos": 100.0 * units.m,
        "zPos": 0.0 * units.m,
        "xSize": 0.2 * units.m,
        "ySize": 5.0 * units.m,
        "zSize": 0.2 * units.m,
    },
    "SouthPlane": {
        "Type": "Cuboid",
        "MaterialName": "Pb",
        "xPos": 0.0 * units.m,
        "yPos": -100.0 * units.m,
        "zPos": 0.0 * units.m,
        "xSize": 0.2 * units.m,
        "ySize": 5.0 * units.m,
        "zSize": 0.2 * units.m,
    },
}
mass_embedder.Sensitive = {
    "NorthPlane": {
        "Type": "MCCollectorSensDet",
        "RequireEDep": False,
        "OnlyForward": False,
        "PrintStats": True,
    },
    "SouthPlane": {
        "Type": "MCCollectorSensDet",
        "RequireEDep": False,
        "OnlyForward": False,
        "PrintStats": True,
    },
}
mass_embedder.Hit = {
    "NorthPlane": {
        "Type": "GetMCCollectorHitsAlg",
    },
    "SouthPlane": {
        "Type": "GetMCCollectorHitsAlg",
    },
}


# setting up the TruthFlaggingTrackAction options
def updateZMax():
    from Configurables import TruthFlaggingTrackAction

    trth = TruthFlaggingTrackAction(
        "GiGaMT.GiGaActionInitializer.TruthFlaggingTrackAction"
    )
    trth.ZmaxForStoring = 0.0 * units.m
    trth.ZmaxForStoringTilt = math.pi / 4.0 * units.radian
    trth.OutputLevel = DEBUG
    trth.StorePrimaries = True
    trth.StoreAll = True
    trth.StoreByOwnProcess = False
    trth.StoreByChildProcess = False
    trth.StoreByChildEnergy = False
    trth.StoreForcedDecays = False
    trth.StoreByOwnEnergy = True


appendPostConfigAction(updateZMax)

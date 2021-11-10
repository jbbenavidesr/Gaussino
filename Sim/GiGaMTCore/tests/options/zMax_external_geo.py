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
# standard Gaussino
from Configurables import Gaussino
Gaussino().EvtMax = 1
Gaussino().EnableHive = True
Gaussino().ThreadPoolSize = 1
Gaussino().EventSlots = 1

from Configurables import GiGaMT
GiGaMT().NumberOfWorkerThreads = 1

# Enable EDM as we will create hits
from Configurables import Gaussino
Gaussino().ConvertEDM = True

# Activate EM physics
from Gaussino.Simulation import SimPhase
SimPhase().PhysicsConstructors = ["GiGaMT_G4EmStandardPhysics"]

# Particle Gun
# shoots just one 1 GeV photon along z
from Gaudi.Configuration import DEBUG
from Gaussino.Generation import GenPhase
GenPhase().ParticleGun = True
GenPhase().ParticleGunUseDefault = False

from Configurables import ParticleGun
pgun = ParticleGun("ParticleGun")
pgun.OutputLevel = DEBUG

from Configurables import MaterialEval
pgun.ParticleGunTool = "MaterialEval"
pgun.addTool(MaterialEval, name="MaterialEval")
import math
from GaudiKernel.SystemOfUnits import GeV, MeV, mm, radian
pgun.MaterialEval.EtaPhi = True
pgun.MaterialEval.UseGrid = True
pgun.MaterialEval.MinEta = 0.
pgun.MaterialEval.MaxEta = 0.
pgun.MaterialEval.MinPhi = 0. * radian
pgun.MaterialEval.MaxPhi = 2 * math.pi * radian
# produce only 2 gammas
pgun.MaterialEval.PdgCode = 22
pgun.MaterialEval.NStepsInEta = 1
pgun.MaterialEval.NStepsInPhi = 2
pgun.MaterialEval.ModP = 100. * MeV
pgun.MaterialEval.OutputLevel = DEBUG

from Configurables import FlatNParticles
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 2
pgun.FlatNParticles.MaxNParticles = 2

from Gaudi.Configuration import DEBUG
from GaudiKernel.SystemOfUnits import m, g, cm3, pascal, mole, kelvin
# External detector embedders in mass & parallel geometry
from Configurables import ExternalDetectorEmbedder
mass_embedder = ExternalDetectorEmbedder('NorthSouthPlanesEmbedder')

# plain/testing geometry service
mass_embedder.World = {
    'WorldMaterial': 'OuterSpace',
    'Type': 'ExternalWorldCreator',
    'WorldSizeX': 150 * m,
    'WorldSizeY': 150 * m,
    'WorldSizeZ': 20 * m,
    'OutputLevel': DEBUG,
}

# material needed for the external world
mass_embedder.Materials = {
    "OuterSpace": {
        'AtomicNumber': 1.,
        'MassNumber': 1.01 * g / mole,
        'Density': 1.e-25 * g / cm3,
        'Pressure': 3.e-18 * pascal,
        'Temperature': 2.73 * kelvin,
        'State': 'Gas',
        'OutputLevel': DEBUG,
    },
    'Pb': {
        'Type': 'MaterialFromElements',
        'Symbols': ['Pb'],
        'AtomicNumbers': [82.],
        'MassNumbers': [207.2 * g / mole],
        'MassFractions': [1.],
        'Density': 11.29 * g / cm3,
        'State': 'Solid',
        'OutputLevel': DEBUG,
    },
}

# Generic options for all detectors
from GaudiKernel.SystemOfUnits import m
mass_embedder.Shapes = {
    'NorthPlane': {
        'Type': 'Cuboid',
        'MaterialName': 'Pb',
        'xPos': 0. * m,
        'yPos': 100. * m,
        'zPos': 0. * m,
        'xSize': .2 * m,
        'ySize': 5. * m,
        'zSize': .2 * m,
        'OutputLevel': DEBUG,
    },
    'SouthPlane': {
        'Type': 'Cuboid',
        'MaterialName': 'Pb',
        'xPos': 0. * m,
        'yPos': -100. * m,
        'zPos': 0. * m,
        'xSize': .2 * m,
        'ySize': 5. * m,
        'zSize': .2 * m,
        'OutputLevel': DEBUG,
    },
}

mass_embedder.Sensitive = {
    'NorthPlane': {
        'Type': 'MCCollectorSensDet',
        'RequireEDep': False,
        'OnlyForward': False,
        'PrintStats': True,
        'OutputLevel': DEBUG,
    },
    'SouthPlane': {
        'Type': 'MCCollectorSensDet',
        'RequireEDep': False,
        'OnlyForward': False,
        'PrintStats': True,
        'OutputLevel': DEBUG,
    },
}

mass_embedder.Hit = {
    'NorthPlane': {
        'Type': 'GetMCCollectorHitsAlg',
        'OutputLevel': DEBUG,
    },
    'SouthPlane': {
        'Type': 'GetMCCollectorHitsAlg',
        'OutputLevel': DEBUG,
    },
}

# here embedding of the geometry takes place
from Gaussino.Simulation import SimPhase
SimPhase().ExternalDetectorEmbedder = "NorthSouthPlanesEmbedder"

# setting up the TruthFlaggingTrackAction options
from Gaudi.Configuration import appendPostConfigAction


def updateZMax():
    from Configurables import TruthFlaggingTrackAction
    trth = TruthFlaggingTrackAction(
        "GiGaMT.GiGaActionInitializer.TruthFlaggingTrackAction")
    trth.ZmaxForStoring = 0. * m
    trth.ZmaxForStoringTilt = math.pi / 4. * radian
    trth.OutputLevel = DEBUG
    trth.StorePrimaries = True
    trth.StoreAll = True
    trth.StoreByOwnProcess = False
    trth.StoreByChildProcess = False
    trth.StoreByChildEnergy = False
    trth.StoreForcedDecays = False
    trth.StoreByOwnEnergy = True


appendPostConfigAction(updateZMax)

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
from Gaussino.Generation import GenPhase
GenPhase().ParticleGun = True
GenPhase().ParticleGunUseDefault = False

from Configurables import ParticleGun
pgun = ParticleGun("ParticleGun")
from Configurables import FixedMomentum
pgun.ParticleGunTool = "FixedMomentum"
pgun.addTool(FixedMomentum, name="FixedMomentum")
from GaudiKernel.SystemOfUnits import GeV
pgun.FixedMomentum.px = 0. * GeV
pgun.FixedMomentum.py = 0. * GeV
pgun.FixedMomentum.pz = 1. * GeV
pgun.FixedMomentum.PdgCodes = [22]
from Configurables import FlatNParticles
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 1
pgun.FlatNParticles.MaxNParticles = 1

# plain/testing geometry service
from Gaudi.Configuration import DEBUG
world = {
    'WorldMaterial': 'OuterSpace',
    'Type': 'ExternalWorldCreator',
    'OutputLevel': DEBUG,
}

# material needed for the external world
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin
from Gaudi.Configuration import DEBUG
materials = {
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
generic_shape = {
    'Type': 'Cuboid',
    'xPos': 0. * m,
    'yPos': 0. * m,
    'zPos': 5. * m,
    'xSize': 5. * m,
    'ySize': 5. * m,
    'zSize': 1. * m,
    'OutputLevel': DEBUG,
}

generic_sensitive = {
    'Type': 'MCCollectorSensDet',
    'RequireEDep': False,
    'OnlyForward': False,
    'OutputLevel': DEBUG,
}

generic_hit = {
    'Type': 'GetMCCollectorHitsAlg',
    'OutputLevel': DEBUG,
}

# External detector embedders in mass & parallel geometry
from Configurables import ExternalDetectorEmbedder
mass_embedder = ExternalDetectorEmbedder('MassEmbedder')
parallel_embedder_1 = ExternalDetectorEmbedder('ParallelEmbedder1')
parallel_embedder_2 = ExternalDetectorEmbedder('ParallelEmbedder2')

mass_embedder.Shapes = {'MassPlane': dict(generic_shape)}
mass_embedder.Shapes['MassPlane']["MaterialName"] = 'Pb'
mass_embedder.Sensitive = {'MassPlane': dict(generic_sensitive)}
mass_embedder.Hit = {'MassPlane': dict(generic_hit)}
mass_embedder.Materials = materials
mass_embedder.World = world

parallel_embedder_1.Shapes = {'ParallelPlane1': dict(generic_shape)}
parallel_embedder_1.Shapes['ParallelPlane1']["MaterialName"] = 'OuterSpace'
parallel_embedder_1.Sensitive = {'ParallelPlane1': dict(generic_sensitive)}
parallel_embedder_1.Hit = {'ParallelPlane1': dict(generic_hit)}

parallel_embedder_2.Shapes = {'ParallelPlane2': dict(generic_shape)}
parallel_embedder_2.Shapes['ParallelPlane2']["MaterialName"] = 'Pb'
parallel_embedder_2.Sensitive = {'ParallelPlane2': dict(generic_sensitive)}
parallel_embedder_2.Hit = {'ParallelPlane2': dict(generic_hit)}

from Configurables import ParallelGeometry
ParallelGeometry().ParallelWorlds = {
    'ParallelWorld1': {
        'ExternalDetectorEmbedder': 'ParallelEmbedder1',
    },
    'ParallelWorld2': {
        'ExternalDetectorEmbedder': 'ParallelEmbedder2',
    },
}
ParallelGeometry().ParallelPhysics = {
    'ParallelWorld1': {
        'LayeredMass': True,
        'ParticlePIDs': [22],
    },
    'ParallelWorld2': {
        'LayeredMass': False,
        'ParticlePIDs': [22],
    },
}

from Gaussino.Simulation import SimPhase
SimPhase().ParallelGeometry = True

# here embedding of the geometry takes place
from Gaussino.Simulation import SimPhase
SimPhase().ExternalDetectorEmbedder = "MassEmbedder"

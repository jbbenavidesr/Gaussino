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

from Configurables import Gaussino
Gaussino().EvtMax = 1
Gaussino().EnableHive = True
Gaussino().ThreadPoolSize = 1
Gaussino().EventSlots = 1

# some dumb generation, not important, just fast
from Gaussino.Generation import GenPhase
GenPhase().ParticleGun = True
GenPhase().ParticleGunUseDefault = True

from Gaussino.Simulation import SimPhase
SimPhase().PhysicsConstructors = [
    "GiGaMT_G4HadronPhysicsFTFP_BERT",
    "GiGaMT_G4EmStandardPhysics",
]

# setting up external geometry service

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
        "OutputLevel": DEBUG,
    },
}

external.Sensitive = {
    "MyCube": {
        "Type": "MCCollectorSensDet",
        "OutputLevel": DEBUG,
    },
}

# plain/testing geometry service
external.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
    "OutputLevel": DEBUG,
}

# material needed for the external world
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin
external.Materials = {
    "OuterSpace": {
        "AtomicMass": 1.,
        "MassNumber": 1.01 * g / mole,
        "Density": 1.e-25 * g / cm3,
        "Pressure": 3.e-18 * pascal,
        "Temperature": 2.73 * kelvin,
    },
}

# here embedding of the geometry takes place
from Gaussino.Simulation import SimPhase
SimPhase().ExternalDetectorEmbedder = "Testing"

# Write to GDML
SimPhase().ExportGDML = {
    "GDMLFileName": "ExternalCube.gdml",
    "GDMLFileNameOverwrite": True,
    "GDMLExportEnergyCuts": True,
    "GDMLExportSD": True,
}

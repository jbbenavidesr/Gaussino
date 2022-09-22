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
# standard Gaussino
from Configurables import Gaussino
Gaussino().EvtMax = 1
Gaussino().EnableHive = True
Gaussino().ThreadPoolSize = 1
Gaussino().EventSlots = 1

from Configurables import GiGaMT
GiGaMT().NumberOfWorkerThreads = 1

# some dumb generation, not important, just fast
from Gaussino.Generation import GenPhase
GenPhase().ParticleGun = True
GenPhase().ParticleGunUseDefault = False

# Particle Gun On
# shoots one 1 GeV e- along z-axis
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
pgun.FixedMomentum.PdgCodes = [11]
from Configurables import FlatNParticles
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 1
pgun.FlatNParticles.MaxNParticles = 1

from Gaussino.Simulation import SimPhase
SimPhase().PhysicsConstructors = [
    "GiGaMT_G4EmStandardPhysics",
]

# adding external detectors
from Configurables import ExternalDetectorEmbedder
external = ExternalDetectorEmbedder("Testing")
from GaudiKernel.SystemOfUnits import m
from Gaudi.Configuration import DEBUG

# plain/testing geometry service
external.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
    "WorldLogicalVolumeName": "NewWorldLvol",
    "WorldPhysicalVolumeName": "NewWorldPvol",
    "OutputLevel": DEBUG,
}

# material needed for the external world
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin
external.Materials = {
    "OuterSpace": {
        "AtomicNumber": 1.,
        "MassNumber": 1.01 * g / mole,
        "Density": 1.e-25 * g / cm3,
        "Pressure": 3.e-18 * pascal,
        "Temperature": 2.73 * kelvin,
    },
}

# here embedding of the geometry takes place
from Gaussino.Simulation import SimPhase
SimPhase().ExternalDetectorEmbedder = "Testing"

# Import GDML
import os
root = os.getenv("GIGAMTGEOROOT")
SimPhase().ImportGDML = [
    {
        "GDMLFileName": root + "/tests/data/cube_to_import.gdml",
        "OutputLevel": DEBUG,
    },
]

SimPhase().SensDetMap = {
    "MCCollectorSensDet/CubeToImportSDet": ["CubeToImportLVol"],
}

from Configurables import MCCollectorSensDet
MCCollectorSensDet(
    "GiGaMT.DetConst.CubeToImportSDet",
    OutputLevel=DEBUG,
    PrintStats=True,
)

# Export GDML
SimPhase().ExportGDML = {
    "GDMLFileName": "cube_to_export.gdml",
    "GDMLFileNameOverwrite": True,
    "GDMLExportEnergyCuts": True,
    "GDMLExportSD": True,
    "GDMLAddReferences": True,
}

# Parallel Geometry

## Parallel Cube

Here's an example of how you can embed an empty cube in a parallel world that will also be activated as an `MCCollector` type of sensitive detector:

```python
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
GenPhase().ParticleGunUseDefault = True

from Gaussino.Simulation import SimPhase
SimPhase().ParallelGeometry = True

from Configurables import ExternalDetectorEmbedder
parallel_embedder = ExternalDetectorEmbedder("CubeEmbedder")


# sets up the mass geometry
parallel_embedder.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
    "OutputLevel": DEBUG,
}

# material needed for the external world
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin
parallel_embedder.Materials = {
    "OuterSpace": {
        "AtomicNumber": 1.,
        "MassNumber": 1.01 * g / mole,
        "Density": 1.e-25 * g / cm3,
        "Pressure": 3.e-18 * pascal,
        "Temperature": 2.73 * kelvin,
    },
}

parallel_embedder.Shapes = {
    "Cube": {
        "Type": "Cuboid",
        "xSize": 1. * m,
        "ySize": 1. * m,
        "zSize": 1. * m,
    },
}

parallel_embedder.Sensitive = {
    "Cube": {
        "Type": "MCCollectorSensDet",
    },
}

from Configurables import ParallelGeometry
ParallelGeometry().ParallelWorlds = {
    'ParallelCubeWorld': {
        'ExternalDetectorEmbedder': 'CubeEmbedder',
    },
}

ParallelGeometry().ParallelPhysics = {
    'ParallelCubeWorld': {
        'LayeredMass': False, # if true, then materials will be overridden down the stack
    },
}
```
## Mixed geometry 

Here you'll find 3 same-size sensitive detectors (of type `MCCollector`) each placed in a different world. One of them is placed in the mass world and is made out of the lead. The second one is made out of the vacuum and placed in a parallel world, right on top of the mass world. Finally, the last one is placed in another parallel world on top of the other worlds.

A single 1 GeV photon is released along the z-axis. Because the last world on the stack has `LayerdMass=False`, the material is not overridden and the photon "sees" vacuum as the material of the plane. As a result, it does not deposit any energy within its body. 

This example is also used as a test `mixed_geometry.qmt`. The test checks whether the hits generated in all three sensitive detectors are EXACTLY the same. They should be registered in the same place and have 0 energy deposit.

![mixed_geometry](/images/mixed_geometry.png)

```python
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
    'PrintStats': True,
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
        'ExportGDML': {
            'GDMLFileName': 'ParallelWorld1.gdml',
            'GDMLFileNameOverwrite': True,
            'GDMLExportSD': True,
            'GDMLExportEnergyCuts': True,
        },
    },
    'ParallelWorld2': {
        'ExternalDetectorEmbedder': 'ParallelEmbedder2',
        'ExportGDML': {
            'GDMLFileName': 'ParallelWorld2.gdml',
            'GDMLFileNameOverwrite': True,
            'GDMLExportSD': True,
            'GDMLExportEnergyCuts': True,
        },
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

SimPhase().ExportGDML = {
    'GDMLFileName': 'MassWorld.gdml',
    'GDMLFileNameOverwrite': True,
    'GDMLExportSD': True,
    'GDMLExportEnergyCuts': True,
}
```

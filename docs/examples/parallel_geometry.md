# Parallel Geometry

## Parallel Cube

Here's an example of how you can embed an empty cube in a parallel world that will also be activated as an `MCCollector` type of sensitive detector:

```python
from Gaudi.Configuration import importOptions
importOptions("$GAUSSINOOPTS/General/Events-1.py")
importOptions("$GAUSSINOOPTS/General/ConvertEDM.py")
importOptions("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")

from ExternalDetector.Materials import OUTER_SPACE
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
    "OuterSpace": OUTER_SPACE,
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
from Gaudi.Configuration import importOptions

importOptions("$GAUSSINOOPTS/General/Events-1.py")
importOptions("$GAUSSINOOPTS/General/ConvertEDM.py")
importOptions("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")

from GaudiKernel import SystemOfUnits as units
from ExternalDetector.Materials import (
    OUTER_SPACE,
    LEAD,
)
from Configurables import (
    GaussinoGeometry,
    ExternalDetectorEmbedder,
    ParallelGeometry,
)

# plain/testing geometry service
world = {
    'WorldMaterial': 'OuterSpace',
    'Type': 'ExternalWorldCreator',
}

# material needed for the external world
materials = {
    "OuterSpace": OUTER_SPACE,
    'Pb': LEAD,
}

# Generic options for all detectors
generic_shape = {
    'Type': 'Cuboid',
    'xPos': 0. * units.m,
    'yPos': 0. * units.m,
    'zPos': 5. * units.m,
    'xSize': 5. * units.m,
    'ySize': 5. * units.m,
    'zSize': 1. * units.m,
}

generic_sensitive = {
    'Type': 'MCCollectorSensDet',
    'RequireEDep': False,
    'OnlyForward': False,
    'PrintStats': True,
}

generic_hit = {
    'Type': 'GetMCCollectorHitsAlg',
}

# External detector embedders in mass & parallel geometry
mass_embedder = ExternalDetectorEmbedder('MassEmbedder')
# here embedding of the geometry takes place
GaussinoGeometry().ExternalDetectorEmbedder = "MassEmbedder"
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

GaussinoGeometry().ExportGDML = {
    'GDMLFileName': 'MassWorld.gdml',
    'GDMLFileNameOverwrite': True,
    'GDMLExportSD': True,
    'GDMLExportEnergyCuts': True,
}

```

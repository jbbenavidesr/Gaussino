# Custom Simulation

## Simple Cube with a fast simulation model

Here's an example of how you can embed an empty cube in a mass world that will also be activated as an `MCCollector` type of sensitive detector. `ImmediateDepositModel` will be activated within that cube volume. In this model we deposit all of the particle's energy immediately after it entered the custom simulation region.

```python
from Gaudi.Configuration import importOptions
importOptions("$GAUSSINOOPTS/General/Events-1.py")
importOptions("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")

# setting up external geometry service
from Configurables import (
    GaussinoSimulation,
    ExternalDetectorEmbedder,
    CustomSimulation,
)

GaussinoSimulation(
    CustomSimulation="ImmediateDepositCreator",
    ExternalDetectorEmbedder="VacuumCubeEmbedder"
)

CustomSimulation(
    "ImmediateDepositCreator",
    Model={
        "CubeImmediateDeposit": {
            "Type": "ImmediateDepositModel",
        },
    },
    Region={
        "CubeImmediateDeposit": {
            "SensitiveDetectorName": 'VacuumCubeSDet',
        }
    },
    Physics={
        'ParticlePIDs': [22],
    }
)

# adding external detectors
from GaudiKernel import SystemOfUnits as units
from ExternalDetector.Materials import OUTER_SPACE
ExternalDetectorEmbedder(
    "VacuumCubeEmbedder"
    Shapes={
        "VacuumCube": {
            "Type": "Cuboid",
            "xSize": 1. * m,
            "ySize": 1. * m,
            "zSize": 1. * m,
        },
    },
    Sensitive={
        "VacuumCube": {
            "Type": "MCCollectorSensDet",
            'PrintStats': True,
        },
    },
    World={
        "WorldMaterial": "OuterSpace",
        "Type": "ExternalWorldCreator",
    },
    Materials={
        "OuterSpace": OUTER_SPACE,
    },
)
```

## Custom simulation in mixed geometry

Here you'll find 2 same-size sensitive detectors (of type `MCCollector`) each placed in different worlds. One of them is placed in the mass world (filled with almost vacuum). The second one is made out of the lead and placed in a parallel world, right on top of the mass world. The one in the parallel world (with `LayeredMass=True`) is a bit closer to the interaction point and hence no energy deposit will be registered in the mass world. The detector in the parallel world will capture all the energy of the photon because it is using the `ImmediateDepositModel`.

```python
from Gaudi.Configuration import importOptions
importOptions("$GAUSSINOOPTS/General/Events-1.py")
importOptions("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")

from Configurables import (
    GaussinoSimulation,
    GaussinoGeometry,
    ExternalDetectorEmbedder,
    CustomSimulation,
    ParallelGeometry,
)

from ExternalDetector.Materials import (
    OUTER_SPACE,
    LEAD,
)

from GaudiKernel import SystemOfUnits as units

GaussinoSimulation(
    CustomSimulation="MassFastSimulation",
)

GaussinoGeometry(
    ExternalDetectorEmbedder="MassGeometryEmbedder",
)

ExternalDetectorEmbedder(
    "MassGeometryEmbedder",
    Shapes={
        "MassVacuumCalo": {
            "Type": "Cuboid",
            "zPos": 10. * units.m,
            "xSize": 10. * units.m,
            "ySize": 10. * units.m,
            "zSize": 2. * units.m,
        },
    },
    Sensitive={
        "MassVacuumCalo": {
            "Type": "MCCollectorSensDet",
            'PrintStats': True,
        },
    },
    World={
        "WorldMaterial": "OuterSpace",
        "Type": "ExternalWorldCreator",
    },
    Materials={
        "OuterSpace": OUTER_SPACE,
        "Pb": LEAD,
    }
)

CustomSimulation(
    "MassFastSimulation",
    Model={
        'MassImmediateDeposit': {
            'Type': 'ImmediateDepositModel',
        }
    },
    Region={
        'MassImmediateDeposit': {
            'SensitiveDetectorName': 'MassVacuumCaloSDet',
        }
    },
    Physics={
        'ParticlePIDs': [22],
    },
)

ExternalDetectorEmbedder(
    "ParallelGeometryEmbedder",
    Shapes={
        "ParallelLeadCalo": {
            "Type": "Cuboid",
            "zPos": 9.99999 * units.m,
            "xSize": 10. * units.m,
            "ySize": 10. * units.m,
            "zSize": 2 * units.m,
            "MaterialName": "Pb",
        },
    },
    Sensitive={
        "ParallelLeadCalo": {
            "Type": "MCCollectorSensDet",
            'PrintStats': True,
        },
    },
)

ParallelGeometry(
    ParallelWorlds={
        'ParallelWorld': {
            'ExternalDetectorEmbedder': 'ParallelGeometryEmbedder',
            'CustomSimulation': 'ParallelFastSimulation',
        },
    },
    ParallelPhysics={
        'ParallelWorld': {
            'LayeredMass': True,
            'ParticlePIDs': [22],
        },
    }
)

CustomSimulation(
    'ParallelFastSimulation',
    Model={
        'ParallelImmediateDeposit': {
            'Type': 'ImmediateDepositModel',
        }
    },
    Region={
        'ParallelImmediateDeposit': {
            'SensitiveDetectorName': 'ParallelLeadCaloSDet',
        }
    },
    Physics={
        'ParticlePIDs': [22],
    },
)
```

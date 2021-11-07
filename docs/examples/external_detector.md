# External Detector

## External Cube

This example shows an example of a 1m x 1m x 1m cube detector and embeds it in an external world at (0, 0, 0). This is also a test called `external_cube.qmt`. Here's a graphical visualization:

![cube](/images/cuboid.png)

```python
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

# Write to GDML
SimPhase().ExportGDML = {
    "GDMLFileName": "ExternalCube.gdml",
    "GDMLFileNameOverwrite": True,
    "GDMLExportEnergyCuts": True,
    "GDMLExportSD": True,
}
```

## [Gauss] External Tracker Planes

This test build `PLANES_NO=4` 10m x 10m x 0.01m planes inside LHCb geometry with no other detectors. Each plane is a sensitive detecto of type `GiGaSensDetTracker`. This example will be moved to Gauss's documentation.

![externaltrackerplanes](/images/external_tracker_planes.png)

```python
# number of planes to generate
PLANES_NO = 4

from Configurables import Gauss
Gauss().EvtMax = 1
Gauss().EnableHive = True
Gauss().ThreadPoolSize = 2
Gauss().EventSlots = 2

from Configurables import GenRndInit
GenRndInit().FirstEventNumber = 3000

from Configurables import CondDB
CondDB().Upgrade = True

from Configurables import LHCbApp
LHCbApp().DataType = "Upgrade"
LHCbApp().DDDBtag = "upgrade/dddb-20210617"
LHCbApp().CondDBtag = "upgrade/sim-20210617-vc-mu100"

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

# empty LHCb
from Gauss.Geometry import LHCbGeo
LHCbGeo().DetectorGeo = {"Detectors": []}
LHCbGeo().DetectorSim = {"Detectors": []}
LHCbGeo().DetectorMoni = {"Detectors": []}

from Gauss.Geometry import BeamPipe
BeamPipe().State = "BeamPipeOff"

# adding external detectors
from Configurables import ExternalDetectorEmbedder
external = ExternalDetectorEmbedder("Testing")
# generating planes
from GaudiKernel.SystemOfUnits import m
from Gaudi.Configuration import DEBUG
shapes = {}
sensitive = {}
hit = {}
moni = {}
for plane in range(PLANES_NO):
    shapes['Plane{}'.format(plane + 1)] = {
        "Type": "Cuboid",
        "xPos": 0. * m,
        "yPos": 0. * m,
        "zPos": (1. + plane) * m,
        "xSize": 10. * m,
        "ySize": 10. * m,
        "zSize": .01 * m,
        "OutputLevel": DEBUG,
    }
    sensitive['Plane{}'.format(plane + 1)] = {
        "Type": "GiGaSensDetTracker",
        "RequireEDep": False,
        "OutputLevel": DEBUG,
    }
    hit['Plane{}'.format(plane + 1)] = {
        "Type": "GetTrackerHitsAlg",
        "OutputLevel": DEBUG,
    }
    moni['Plane{}'.format(plane + 1)] = {
        "Type": "MCHitMonitor",
        "HitsPropertyName": "mcPathString",
        "OutputLevel": DEBUG,
    }
external.Shapes = shapes
external.Sensitive = sensitive
external.Hit = hit
external.Moni = moni
# embedding of the geometry in GaussGeo
from Gauss.Geometry import LHCbGeo
LHCbGeo().ExternalDetectorEmbedder = "Testing"
```


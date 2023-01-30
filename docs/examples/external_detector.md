# External Detector

## External Cube

This example shows an example of a 1m x 1m x 1m cube detector and embeds it in an external world at (0, 0, 0). This is also a test called `external_cube.qmt`. Here's a graphical visualization:

![cube](/images/cuboid.png)

```python
from Gaudi.Configuration import importOptions
importOptions("$GAUSSINOOPTS/General/Events-1.py")
importOptions("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")
importOptions("$GAUSSINOOPTS/Geometry/ExternalDetector-SimpleCube.py")
```

## Tracker planes

## External Tracker Planes

This test build `PLANES_NO=4` 10m x 10m x 0.01m planes. Each plane is a sensitive detecto of type `MCCollectorSensDet`.

![externaltrackerplanes](/images/external_tracker_planes.png)

```python
from Gaudi.Configuration import DEBUG
from GaudiKernel import SystemOfUnits as units
from Configurables import ExternalDetectorEmbedder
from Gaudi.Configuration import importOptions

importOptions("$GAUSSINOOPTS/General/Events-1.py")
importOptions("$GAUSSINOOPTS/General/ConvertEDM.py")
importOptions("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")

# number of planes to generate
PLANES_NO = 4

# adding external detectors
external = ExternalDetectorEmbedder("ExternalDetectorEmbedder_0")
# generating planes
shapes = {}
sensitive = {}
hit = {}
moni = {}
for plane in range(PLANES_NO):
    shapes['Plane{}'.format(plane + 1)] = {
        "Type": "Cuboid",
        "xPos": 0. * units.m,
        "yPos": 0. * units.m,
        "zPos": (1. + plane) * units.m,
        "xSize": 10. * units.m,
        "ySize": 10. * units.m,
        "zSize": .01 * units.mm,
        "OutputLevel": DEBUG,
    }
    sensitive['Plane{}'.format(plane + 1)] = {
        "Type": "MCCollectorSensDet",
        "RequireEDep": False,
        "PrintStats": True,
        "OutputLevel": DEBUG,
    }
    hit['Plane{}'.format(plane + 1)] = {
        "Type": "GetMCCollectorHitsAlg",
        "OutputLevel": DEBUG,
    }
    moni['Plane{}'.format(plane + 1)] = {
        "Type": "MCCollector",
        "HitsPropertyName": "CollectorHits",
        "OutputLevel": DEBUG,
    }
external.Shapes = shapes
external.Sensitive = sensitive
external.Hit = hit
external.Moni = moni

from Configurables import NTupleSvc
NTupleSvc().Output = [
    "FILE1 DATAFILE='CaloCollector.root' TYP='ROOT' OPT='NEW'"
]
```


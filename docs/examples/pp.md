## _pp_-collisions

Below you will find options needed to run _pp_-collisions with Gaussino:

```python
from GaudiKernel import SystemOfUnits as units
from ExternalDetector.Materials import OUTER_SPACE
from Configurables import (
    Gaussino,
    GaussinoGeneration,
    GaussinoGeometry,
    ExternalDetectorEmbedder,
)
Gaussino().EvtMax = 1

GaussinoSimulation().PhysicsConstructors += [
    "GiGaMT_G4EmStandardPhysics",
    "GiGaMT_G4HadronPhysicsFTFP_BERT",
]

GaussinoGeneration(
    SampleGenerationTool="MinimumBias",
    ProductionTool="Pythia8ProductionMT",
    PileUpTool="FixedLuminosityWithSvc",
    BeamMomentum=900 * units.GeV,
    RevolutionFrequency=11.245 * units.kilohertz,
    TotalCrossSection=65.3 * units.millibarn,
    InteractionPosition=[0.0] * 3,
    BeamHCrossingAngle=0.0 * units.mrad,
    BeamVCrossingAngle=0.0 * units.mrad,
    BeamLineAngles=[0.0, 0.0],
)

emb_name = "ExternalDetectorEmbedder"
GaussinoGeometry().ExternalDetectorEmbedder = emb_name
external = ExternalDetectorEmbedder(emb_name)

# plain/testing geometry service
external.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
}

# material needed for the external world
external.Materials = {
    "OuterSpace": OUTER_SPACE,
}
```

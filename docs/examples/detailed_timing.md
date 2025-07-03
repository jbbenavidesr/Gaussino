# Detailed Timing Example

This example demonstrates how to set up detailed timing for a specific detector pattern in Gaussino.
The whole purpose of the detailed timing is to measure the time taken by Geant4 to process each step in the simulation.
You will need to define a detector pattern like the one below, which is a cube in this case based on the setup provided in section [](../getting_started/first_simulation_job.md#running-your-first-simulation)


Add the following code to your options file:
```python
from Configurables import GaussinoGeometry
GaussinoGeometry(
    DetailedTimingOpts={
        "DetectorPatterns": {
            "Embedder_Cube": [
                cube_name, # first pattern
            ]
        },
    }
)
```

Once the simulation stops, you will see the following output in the Gaussino log:
```
[...]
GiGaMT.GiGaActi...   INFO Detailed timing for DetectorTiming [s]:
Name                          Mean           Thread 0
Embedder_Cube                 8.713e+01      8.713e+01

GiGaMT.GiGaActi...   INFO Detailed timing for ParticleDetectorTiming [s]:
Name                          Mean           Thread 0
Embedder_Cube,e-              7.666e+01      7.666e+01
Embedder_Cube,e+              9.217e+00      9.217e+00
Embedder_Cube,gamma           1.253e+00      1.253e+00
Embedder_Cube,Pb208           7.732e-05      7.732e-05
Embedder_Cube,Pb207           4.098e-05      4.098e-05
Embedder_Cube,Pb206           1.854e-05      1.854e-05

GiGaMT.GiGaActi...   INFO Detailed timing for ProcessTiming [s]:
Name                          Mean           Thread 0
eIoni                         8.493e+01      8.493e+01
phot                          9.651e-01      9.651e-01
eBrem                         9.368e-01      9.368e-01
Transportation                7.527e-01      7.527e-01
compt                         1.975e-01      1.975e-01
Rayl                          4.790e-02      4.790e-02
conv                          4.138e-02      4.138e-02
annihil                       1.584e-02      1.584e-02
CoulombScat                   3.137e-04      3.137e-04
ionIoni                       1.368e-04      1.368e-04
msc                           1.859e-05      1.859e-05

GiGaMT.GiGaActi...   INFO Detailed timing for GeneralTiming [s]:
Name                          Mean           Thread 0
Time in all detectors         8.788e+01      8.788e+01
Time in known detectors       8.713e+01      8.713e+01
[...]
```

In addition to the output in the log, 4 CSV files will be created in the working directory:
- `DetectorTiming.csv`
- `ParticleDetectorTiming.csv`
- `ProcessTiming.csv`
- `GeneralTiming.csv`

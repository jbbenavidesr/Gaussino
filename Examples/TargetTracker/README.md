# TargetTracker Example

## Overview

The TargetTracker example is based on Geant4's **B2a example**, demonstrating a simplified fixed-target tracking detector geometry. This example serves multiple purposes:

- **Learning**: Shows how to use the Gaussino framework for tracking detector simulations
- **AdePT Integration**: Demonstrates how to integrate AdePT or G4HepEm physics
- **Benchmark**: Provides a simple, reproducible setup for testing new features (like the AdePT integration)

## Physics Concepts

### Fixed-Target Tracker

A fixed-target tracker consists of:
- **Target**: Dense material (e.g., lead) where particles interact and produce secondaries
- **Tracking chambers**: Detectors arranged downstream to track particle trajectories

This design is used in many fixed-target experiments to study:
- Particle scattering and interactions
- Secondary particle production
- Track reconstruction and momentum measurement

### Default Configuration

- **Target**: 5 cm long, 2.5 cm radius lead cylinder (G4_Pb)
- **5 tracking chambers** with increasing radius
  - Spacing: 80 cm between chambers
  - Width: 20 cm each chamber
  - Material: Xenon (G4_Xe)
- **Total tracker length**: ~4.8 m

The chambers have increasing radius to track diverging particles from the target.

## Directory Structure

```
TargetTracker/
├── CMakeLists.txt                    # Build configuration
├── README.md                         # This file
├── include/SimpleCollector/          # C++ headers for sensitive detectors
├── src/                              # C++ implementation
│   ├── Lib/                          # Hit and sensitive detector classes
│   └── Components/                   # Gaudi components
├── python/TargetTracker/             # Python configuration modules
│   ├── tracker_geometry.py           # Geometry setup functions
│   ├── tracker_simulation.py         # Physics configuration
│   └── tracker_generation.py         # Particle gun setup
└── configs/                          # Example configuration scripts
    ├── geometry.py                   # Geometry configuration
    ├── generation.py                 # Particle generation
    ├── geant4_simulation.py          # Standard Geant4 physics
    └── adept_simulation.py           # AdePT/G4HepEm physics
```

## How to Run

### Basic Geant4 Simulation

Run a standard Geant4 simulation with electrons:

```bash
cd Examples/TargetTracker
gaudirun.py configs/geometry.py \
            configs/generation.py \
            configs/geant4_simulation.py
```

### With Custom Parameters

Use environment variables to customize the simulation:

```bash
export PARTICLE_TYPE=electron          # electron, gamma, or proton
export PARTICLE_ENERGY_MEV=100         # Energy in MeV
export PARTICLES_PER_EVENT=50          # Particles per event
export NUMBER_OF_EVENTS=100            # Total events
export NUMBER_OF_THREADS=4             # Thread pool size

gaudirun.py configs/geometry.py \
            configs/generation.py \
            configs/geant4_simulation.py
```

### AdePT/G4HepEm Integration

To use **G4HepEm** (optimized electromagnetic physics):

```bash
gaudirun.py configs/geometry.py \
            configs/generation.py \
            configs/adept_simulation.py
```

To use **AdePT** (GPU-accelerated physics):

```bash
export USE_ADEPT=true
export ADEPT_VERBOSITY=1
export ADEPT_TRACK_SLOTS=14
export ADEPT_HIT_SLOTS=40

gaudirun.py configs/geometry.py \
            configs/generation.py \
            configs/adept_simulation.py
```

## Environment Variables

### Particle Generation
- `PARTICLE_TYPE`: Particle to simulate (`electron`, `gamma`, `proton`). Default: `electron`
- `PARTICLE_ENERGY_MEV`: Particle energy in MeV. Default: `10.0`
- `PARTICLES_PER_EVENT`: Number of particles per event. Default: `100`

### Simulation Control
- `NUMBER_OF_EVENTS`: Number of events to simulate. Default: `10`
- `NUMBER_OF_THREADS`: Number of threads for multithreading. Default: `1`

### AdePT Configuration
- `USE_ADEPT`: Set to `true` to use AdePT instead of G4HepEm. Default: `false`
- `ADEPT_VERBOSITY`: AdePT verbosity level (0-5). Default: `0`
- `ADEPT_TRACK_SLOTS`: Track buffer size in millions. Default: `14`
- `ADEPT_HIT_SLOTS`: Hit buffer size in millions. Default: `40`

## Python API Usage

You can also use the Python modules directly in your own scripts:

```python
from GaudiKernel import SystemOfUnits as units
from TargetTracker.tracker_geometry import (
    set_target_tracker,
    tracker_default_options,
)
from TargetTracker.tracker_simulation import set_full_simulation
from TargetTracker.tracker_generation import set_particle_gun

# Customize geometry options
my_options = tracker_default_options.copy()
my_options["n_chambers"] = 10
my_options["chamber_material"] = "G4_Pb"  # Use lead chambers

# Set up the simulation
set_target_tracker(my_options, emb_name="MyTracker")
set_particle_gun(my_options, particle_type=2212, particle_energy=1*units.GeV)
set_full_simulation()

# Configure Gaussino...
```

## AdePT Integration Tutorial

This example demonstrates the key steps for integrating AdePT into Gaussino simulations:

### 1. **Physics List Replacement**

The `tracker_simulation.py` module shows how to replace standard Geant4 electromagnetic physics:

```python
# Standard Geant4
"GiGaMT_G4EmStandardPhysics_option2"

# With AdePT (GPU) or G4HepEm (optimized CPU)
"GiGaMT_G4EmStandardPhysics_option2_AdePT"   # or
"GiGaMT_G4EmStandardPhysics_option2_HepEm"
```

### 2. **AdePT Configuration**

When using AdePT, configure GPU parameters via Geant4 UI commands:

```python
GiGaMTRunManagerFAC("GiGaMT.GiGaMTRunManagerFAC").InitCommands = [
    "/adept/setVerbosity 0",
    "/adept/setCUDAStackLimit 8192",
    "/adept/setTrackInAllRegions true",
    "/adept/setMillionsOfTrackSlots 14",
    "/adept/setMillionsOfHitSlots 40",
]
```

### Parametric Studies

Vary parameters systematically:

```bash
for energy in 10 50 100 500 1000; do
    export PARTICLE_ENERGY_MEV=$energy
    gaudirun.py configs/geometry.py configs/generation.py \
                configs/geant4_simulation.py
done
```

## Physics Validation

The hit collections record:
- Position of hits in each chamber
- Energy deposition per hit
- Track ID for matching particles

Compare these distributions across different physics configurations to validate AdePT integration.

## References

- **Geant4 B2a example**: [Geant4 Examples](https://geant4-userdoc.web.cern.ch/Examples/basic.html)
- **AdePT project**: [GPU-accelerated particle transport](https://github.com/apt-sim/AdePT)
- **G4HepEm**: [Optimized electromagnetic physics library](https://github.com/mnovak42/g4hepem)
- **Gaussino documentation**: [Gaussino Docs](https://gaussino.docs.cern.ch/)

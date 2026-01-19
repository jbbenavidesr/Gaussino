# SamplingCalorimeter Example

## Overview

The SamplingCalorimeter example is based on Geant4's **B4c example**, demonstrating a simplified sampling calorimeter geometry. This example serves multiple purposes:

- **Learning**: Shows how to use the Gaussino framework for calorimeter simulations
- **AdePT Integration**: Demonstrates how to integrate AdePT or G4HepEm physics
- **Benchmark**: Provides a simple, reproducible setup for testing new features (like the AdePT integration)

## Physics Concepts

### Sampling Calorimeter

A sampling calorimeter measures particle energy by alternating layers of:
- **Absorber material** (dense, passive): Induces electromagnetic showers (e.g., lead)
- **Active detector material** (gap): Samples the shower energy (e.g., argon gas)

### Default Configuration

- **10 layers** of absorber-gap pairs
- **Absorber**: 10 mm lead (G4_Pb)
- **Gap**: 5 mm argon gas (G4_Ar)
- **Transverse size**: 10 cm × 10 cm
- **Total depth**: ~15 cm

## Directory Structure

```
SamplingCalorimeter/
├── CMakeLists.txt                    # Build configuration
├── README.md                         # This file
├── include/CalorimeterCollector/     # C++ headers for sensitive detectors
├── src/                              # C++ implementation
│   ├── Lib/                          # Hit and sensitive detector classes
│   └── Components/                   # Gaudi components
├── python/SamplingCalorimeter/       # Python configuration modules
│   ├── calorimeter_geometry.py       # Geometry setup functions
│   ├── calorimeter_simulation.py     # Physics configuration
│   ├── calorimeter_generation.py     # Particle gun setup
│   └── calorimeter_monitoring.py     # Histogram configuration
└── configs/                          # Example configuration scripts
    ├── geometry.py                   # Geometry configuration
    ├── generation.py                 # Particle generation
    ├── monitoring.py                 # Monitoring setup
    ├── geant4_simulation.py          # Standard Geant4 physics
    └── adept_simulation.py           # AdePT/G4HepEm physics
```

## How to Run

### Basic Geant4 Simulation

Run a standard Geant4 simulation with electrons:

```bash
cd Examples/SamplingCalorimeter
gaudirun.py configs/geometry.py \
            configs/generation.py \
            configs/monitoring.py \
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
            configs/monitoring.py \
            configs/geant4_simulation.py
```

### AdePT/G4HepEm Integration

To use **G4HepEm** (optimized electromagnetic physics):

```bash
gaudirun.py configs/geometry.py \
            configs/generation.py \
            configs/monitoring.py \
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
            configs/monitoring.py \
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
from SamplingCalorimeter.calorimeter_geometry import (
    set_sampling_calorimeter,
    calorimeter_default_options,
)
from SamplingCalorimeter.calorimeter_simulation import set_full_simulation
from SamplingCalorimeter.calorimeter_generation import set_particle_gun
from SamplingCalorimeter.calorimeter_monitoring import set_monitoring

# Customize geometry options
my_options = calorimeter_default_options.copy()
my_options["n_layers"] = 20
my_options["gap_material"] = "G4_Kr"  # Use krypton instead of argon

# Set up the simulation
set_sampling_calorimeter(my_options, emb_name="MyCalorimeter")
set_particle_gun(my_options, particle_type=22, particle_energy=50*units.GeV)
set_monitoring(my_options, particle_energy=50*units.GeV)
set_full_simulation()

# Configure Gaussino...
```

## AdePT Integration

This example demonstrates the key steps for integrating AdePT into Gaussino simulations:

### 1. **Physics List Replacement**

The `calorimeter_simulation.py` module shows how to replace standard Geant4 electromagnetic physics:

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

## Physics Validation

The monitoring output provides histograms for:
- Energy deposition in absorber layers
- Energy deposition in gap layers
- Track length in absorber
- Track length in gap

Compare these distributions across different physics configurations to validate AdePT integration.

## References

- **Geant4 B4c example**: [Geant4 Examples](https://geant4-userdoc.web.cern.ch/Examples/basic.html)
- **AdePT project**: [GPU-accelerated particle transport](https://github.com/apt-sim/AdePT)
- **G4HepEm**: [Optimized electromagnetic physics library](https://github.com/mnovak42/g4hepem)
- **Gaussino documentation**: [Gaussino Docs](https://gaussino.docs.cern.ch/)


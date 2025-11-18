###############################################################################
# (c) Copyright 2025 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

"""
Simulation configuration for TargetTracker example.

This module provides functions to configure the physics simulation,
including standard Geant4 physics and AdePT/G4HepEm integration.
"""

from GaudiKernel import SystemOfUnits as units
from Configurables import GaussinoSimulation, GiGaMTRunManagerFAC


def set_full_simulation(production_cuts=700 * units.micrometer):
    """
    Configure standard Geant4 simulation with FTFP_BERT physics list.

    This uses the standard Geant4 physics constructors for electromagnetic
    and hadronic physics, suitable for tracking detector simulations.

    Args:
        production_cuts: Production cuts for gamma, electron, positron (default: 700 μm)
    """
    GaussinoSimulation(
        PhysicsConstructors=[
            "GiGaMT_G4EmStandardPhysics_option2",
            "GiGaMT_G4EmExtraPhysics",
            "GiGaMT_G4DecayPhysics",
            "GiGaMT_G4HadronElasticPhysics",
            "GiGaMT_G4HadronPhysicsFTFP_BERT",
            "GiGaMT_G4StoppingPhysics",
            "GiGaMT_G4IonPhysics",
            "GiGaMT_G4NeutronTrackingCut",
        ],
        CutForElectron=production_cuts,
        CutForPositron=production_cuts,
        CutForGamma=production_cuts,
        DumpCutsTable=True,
    )


def set_adept_simulation(
    production_cuts=700 * units.micrometer,
    adept_verbosity=0,
    track_slots=14,
    hit_slots=40,
    use_adept=False,
):
    """
    Configure simulation with AdePT or G4HepEm integration.

    This replaces the standard electromagnetic physics with either AdePT
    (GPU-accelerated) or G4HepEm (optimized CPU) implementations.

    Args:
        production_cuts: Production cuts for gamma, electron, positron (default: 700 μm)
        adept_verbosity: AdePT verbosity level (default: 0)
        track_slots: AdePT track slot size in millions (default: 14)
        hit_slots: AdePT hit slot size in millions (default: 40)
        use_adept: If True, use AdePT; if False, use G4HepEm (default: False)
    """
    # Choose physics constructor
    em_physics = (
        "GiGaMT_G4EmStandardPhysics_option2_AdePT"
        if use_adept
        else "GiGaMT_G4EmStandardPhysics_option2_HepEm"
    )

    GaussinoSimulation(
        PhysicsConstructors=[
            em_physics,
            "GiGaMT_G4EmExtraPhysics",
            "GiGaMT_G4DecayPhysics",
            "GiGaMT_G4HadronElasticPhysics",
            "GiGaMT_G4HadronPhysicsFTFP_BERT",
            "GiGaMT_G4StoppingPhysics",
            "GiGaMT_G4IonPhysics",
            "GiGaMT_G4NeutronTrackingCut",
        ],
        CutForElectron=production_cuts,
        CutForPositron=production_cuts,
        CutForGamma=production_cuts,
        DumpCutsTable=True,
    )

    # Configure AdePT if using it
    if use_adept:
        GiGaMTRunManagerFAC("GiGaMT.GiGaMTRunManagerFAC").InitCommands = [
            f"/adept/setVerbosity {adept_verbosity}",
            "/adept/setCUDAStackLimit 8192",
            "/adept/setTrackInAllRegions true",
            f"/adept/setMillionsOfTrackSlots {track_slots}",
            f"/adept/setMillionsOfHitSlots {hit_slots}",
        ]

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
Monitoring configuration for SamplingCalorimeter example.

This module provides functions to configure monitoring histograms
for energy deposition and track length analysis.
"""

from GaudiKernel import SystemOfUnits as units
from Configurables import ApplicationMgr, CalorimeterMonitoring


def set_monitoring(
    geometry_opts,
    emb_name="SamplingCalorimeter",
    particle_energy=10 * units.MeV,
    particles_per_event=100,
):
    """
    Configure CalorimeterMonitoring algorithm for the sampling calorimeter.

    This sets up histograms to monitor energy deposition and track length
    in both the absorber and gap layers of the calorimeter.

    Args:
        geometry_opts: Dictionary with calorimeter geometry options
        emb_name: Name of the ExternalDetectorEmbedder (default: "SamplingCalorimeter")
        particle_energy: Energy of incident particles (for histogram ranges)
        particles_per_event: Number of particles per event (for histogram ranges)
    """
    # Extract geometry parameters
    n_layers = geometry_opts.get("n_layers", 10)
    absorber_thickness = geometry_opts.get("absorber_thickness", 10 * units.mm)
    gap_thickness = geometry_opts.get("gap_thickness", 5 * units.mm)

    # Calculate detector dimensions
    layer_thickness = absorber_thickness + gap_thickness
    calor_thickness = layer_thickness * n_layers

    # Define collection names
    layer_base_name = f"{emb_name}_Layer"
    absorber_name = f"{layer_base_name}_Absorber"
    gap_name = f"{layer_base_name}_Gap"

    # Configure monitoring algorithm
    moni = CalorimeterMonitoring(
        "CalorimeterMonitoring",
        AbsorberCollectionName=f"{absorber_name}SDet/Hits",
        GapCollectionName=f"{gap_name}SDet/Hits",
        MaxGapEnergy=particle_energy * particles_per_event,
        MaxGapLength=calor_thickness * particles_per_event,
        MaxAbsorberEnergy=particle_energy * particles_per_event,
        MaxAbsorberLength=calor_thickness * particles_per_event,
    )

    # Add to application manager
    ApplicationMgr().TopAlg.append(moni)

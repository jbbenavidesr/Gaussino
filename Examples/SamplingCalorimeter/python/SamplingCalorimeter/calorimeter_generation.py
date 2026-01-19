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
Particle generation configuration for SamplingCalorimeter example.

This module provides functions to configure particle guns for the
calorimeter simulation.
"""

from GaudiKernel import SystemOfUnits as units
from Configurables import (
    GaussinoGeneration,
    ParticleGun,
    FixedMomentum,
    FlatSmearVertex,
    FlatNParticles,
)


# Predefined generation options
generation_default_options = {
    "particle_type": 11,  # electron
    "particle_energy": 10 * units.MeV,
    "particles_per_event": 100,
}


def set_particle_gun(
    geometry_opts,
    particle_type=11,
    particle_energy=10 * units.MeV,
    particles_per_event=100,
    gun_position=None,
):
    """
    Configure a particle gun for the calorimeter simulation.

    The particle gun fires particles along the z-axis toward the calorimeter.
    By default, the gun is positioned at the front edge of the world volume.

    Args:
        geometry_opts: Dictionary with calorimeter geometry options (used to calculate gun position)
        particle_type: PDG code for particle type (default: 11 for electrons)
            Common values: 11 (electron), 22 (gamma), 2212 (proton)
        particle_energy: Energy of the particles (default: 10 MeV)
        particles_per_event: Number of particles to generate per event (default: 100)
        gun_position: Z-position of the gun (default: calculated from geometry_opts)
    """
    # Calculate gun position if not provided
    if gun_position is None:
        n_layers = geometry_opts.get("n_layers", 10)
        absorber_thickness = geometry_opts.get("absorber_thickness", 10 * units.mm)
        gap_thickness = geometry_opts.get("gap_thickness", 5 * units.mm)
        layer_thickness = absorber_thickness + gap_thickness
        calor_thickness = layer_thickness * n_layers
        world_length = 1.2 * calor_thickness
        gun_position = -0.5 * world_length

    # Enable particle gun
    GaussinoGeneration().ParticleGun = True
    pgun = ParticleGun("ParticleGun")

    # Configure momentum (fixed, along z-axis)
    pgun.addTool(FixedMomentum, name="FixedMomentum")
    pgun.ParticleGunTool = "FixedMomentum"
    pgun.FixedMomentum.px = 0.0 * units.GeV
    pgun.FixedMomentum.py = 0.0 * units.GeV
    pgun.FixedMomentum.pz = particle_energy
    pgun.FixedMomentum.PdgCodes = [particle_type]

    # Configure number of particles
    pgun.addTool(FlatNParticles, name="FlatNParticles")
    pgun.NumberOfParticlesTool = "FlatNParticles"
    pgun.FlatNParticles.MinNParticles = particles_per_event
    pgun.FlatNParticles.MaxNParticles = particles_per_event

    # Configure vertex position (fixed at gun position)
    pgun.addTool(FlatSmearVertex, name="FlatSmearVertex")
    pgun.FlatSmearVertex.xVertexMin = 0.0 * units.mm
    pgun.FlatSmearVertex.xVertexMax = 0.0 * units.mm
    pgun.FlatSmearVertex.yVertexMin = 0.0 * units.mm
    pgun.FlatSmearVertex.yVertexMax = 0.0 * units.mm
    pgun.FlatSmearVertex.zVertexMin = gun_position
    pgun.FlatSmearVertex.zVertexMax = gun_position

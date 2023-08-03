###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

from Configurables import FixedMomentum, FlatNParticles, MomentumRange, ParticleGun
from GaudiKernel import PhysicalConstants as constants
from GaudiKernel import SystemOfUnits as units


def set_particle_gun_momentum_range(
    min_particles_no=1,
    max_particles_no=1,
    pdg_codes=[],
    min_momentum=1.0 * units.GeV,
    max_momentum=1.0 * units.GeV,
    theta_min=0,
    theta_max=constants.pi,
):
    pgun = ParticleGun("ParticleGun")
    pgun.addTool(MomentumRange, name="MomentumRange")
    pgun.ParticleGunTool = "MomentumRange"
    pgun.addTool(FlatNParticles, name="FlatNParticles")
    pgun.NumberOfParticlesTool = "FlatNParticles"
    pgun.FlatNParticles.MinNParticles = min_particles_no
    pgun.FlatNParticles.MaxNParticles = max_particles_no
    pgun.MomentumRange.PdgCodes = pdg_codes
    pgun.MomentumRange.MomentumMin = min_momentum
    pgun.MomentumRange.MomentumMax = max_momentum
    pgun.MomentumRange.ThetaMin = theta_min
    pgun.MomentumRange.ThetaMax = theta_max


def set_particle_gun_fixed_momentum(
    min_particles_no=1,
    max_particles_no=1,
    pdg_codes=[],
    momentum_x=0.0 * units.GeV,
    momentum_y=10.0 * units.GeV,
    momentum_z=0.0 * units.GeV,
):
    pgun = ParticleGun("ParticleGun")
    pgun.addTool(FixedMomentum, name="FixedMomentum")
    pgun.ParticleGunTool = "FixedMomentum"
    pgun.addTool(FlatNParticles, name="FlatNParticles")
    pgun.NumberOfParticlesTool = "FlatNParticles"
    pgun.FlatNParticles.MinNParticles = min_particles_no
    pgun.FlatNParticles.MaxNParticles = max_particles_no
    pgun.FixedMomentum.px = momentum_x
    pgun.FixedMomentum.py = momentum_y
    pgun.FixedMomentum.pz = momentum_z
    pgun.FixedMomentum.PdgCodes = pdg_codes

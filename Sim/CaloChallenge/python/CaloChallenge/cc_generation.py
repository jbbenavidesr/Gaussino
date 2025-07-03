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

import math

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
    from Configurables import FlatNParticles, MomentumRange, ParticleGun

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
    momentum_y=0.0 * units.GeV,
    momentum_z=0.0 * units.GeV,
):
    from Configurables import FixedMomentum, FlatNParticles, ParticleGun

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


def set_particle_gun(
    opts: dict, type: str = "FixedMomentum", calo_type: str = "cylindrical", **kwargs
):
    from Configurables import GaussinoGeneration

    GaussinoGeneration().ParticleGun = True
    if type == "FixedMomentum":
        set_particle_gun_fixed_momentum(**kwargs)
    elif type == "MomentumRange":
        if calo_type == "cylindrical":
            kwargs.setdefault(
                "theta_min",
                constants.pi / 2.0
                - math.atan(
                    opts["detector_length"] / 2.0 / opts["detector_inner_radius"]
                ),
            )
            kwargs.setdefault(
                "theta_max",
                constants.pi / 2.0
                + math.atan(
                    opts["detector_length"] / 2.0 / opts["detector_inner_radius"]
                ),
            )
        elif calo_type == "planar":
            kwargs.setdefault("theta_min", 0.0)
            kwargs.setdefault(
                "theta_max",
                math.atan(
                    math.sqrt(
                        (opts["detector_x_size"] / 2.0) ** 2
                        + (opts["detector_y_size"] / 2.0) ** 2
                    )
                    / opts["detector_z_pos"]
                ),
            )
        else:
            raise NotImplementedError("Unsupported calorimeter type.")
        set_particle_gun_momentum_range(**kwargs)
    else:
        raise NotImplementedError("Unsupported particle gun.")

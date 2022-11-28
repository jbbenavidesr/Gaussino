###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from GaudiKernel import SystemOfUnits as units
from Configurables import (
    GaussinoGeneration,
    HepMCWriter,
    ParticleGun,
    FlatNParticles,
    MomentumRange,
)

GaussinoGeneration().WriteHepMC = True
HepMCWriter().Writer = "WriterAscii"

GaussinoGeneration().ParticleGun = True
pgun = ParticleGun()
# pgun.EventType = 53210205
pgun.addTool(MomentumRange, name="MomentumRange")
pgun.ParticleGunTool = "MomentumRange"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.FlatNParticles.MinNParticles = 1
pgun.FlatNParticles.MaxNParticles = 1
pgun.MomentumRange.PdgCodes = [-2112]
pgun.MomentumRange.MomentumMin = 2.0 * units.GeV
pgun.MomentumRange.MomentumMax = 100.0 * units.GeV
pgun.MomentumRange.ThetaMin = 0.015 * units.rad
pgun.MomentumRange.ThetaMax = 0.300 * units.rad

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
from Configurables import FixedMomentum, FlatNParticles, GaussinoGeneration, ParticleGun
from GaudiKernel import SystemOfUnits as units

GaussinoGeneration().ParticleGun = True
pgun = ParticleGun("ParticleGun")
pgun.addTool(FixedMomentum, name="FixedMomentum")
pgun.ParticleGunTool = "FixedMomentum"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.FlatNParticles.MinNParticles = 1
pgun.FlatNParticles.MaxNParticles = 1
pgun.FixedMomentum.px = 0.0 * units.GeV
pgun.FixedMomentum.py = 0.0 * units.GeV
pgun.FixedMomentum.pz = 1.0 * units.GeV
pgun.FixedMomentum.PdgCodes = [11]

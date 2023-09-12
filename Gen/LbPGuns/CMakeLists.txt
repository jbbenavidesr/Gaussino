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
#[=======================================================================[.rst:
Gen/LbPGuns
-----------
#]=======================================================================]
gaudi_add_header_only_library(LbPGunsLib
    LINK
        Gaudi::GaudiKernel
        Gaussino::NewRndLib
)

gaudi_add_module(LbPGuns
    SOURCES
    	src/Components/BeamShape.cpp
	src/Components/Cosmics.cpp
	src/Components/FixedMomentum.cpp
	src/Components/FlatNParticles.cpp
	src/Components/FlatPtRapidity.cpp
	src/Components/GaussianTheta.cpp
	src/Components/GenericGun.cpp
	src/Components/MaterialEval.cpp
	src/Components/MomentumRange.cpp
	src/Components/MomentumSpectrum.cpp
	src/Components/ParticleGun.cpp
    LINK
	Gaussino::GeneratorsLib
	Gaussino::Defaults
	Gaussino::HepMCUserLib
	Gaussino::LbPGunsLib
)

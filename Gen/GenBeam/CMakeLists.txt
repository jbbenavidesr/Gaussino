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
Gen/GenBeam
-----------
#]=======================================================================]
gaudi_add_header_only_library(GenBeamLib
    LINK
        Gaudi::GaudiKernel
)

gaudi_add_module(GenBeam
    SOURCES
    	src/Components/BeamInfoSvc.cpp
	src/Components/BeamSpotSmearVertexWithSvc.cpp
	src/Components/CollidingBeamsWithSvc.cpp
	src/Components/FixedLuminosityWithSvc.cpp
    LINK
        HepMC3::HepMC3
    	Gaudi::GaudiAlgLib
	LHCb::GenEvent #  TODO: [LHCb DEPENDENCY]
	Gaussino::HepMCUserLib
	Gaussino::NewRndLib
    	Gaussino::GenBeamLib
	Gaussino::GenInterfacesLib
)

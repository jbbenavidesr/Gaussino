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
Sim/GiGaMT
----------

This contains the fundamental GiGaMT service that sets up the entire mess in
its initialization and provides the function to call that handles the simulation.
Also there is the main simulation algorithm that actually uses the service
#]=======================================================================]

gaudi_add_library(GiGaMTLib
    SOURCES
        src/Lib/GiGaException.cpp
	src/Lib/GiGaUtil.cpp
    LINK
        PUBLIC
	    Gaussino::GiGaMTFactoriesLib
	    Gaussino::HepMCUserLib
	    Gaussino::SimInterfacesLib
)

gaudi_add_module(GiGaMT
    SOURCES
        src/Components/GiGaActionInitializer.cpp
	src/Components/GiGaAlg.cpp
	src/Components/GiGaMT.cpp
	src/Components/GiGaMTISetUpSvc.cpp
	src/Components/HepMC3ToMCTruthConverter.cpp
	src/Components/SkipSimAlg.cpp
    LINK
    	Geant4::G4vis_management
        Gaussino::GiGaMTCoreRunLib
        Gaussino::GiGaMTTruthLib
        Gaussino::GiGaMTLib
)

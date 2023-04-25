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
Sim/CustomSimulation
--------------------
#]=======================================================================]

gaudi_add_library(CustomSimulationLib
    SOURCES
        src/Lib/HitMaker.cpp
        src/Lib/Physics.cpp
    LINK
        PUBLIC
	    Geant4::G4physicslists
        Gaussino::GiGaMTCoreMessageLib
	    Gaussino::GiGaMTFactoriesLib
	    Gaussino::GiGaMTPhysFactoriesLib
)

gaudi_add_module(CustomSimulation
    SOURCES
        src/Components/DefaultPhysicsFactory.cpp
        src/Components/RegionFactory.cpp
    LINK
        Gaussino::CustomSimulationLib
)

gaudi_install(PYTHON)

gaudi_generate_confuserdb()

gaudi_add_pytest()

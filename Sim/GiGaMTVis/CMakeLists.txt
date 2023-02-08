###############################################################################
# (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
#[=======================================================================[.rst:
Sim/GiGaMTVis
-------------
#]=======================================================================]

gaudi_add_module(GiGaMTVis
    SOURCES
        src/GiGaTrajectoryTransverseMomentumFilter.cpp
        src/GiGaTrajectoryPseudorapidityFilter.cpp
        src/GiGaVisManager.cpp
    LINK
    	Geant4::G4event
    	Geant4::G4global
    	Geant4::G4graphics_reps
    	Geant4::G4vis_management
    	Geant4::G4modeling
    	Geant4::G4Tree
      Geant4::G4FR
      Geant4::G4visHepRep
      Geant4::G4OpenGL
        Gaudi::GaudiAlgLib
        Gaussino::GiGaMTCoreMessageLib
	Gaussino::GiGaMTFactoriesLib
	Gaussino::UtilsLib
)

gaudi_generate_confuserdb()

gaudi_install(PYTHON)

gaudi_add_tests(pytest)

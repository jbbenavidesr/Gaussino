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
Sim/GiGaMTFactories
-------------------
#]=======================================================================]

gaudi_add_header_only_library(GiGaMTFactoriesLib
    LINK
		Gaudi::GaudiKernel
		Gaudi::GaudiAlgLib
        Gaussino::GiGaMTCoreMessageLib
)


gaudi_add_module(GiGaMTRunFactories
    SOURCES
	src/Components/GiGaEventActionCommand.cpp
        src/Components/GiGaMTRunManagerFAC.cpp
	src/Components/GiGaRunActionCommand.cpp
	src/Components/GiGaWorkerPilotFAC.cpp
    LINK
        HepMC3::HepMC3
	Geant4::G4interfaces
        Gaussino::GiGaMTFactoriesLib
	Gaussino::GiGaMTCoreRunLib
)

gaudi_add_header_only_library(GiGaMTPhysFactoriesLib)


# define GiGaMTPhysFactories sources to possibly append G4HepEm
set(GIGA_MT_PHYSFACTORIES_SOURCES
  src/Components/GiGaMTModularPhysListFAC.cpp
  src/Components/GiGaMTG4PhysicsConstrFAC.cpp
  src/Components/GiGaMTG4PhysicsFactory.cpp
)

if(USE_G4HEPEM)
  list(APPEND GIGA_MT_PHYSFACTORIES_SOURCES
    src/GiGaMTEmOffloading/G4EmStandardPhysics_option2_HepEm.cc
    src/GiGaMTEmOffloading/GiGaG4HepEmTrackingManager.cc
  )
endif()

if(USE_ADEPT)
  list(APPEND GIGA_MT_PHYSFACTORIES_SOURCES
    src/GiGaMTEmOffloading/G4EmStandardPhysics_option2_AdePT.cc
  )
endif()

# define GiGaMTPhysFactories link libraries to possibly append G4HepEm
set(GIGA_MT_PHYSFACTORIES_LINK_LIBS
  CLHEP::CLHEP
  Geant4::G4run
  Geant4::G4physicslists
  Gaudi::GaudiAlgLib
  Gaussino::GiGaMTFactoriesLib
  Gaussino::GiGaMTCoreMessageLib
)

# if AdePT is available, G4HepEm is transitively available, else we link it manually
if(USE_G4HEPEM AND NOT USE_ADEPT)
  list(APPEND GIGA_MT_PHYSFACTORIES_LINK_LIBS
    G4HepEm::g4HepEm
    G4HepEm::g4HepEmData
    G4HepEm::g4HepEmInit
    G4HepEm::g4HepEmRun
  )
endif()

gaudi_add_module(GiGaMTPhysFactories
    SOURCES
    ${GIGA_MT_PHYSFACTORIES_SOURCES}
    LINK
    ${GIGA_MT_PHYSFACTORIES_LINK_LIBS}
)

if (USE_ADEPT)
  cuda_rdc_target_link_libraries(GiGaMTPhysFactories PUBLIC AdePT::AdePT_G4_integration)
endif()

target_include_directories(GiGaMTPhysFactories PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/GiGaMTEmOffloading
)

gaudi_add_header_only_library(GiGaMTDetFactoriesLib)


gaudi_add_module(GiGaMTDetFactories
    SOURCES
        src/Components/GiGaMTDetectorConstructionFAC.cpp
	src/Components/GDMLConstructionFactory.cpp
    LINK
    	CLHEP::Vector
	Geant4::G4digits_hits
	Geant4::G4geometry
	Geant4::G4global
	Geant4::G4persistency
	Geant4::G4run
	Gaudi::GaudiAlgLib
	XercesC::XercesC
	Gaussino::UtilsLib
	Gaussino::GiGaMTCoreDetLib
	Gaussino::GiGaMTFactoriesLib
	Gaussino::GiGaMTGeoLib
	Gaussino::SimInterfacesLib
)

gaudi_add_module(GiGaMTTruthFactories
    SOURCES
        src/Components/TruthActionFactories.cpp
    LINK
        Gaussino::GiGaMTFactoriesLib
	Gaussino::GiGaMTCoreMessageLib
	Gaussino::GiGaMTCoreRunLib
)

gaudi_add_library(GiGaMTMagnetFactoriesLib
    SOURCES
		src/Lib/MagneticField.cpp
    LINK
        PUBLIC
			Geant4::G4geometry
			Gaussino::GiGaMTFactoriesLib
			Gaussino::GiGaMTCoreMessageLib
)

gaudi_add_module(GiGaMTMagnetFactories
    SOURCES
		src/Components/Steppers.cpp
		src/Components/MagneticFieldManager.cpp
		src/Components/UniformMagneticField.cpp
    LINK
        Gaussino::GiGaMTMagnetFactoriesLib
)

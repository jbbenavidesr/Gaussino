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
################################################################################
# Package: GiGaMTFactories 
#
# This package will contain the factories to construct the various G4 objects.
# This is done to clearly separate the G4 world from any Gaudi configurables parts.
# All factories should be thread-safe and implement a constant method 'construct' which
# and returns the constructed type by value.
# FIXME: This requirement should probably be enforced somehow...
# 
#################################################################################
#[=======================================================================[.rst:
Sim/GiGaMTFactories
---------------------------
#]=======================================================================]

gaudi_add_header_only_library(GiGaMTFactoriesLib
    LINK
        Gaudi::GaudiKernel
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


gaudi_add_module(GiGaMTPhysFactories
    SOURCES
	src/Components/GiGaMTModularPhysListFAC.cpp
	src/Components/GiGaMTG4PhysicsConstrFAC.cpp
	src/Components/GiGaMTG4PhysicsFactory.cpp
    LINK
        CLHEP::CLHEP
        Geant4::G4run
        Geant4::G4physicslists
        Gaudi::GaudiAlgLib
        Gaussino::GiGaMTFactoriesLib
        Gaussino::GiGaMTCoreMessageLib
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
        src/Lib/FieldMgrBase.cpp
    LINK
        PUBLIC
	    CLHEP::CLHEP
	    Geant4::G4geometry
	    Gaudi::GaudiKernel
	    Gaudi::GaudiAlgLib
	    Gaussino::GiGaMTCoreMessageLib
)

gaudi_add_module(GiGaMTMagnetFactories
    SOURCES
        src/Components/MagFieldFromSvc.cpp
	src/Components/Steppers.cpp
	src/Components/ToolFieldMgr.cpp
	src/Components/ZeroFieldMgr.cpp
    LINK
        Gaussino::GiGaMTMagnetFactoriesLib
)

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
# Package: GiGaMTCore
#
# This package should contain all modified core Geant4 classes (e.g.
# run managers). While the Gaudi cmake plugins are used to handle this package,
# none of the modified classes should depend on anything Gaudi related.
#################################################################################
#[=======================================================================[.rst:
Sim/GiGaMTCore
--------------
#]=======================================================================]

gaudi_add_header_only_library(GiGaMTCoreUtilsLib
    LINK
        Gaudi::GaudiKernel
        Gaussino::HepMCUtilsLib
        Gaussino::HepMCUserLib
        Gaussino::NewRndLib
        Gaussino::UtilsLib
        Geant4::G4global
        HepMC3::HepMC3
)
gaudi_add_header_only_library(GiGaMTCoreDetLib
    LINK
        Gaudi::GaudiKernel
        Gaussino::HepMCUtilsLib
        Gaussino::HepMCUserLib
        Gaussino::NewRndLib
        Gaussino::UtilsLib
        Geant4::G4global
        HepMC3::HepMC3
)

gaudi_add_library(GiGaMTCoreMessageLib
    SOURCES
        src/Lib/GiGaMessage.cpp
    LINK
        Gaudi::GaudiKernel
        Geant4::G4global
        HepMC3::HepMC3
)

gaudi_add_library(GiGaMTCoreCutLib
    SOURCES
        src/Lib/GaussinoG4UserLimits.cpp
    LINK
        PUBLIC
            CLHEP::Vector
            CLHEP::Random
	    Geant4::G4global
            Gaussino::GiGaMTCoreMessageLib
)

gaudi_add_library(GiGaMTCoreRunLib
    SOURCES
        src/Lib/Common.cpp
        src/Lib/ConverterInfo.cpp
        src/Lib/G4EventProxy.cpp
	src/Lib/G4EventProxyAttribute.cpp
        src/Lib/GaussinoEventInformation.cpp
        src/Lib/GaussinoPrimaryParticleInformation.cpp
        src/Lib/GaussinoTrackInformation.cpp
	src/Lib/GiGaMTRunManager.cpp
	src/Lib/GiGaRunActionCommand.cpp
	src/Lib/GiGaWorkerPayload.cpp
	src/Lib/GiGaWorkerPilot.cpp
	src/Lib/GiGaWorkerRunManager.cpp
        src/Lib/Helpers.cpp
        src/Lib/LinkedParticle.cpp
        src/Lib/LinkedParticleHelpers.cpp
        src/Lib/MCTruthAttribute.cpp
        src/Lib/MCTruthConverter.cpp
        src/Lib/SimResultsAttribute.cpp
        src/Lib/TruthFlaggingTrackAction.cpp
        src/Lib/TruthStoringTrackAction.cpp
        src/Lib/ZMaxPlane.cpp
    LINK
        PUBLIC
	    HepMC3::HepMC3
	    HepMC3::search
	    Geant4::G4event
	    Geant4::G4geometry
	    Geant4::G4global
	    Geant4::G4intercoms
	    Geant4::G4particles
	    Geant4::G4processes
	    Geant4::G4run
	    Geant4::G4track
	    Geant4::G4tracking
	    LHCb::MCEvent         # TODO: [LHCb DEPENDENCY]
	    Gaussino::HepMCUtilsLib
	    Gaussino::NewRndLib
            Gaussino::GiGaMTCoreUtilsLib
            Gaussino::GiGaMTCoreMessageLib
            Gaussino::GiGaMTCoreDetLib
)

gaudi_add_pytest()

gaudi_add_executable(test_zMaxTilt
    SOURCES tests/test_zMaxTilt.cpp
    LINK
    	Boost::unit_test_framework
	Gaudi::GaudiKernel
	Gaussino::GiGaMTCoreRunLib
    TEST
)

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
Sim/GiGaMTExamples/FCCTest
--------------------------
#]=======================================================================]
################################################################################
# Package: FCCTest
# Adapted from Gaussino FCC TestGeometry package: https://gitlab.cern.ch/jlingema/Gauss/tree/refactor/
# Original author: Anna Zaborowska
#
################################################################################

gaudi_add_library(FCCTestLib
    SOURCES
        src/Lib/CalorimeterHit.cpp
	src/Lib/CalorimeterSD.cpp
    LINK
        PUBLIC
	    Gaussino::GiGaMTCoreRunLib
	    Gaussino::SimInterfacesLib
)

gaudi_add_module(FCCTestPlugins
    SOURCES
        src/Components/G4GdmlTestDetector.cpp
	src/Components/G4SaveTestCalHits.cpp
    LINK
    	Geant4::G4persistency
        Gaussino::FCCTestLib
    	Gaussino::GiGaMTFactoriesLib
)

# FIXME: this test is stupid, what does it test?
#gaudi_add_tests(GeantGdmlWithSD
#    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#    FRAMEWORK tests/options/geant_fullsim_gdml.py
#    #)

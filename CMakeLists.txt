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
cmake_minimum_required(VERSION 3.15)

set(LCG_LAYER LHCB_7 CACHE STRING "Specific set of version to use")
option(LCG_USE_GENERATORS "enable/disable use of LCG generators" ON)

project(Gaussino VERSION 0.1
	LANGUAGES CXX Fortran)

# Enable testing with CTest/CDash
include(CTest)

list(PREPEND CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake
)

set(WITH_Gaussino_PRIVATE_DEPENDENCIES TRUE)
include(GaussinoDependencies)

gaussino_initialize_configuration()

if (NOT USE_DD4HEP)
    set(GAUSSINO_IGNORE_SUBDIRS Sim/GiGaMTDD4hep)
endif()

gaussino_add_subdirectories(
    Defaults
    EDMdev/MCTruthToEDM
    Gen/GenBeam
    Gen/Generators
    Gen/GenInterfaces
    Gen/LbPGuns
    Gen/LbPythia8
    Gen/RndInit
    HepMCUser
    Moni/GaussMonitor
    NewRnd
    Sim/ExternalDetector
    Sim/Gaussino
    Sim/GiGaMT
    Sim/GiGaMTCore
    Sim/GiGaMTDD4hep
    Sim/GiGaMTDebug
    Sim/GiGaMTExamples/FCCTest
    Sim/GiGaMTExamples/IronBox
    Sim/GiGaMTExamples/LHCbFromGDML
    Sim/GiGaMTFactories
    Sim/GiGaMTGeo
    Sim/GiGaMTReDecay
    Sim/GiGaMTTruth
    Sim/MCCollector
    Sim/ParallelGeometry
    Sim/SimInterfaces
    Utils
)

gaudi_install(CMAKE
    # helpers to find external projects
    cmake/FindHepMC3.cmake
    cmake/FindPythia8.cmake
    cmake/GaussinoConfigUtils.cmake
    cmake/GaussinoConfig.cmake.in
)

gaussino_finalize_configuration()

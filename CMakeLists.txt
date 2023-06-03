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

option(CUSTOMSIM "Using custom simulations" ON)
set(Gaussino_PERSISTENT_OPTIONS CUSTOMSIM)

set(WITH_Gaussino_PRIVATE_DEPENDENCIES TRUE)
include(GaussinoDependencies)

gaussino_initialize_configuration()

# check if we have DD4hep support
if (USE_DD4HEP)
    set(DD4hepDirsName DD4hepDirs)
else()
    set(DD4hepDirsName GAUSSINO_IGNORE_SUBDIRS)
endif()
list(APPEND ${DD4hepDirsName} Sim/GiGaMTDD4hep)

if(CUSTOMSIM)
    set(CustomSimulationDirsName CustomSimulationDirs)
else()
    set(CustomSimulationDirsName GAUSSINO_IGNORE_SUBDIRS)
endif()
list(APPEND ${CustomSimulationDirsName}
    Sim/CustomSimulation
    Sim/GiGaMTExamples/CustomSimulationExamples
    Sim/GiGaMTExamples/G4Par04
)

gaussino_env(
    SET GAUSSINOOPTS ${PROJECT_SOURCE_DIR}/Sim/Gaussino/options
    # FIXME: use non-LHCb LHAPDFSets
    SET LHAPDF_DATA_PATH /cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/LHAPDFSets/v62r3/data
    SET PYTHIA8XML ${PYTHIA8_XML}
    SET PYTHIA8DATA ${PYTHIA8_XML}
)

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
    ${CustomSimulationDirs}
    Sim/Gaussino
    Sim/GiGaMT
    Sim/GiGaMTCore
    ${DD4hepDirs}
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
    cmake/FindPythia8.cmake
    cmake/GaussinoConfigUtils.cmake
    cmake/GaussinoConfig.cmake.in
)

gaussino_finalize_configuration()

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

set(LCG_LAYER LHCB_8test CACHE STRING "Specific set of version to use")
option(LCG_USE_GENERATORS "enable/disable use of LCG generators" ON)

project(Gaussino VERSION 1.1
	LANGUAGES CXX Fortran)

# Enable testing with CTest/CDash
include(CTest)

list(PREPEND CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake
)

option(CUSTOMSIM "Using custom simulations" ON)
option(GSINO_USE_TORCH "Use pyTorch C++ API extenstions" ON)
option(GSINO_USE_ONNXRUNTIME "Use ONNXRuntime C++ API extenstions" ON)

# Disable automatically PyTorch and ONNXRuntime on ARM
# (as long as they are provided by LCG)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    if(GSINO_USE_TORCH)
        message(INFO "Torch in Gaussino is not supported on ARM")
        set(GSINO_USE_TORCH OFF)
    endif()
    if(GSINO_USE_ONNXRUNTIME)
        message(INFO "ONNXRuntime in Gaussino is not supported on ARM")
        set(GSINO_USE_ONNXRUNTIME OFF)
    endif()
endif()

list(APPEND Gaussino_PERSISTENT_OPTIONS
    CUSTOMSIM
    GSINO_USE_TORCH
    GSINO_USE_ONNXRUNTIME
)

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
    Examples/CustomSimulationExamples
    Examples/SamplingCalorimeter
    Examples/TargetTracker
    Sim/CaloChallenge
)

if(GSINO_USE_ONNXRUNTIME)
    set(ONNXRuntimeDirsName ONNXRuntimeDirs)
else()
    set(ONNXRuntimeDirsName GAUSSINO_IGNORE_SUBDIRS)
endif()
list(APPEND ${ONNXRuntimeDirsName}
    GaussinoML/GaussinoONNX
)

if(GSINO_USE_TORCH)
    set(TorchDirsName TorchDirs)
else()
    set(TorchDirsName GAUSSINO_IGNORE_SUBDIRS)
endif()
list(APPEND ${TorchDirsName}
    GaussinoML/GaussinoTorch
)

gaussino_env(
    SET GAUSSINOOPTS ${PROJECT_SOURCE_DIR}/Sim/Gaussino/options
    # FIXME: use non-LHCb LHAPDFSets
    SET LHAPDF_DATA_PATH /cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/LHAPDFSets/v62r3p1/data
    SET PYTHIA8XML ${PYTHIA8_XML}
    SET PYTHIA8DATA ${PYTHIA8_XML}
    SET GSINO_USE_TORCH ${GSINO_USE_TORCH}
    SET GSINO_USE_ONNXRUNTIME ${GSINO_USE_ONNXRUNTIME}
)

gaussino_add_subdirectories(
    Defaults
    EDMdev/MCTruthToEDM
    GaussinoML/GaussinoMLBase
    ${ONNXRuntimeDirs}
    ${TorchDirs}
    Gen/GenBeam
    Gen/Generators
    Gen/GenInterfaces
    Gen/LbPGuns
    Gen/LbPythia8
    Gen/RndInit
    HepMCUser
    Moni/GaussMonitor
    NewRnd
    Sim/EDM
    Sim/ExternalDetector
    ${CustomSimulationDirs}
    Sim/Gaussino
    Sim/GiGaMT
    Sim/GiGaMTCore
    ${DD4hepDirs}
    Sim/GiGaMTDebug
    Sim/GiGaMTFactories
    Sim/GiGaMTGeo
    Sim/GiGaMTPhoenix
    Sim/GiGaMTReDecay
    Sim/GiGaMTTruth
    Sim/GiGaMTVis
    Sim/MCCollector
    Sim/ParallelGeometry
    Sim/SimInterfaces
    Utils
)

gaudi_install(CMAKE
    # helpers to find external projects
    cmake/FindPythia8.cmake
    cmake/Findtorch.cmake
    cmake/Findonnxruntime.cmake
    cmake/GaussinoConfigUtils.cmake
    cmake/GaussinoConfig.cmake.in
)

gaussino_finalize_configuration()

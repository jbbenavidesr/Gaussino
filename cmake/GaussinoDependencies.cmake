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
include(${CMAKE_CURRENT_LIST_DIR}/GaussinoConfigUtils.cmake)

# TODO: [LHCb DEPENDENCY]
if(NOT COMMAND lhcb_find_package)
  # Look for LHCb find_package wrapper
  find_file(LHCbFindPackage_FILE LHCbFindPackage.cmake)
  if(LHCbFindPackage_FILE)
      include(${LHCbFindPackage_FILE})
  else()
      # if not found, use the standard find_package
      macro(lhcb_find_package)
          find_package(${ARGV})
      endmacro()
  endif()
endif()

# -- Public dependencies
lhcb_find_package(GaussinoExtLibs REQUIRED) # TODO: [NEW CMAKE] temporary project

# custom simulations work only with G4 > 10.7
# + make sure it is set once and only in Gaussino
if(NOT Gaussino_FOUND AND Geant4_VERSION VERSION_LESS "10.7")
  set(CUSTOMSIM OFF)
endif()

find_package(HepMC3 REQUIRED)
find_package(Pythia8 REQUIRED)

if(GSINO_USE_TORCH)
  find_package(BLAS)
  find_package(torch REQUIRED 1.13 torch c10 torch_cpu torch_python)
  add_compile_definitions(GSINO_USE_TORCH)
endif()

if(GSINO_USE_ONNXRUNTIME)
    find_package(onnxruntime REQUIRED 1.15 onnxruntime)
    add_compile_definitions(GSINO_USE_ONNXRUNTIME)
endif()

if(USE_DD4HEP)
    set(CMAKE_CXX_STANDARD ${GAUDI_CXX_STANDARD})
    find_package(DD4hep REQUIRED DDCore)
    find_package(DD4hepDDG4 REQUIRED)
endif()

if(CUSTOMSIM)
    add_compile_definitions(CUSTOMSIM)
endif()

# -- Private dependencies
if(WITH_Gaussino_PRIVATE_DEPENDENCIES)
    if(BUILD_TESTING)
        find_package(Boost REQUIRED unit_test_framework)
    endif()
endif()

if(WITH_ADEPT)
    find_package(AdePT REQUIRED)
endif()

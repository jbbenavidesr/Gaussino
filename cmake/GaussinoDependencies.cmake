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

if(Geant4_FOUND)
    get_cmake_property(variables VARIABLES)
    foreach(var ${variables})
	string(REGEX MATCH "Geant4_DATASET_([A-Za-z0-9]+)_ENVVAR" _ ${var})
	if(NOT CMAKE_MATCH_1 STREQUAL "")
	    # copy the Geant4Data variables using GaudiToolbox.cmake
	    lhcb_env(SET ${Geant4_DATASET_${CMAKE_MATCH_1}_ENVVAR} ${Geant4_DATASET_${CMAKE_MATCH_1}_PATH})
	endif()
    endforeach()
endif()

find_package(HepMC3 REQUIRED)
find_package(Pythia8 REQUIRED)

if(USE_DD4HEP)
    set(CMAKE_CXX_STANDARD ${GAUDI_CXX_STANDARD})
    find_package(DD4hep REQUIRED DDCore)
    find_package(DD4hepDDG4 
	REQUIRED 
	PATHS ${GAUSSINOEXTLIBS_PREFIX_DIR}/cmake
       	NO_DEFAULT_PATH
    )
endif()

# -- Private dependencies
if(WITH_Gaussino_PRIVATE_DEPENDENCIES)
    if(BUILD_TESTING)
        find_package(Boost REQUIRED unit_test_framework)
    endif()
endif()

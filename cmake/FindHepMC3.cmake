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
# - Locate HepMC3 library
# Defines:
#  Variables:
#   HepMC3_FOUND
#   HepMC3_INCLUDE_DIR
#   HepMC3_INCLUDE_DIRS (not cached)
#   HepMC3_<component>_LIBRARY
#   HepMC3_LIBRARIES (not cached)
#   HepMC3_LIBRARY_DIRS (not cached)
#  Targets:
#   HepMC3::HepMC3 (default)
#   HepMC3::HepMC3search

if(NOT HepMC3_FIND_COMPONENTS)
  set(HepMC3_FIND_COMPONENTS "HepMC3" "HepMC3search")
endif()

set(HepMC3_INCLUDE_DIR ${GAUSSINOEXTLIBS_PREFIX_DIR}/include)
mark_as_advanced(HepMC3_INCLUDE_DIR)

foreach(component IN LISTS HepMC3_FIND_COMPONENTS)
  find_library(HepMC3_${component}_LIBRARY
	       ${component}
           PATHS ${GAUSSINOEXTLIBS_PREFIX_DIR}/lib)
  mark_as_advanced(HepMC3_${component}_LIBRARY)
  if(HepMC3_${component}_LIBRARY)
    set(HepMC3_${component}_FOUND ${HepMC3_${component}_LIBRARY})
    list(APPEND HepMC3_LIBRARIES ${HepMC3_${component}_LIBRARY})
    get_filename_component(_comp_dir ${HepMC3_${component}_LIBRARY} DIRECTORY)
    list(APPEND HepMC3_LIBRARY_DIRS ${_comp_dir})
  endif()
endforeach()

# handle the QUIETLY and REQUIRED arguments and set HepMC_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HepMC3
  HANDLE_COMPONENTS
  VERSION_VAR 0.1
  REQUIRED_VARS HepMC3_INCLUDE_DIR HepMC3_LIBRARIES
)

foreach(component IN LISTS HepMC3_FIND_COMPONENTS)
    if(NOT TARGET HepMC3::${component})
    	add_library(HepMC3::${component} UNKNOWN IMPORTED)
    	set_target_properties(HepMC3::${component} PROPERTIES IMPORTED_LOCATION ${HepMC3_${component}_LIBRARY})
    	target_include_directories(HepMC3::${component} 
	    SYSTEM INTERFACE "${GAUSSINOEXTLIBS_PREFIX_DIR}/include"
	    "${GAUSSINOEXTLIBS_PREFIX_DIR}/share/HepMC3/interfaces")
    endif()
endforeach()

###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

# list(PREPEND CMAKE_PREFIX_PATH ${TORCH_ROOT_DIR})

set(torch_PACKAGE_PATH
  lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/torch
)
find_path(torch_ROOT_DIR ${torch_PACKAGE_PATH})

list(APPEND torch_INCLUDE_DIRS
 ${torch_ROOT_DIR}/${torch_PACKAGE_PATH}/include
 ${torch_ROOT_DIR}/${torch_PACKAGE_PATH}/include/torch/csrc/api/include
)

set(torch_LIBRARIES_DIR ${torch_ROOT_DIR}/${torch_PACKAGE_PATH}/lib)

# Enforce a minimal list if none is explicitly requested
if(NOT torch_FIND_COMPONENTS)
  set(torch_FIND_COMPONENTS torch)
endif()

foreach(component ${torch_FIND_COMPONENTS})
  find_library(torch_${component}_LIBRARY NAMES ${component}
               HINTS ${torch_LIBRARIES_DIR})
  if (torch_${component}_LIBRARY)
    set(torch_${component}_FOUND 1)
    list(APPEND torch_LIBRARIES ${torch_${component}_LIBRARY})
  else()
    set(torch_${component}_FOUND 0)
  endif()
  mark_as_advanced(torch_${component}_LIBRARY)
endforeach()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(torch DEFAULT_MSG torch_LIBRARIES)

if(torch_FOUND)
  foreach(component ${torch_FIND_COMPONENTS})
    if(NOT TARGET torch::${component})
      add_library(torch::${component} UNKNOWN IMPORTED)
      set_target_properties(torch::${component} PROPERTIES IMPORTED_LOCATION ${torch_${component}_LIBRARY})
      target_include_directories(torch::${component} SYSTEM INTERFACE "${torch_INCLUDE_DIRS}")
    endif()
  endforeach()
endif()

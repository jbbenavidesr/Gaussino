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

find_path(onnxruntime_ROOT_DIR include/core/session/onnxruntime_c_api.h)

set(onnxruntime_INCLUDE_DIRS ${onnxruntime_ROOT_DIR}/include)
set(onnxruntime_LIBRARY_DIRS ${onnxruntime_ROOT_DIR}/lib)
find_library(onnxruntime_LIBRARIES NAMES onnxruntime PATHS ${onnxruntime_LIBRARY_DIRS})

if(NOT onnxruntime_FIND_COMPONENTS)
  set(onnxruntime_FIND_COMPONENTS onnxruntime)
endif()

foreach(component ${onnxruntime_FIND_COMPONENTS})
  find_library(onnxruntime_${component}_LIBRARY NAMES ${component}
               HINTS ${onnxruntime_LIBRARIES_DIR})
  if (onnxruntime_${component}_LIBRARY)
    set(onnxruntime_${component}_FOUND 1)
    list(APPEND onnxruntime_LIBRARIES ${onnxruntime_${component}_LIBRARY})
  else()
    set(onnxruntime_${component}_FOUND 0)
  endif()
  mark_as_advanced(onnxruntime_${component}_LIBRARY)
endforeach()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(onnxruntime DEFAULT_MSG onnxruntime_LIBRARIES)

if(onnxruntime_FOUND)
  foreach(component ${onnxruntime_FIND_COMPONENTS})
    if(NOT TARGET onnxruntime::${component})
      add_library(onnxruntime::${component} UNKNOWN IMPORTED)
      set_target_properties(onnxruntime::${component} PROPERTIES IMPORTED_LOCATION ${onnxruntime_${component}_LIBRARY})
      target_include_directories(onnxruntime::${component} SYSTEM INTERFACE "${onnxruntime_INCLUDE_DIRS}")
    endif()
  endforeach()
endif()

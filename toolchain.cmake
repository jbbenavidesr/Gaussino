###############################################################################
# (c) Copyright 2018 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
cmake_minimum_required(VERSION 3.6)

set(LCG_LAYER LHCB_5 CACHE STRING "Specific set of version to use")
option(LCG_USE_GENERATORS "enable/disable use of LCG generators" ON)


# this check is needed because the toolchain is called when checking the
# compiler (without the proper cache)
if(NOT CMAKE_SOURCE_DIR MATCHES "CMakeTmp")
  # Avoid using Gaudi's get_host_binary_tag.py script as it doesn't support Python 3.8
  # https://gitlab.cern.ch/gaudi/Gaudi/-/issues/123
  execute_process(COMMAND "lb-host-binary-tag"
                  OUTPUT_VARIABLE HOST_BINARY_TAG
                  RESULT_VARIABLE HOST_BINARY_RETURN
                  ERROR_VARIABLE  HOST_BINARY_ERROR
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(HOST_BINARY_TAG ${HOST_BINARY_TAG} CACHE STRING "BINARY_TAG of the host")
  if(HOST_BINARY_RETURN OR NOT HOST_BINARY_TAG)
    message(FATAL_ERROR "Error getting host binary tag\nFailed to execute ${HOST_BINARY_TAG_COMMAND}\n"
                        "HOST_BINARY_TAG value: ${HOST_BINARY_TAG}\n"
                        "Program Return Value: ${HOST_BINARY_RETURN}\n"
                        "Error Message: ${HOST_BINARY_ERROR}\n")
  endif()

  find_file(default_toolchain NAMES GaudiDefaultToolchain.cmake
            HINTS ${CMAKE_SOURCE_DIR}/cmake
                  ${CMAKE_CURRENT_LIST_DIR}/cmake)
  if(default_toolchain)
    include(${default_toolchain})
    if(NOT DEFINED CMAKE_USE_CCACHE)
      set(CMAKE_USE_CCACHE "YES" CACHE BOOL "...")
    endif()
  else()
    message(FATAL_ERROR "Cannot find GaudiDefaultToolchain.cmake")
  endif()

  # FIXME: make sure we do not pick up unwanted/problematic projects from LCG
  if(CMAKE_PREFIX_PATH)
    # - ninja (it requires LD_LIBRARY_PATH set to run)
    # - Gaudi (we do not want to use it from LCG)
    # - Geant4 (we do not want to use it from LCG)
    # - xenv (conflicts with the version in the build environment)
    list(FILTER CMAKE_PREFIX_PATH EXCLUDE REGEX "(LCG_|lcg/nightlies).*(ninja|Gaudi|Geant4|xenv)")
  endif()
endif()

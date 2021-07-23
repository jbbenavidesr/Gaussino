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
# Special toolchain file that inherits the same heptools version as the
# used projects.
#
#
# this check is needed because the toolchain is called when checking the
# compiler (without the proper cache)
if(NOT CMAKE_SOURCE_DIR MATCHES "CMakeTmp")
  find_path(heptools_mod_dir UseHEPTools.cmake)
  message(INFO heptools_mod_dir = ${heptools_mod_dir})
  set(CMAKE_MODULE_PATH ${heptools_mod_dir} ${CMAKE_MODULE_PATH})
  if(heptools_version)
    include(UseHEPTools)
    use_heptools(${heptools_version})
  else()
    include(InheritHEPTools)
    inherit_heptools()
  endif()

  # FIXME: make sure we do not pick up unwanted/problematic projects from LCG
  if(CMAKE_PREFIX_PATH)
    # - ninja (it requires LD_LIBRARY_PATH set to run)
    # - Gaudi (we do not want to use it from LCG)
    # - xenv (conflicts with the version in the build environment)
    list(FILTER CMAKE_PREFIX_PATH EXCLUDE REGEX "(LCG_|lcg/nightlies).*(ninja|Gaudi|xenv)")
  endif()

  # FIXME: make sure we do not pick up geant from LCG (it requires LD_LIBRARY_PATH set)
  if(CMAKE_PREFIX_PATH AND CMAKE_VERSION VERSION_GREATER "3.6.0")
    list(FILTER CMAKE_PREFIX_PATH EXCLUDE REGEX "(LCG_|lcg/nightlies).*Geant4")
  endif()
endif()

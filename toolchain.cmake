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

if(NOT DEFINED GAUDI_OLD_STYLE_PROJECT)
  file(STRINGS "${PROJECT_SOURCE_DIR}/CMakeLists.txt" _top_cmakelists)
  set(GAUDI_OLD_STYLE_PROJECT NO)
  foreach(_line IN LISTS _top_cmakelists)
    if(_line MATCHES "^[^#]*gaudi_project")
      set(GAUDI_OLD_STYLE_PROJECT YES)
      break()
    endif()
  endforeach()
  set(GAUDI_OLD_STYLE_PROJECT ${GAUDI_OLD_STYLE_PROJECT} CACHE BOOL "true if the top level CMakeLists file contains a call to gaudi_project")
endif()

if(NOT GAUDI_OLD_STYLE_PROJECT AND "$ENV{GAUDI_OLD_STYLE_PROJECT}" STREQUAL "")
  # for new style CMake projects, or vanilla CMake projects
  if("$ENV{BINARY_TAG}" STREQUAL "" OR "$ENV{LCG_VERSION}" STREQUAL "")
    message(FATAL_ERROR "The environment variables BINARY_TAG and LCG_VERSION mut be set for new style CMake projects")
  endif()
  find_file(LCG_TOOLCHAIN NAMES $ENV{BINARY_TAG}.cmake PATH_SUFFIXES lcg-toolchains/LCG_$ENV{LCG_VERSION})
  if(LCG_TOOLCHAIN)
    include(${LCG_TOOLCHAIN})
    # after including the toolchain, set some LHCb defaults
    find_program(CCACHE_COMMAND NAMES ccache)
    if(CCACHE_COMMAND)
      set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_COMMAND}" CACHE PATH "...")
      set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_COMMAND}" CACHE PATH "...")
    endif()
    if(NOT DEFINED GAUDI_USE_INTELAMPLIFIER)
      set(GAUDI_USE_INTELAMPLIFIER "YES" CACHE BOOL "...")
    endif()
    if(NOT DEFINED GAUDI_LEGACY_CMAKE_SUPPORT)
      set(GAUDI_LEGACY_CMAKE_SUPPORT "YES" CACHE BOOL "...")
    endif()
    if(NOT DEFINED CMAKE_INSTALL_PREFIX OR CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
      set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/InstallArea/$ENV{BINARY_TAG}" CACHE BOOL "...")
    endif()
  else()
    message(FATAL_ERROR "Cannot find LCG $ENV{LCG_VERSION} $ENV{BINARY_TAG} toolchain")
  endif()
else()

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
    # - xenv (conflicts with the version in the build environment)
    list(FILTER CMAKE_PREFIX_PATH EXCLUDE REGEX "(LCG_|lcg/nightlies).*(ninja|Gaudi|xenv)")
  endif()

  # Make sure that when the toolchain is invoked again it uses this branch
  set(ENV{GAUDI_OLD_STYLE_PROJECT} "${CMAKE_SOURCE_DIR}")
endif()
endif()




# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(inherit_heptools_module InheritHEPTools.cmake)

# Process the lines of LCG_generators_*.txt file to extract the
# needed generators (variable generators_versions)
macro(lcg_set_generator root name hash version dir)
  #message(STATUS "Processing ${name} ${version} (${dir})")
  list(FIND generators_versions ${name} _gen_idx)
  list(FIND found_generators ${name} _found_idx)
  if(NOT _gen_idx EQUAL -1 AND _found_idx EQUAL -1)
    math(EXPR _gen_vers_idx "${_gen_idx} + 1")
    list(GET generators_versions ${_gen_vers_idx} _gen_vers)
    #message(STATUS "  required ${_gen_vers}")
    if("${version}" STREQUAL "${_gen_vers}")
      set(${name}_native_version ${${name}_config_version})
      if(NOT IS_ABSOLUTE ${dir})
        set(${name}_home ${root}/${dir})
      else()
        set(${name}_home ${dir})
      endif()
      if(EXISTS ${${name}_home})
        get_filename_component(${name}_home "${${name}_home}" ABSOLUTE)
        set(${name}_config_version ${version} CACHE STRING "Version of ${name}")
        mark_as_advanced(${name}_config_version)
        message(STATUS "Using ${name} ${version} from ${${name}_home}")
        list(INSERT CMAKE_PREFIX_PATH 0 ${${name}_home})
        list(APPEND found_generators ${name})
      endif()
    #else()
    #  message(STATUS "  wrong version (${name} ${version})")
    endif()
  #else()
  #  if(_found_idx EQUAL -1)
  #    message(STATUS "  ${name} not required")
  #  else()
  #    message(STATUS "  ${name} already found")
  #  endif()
  endif()
endmacro()

# Check that all the generators in generators_versions are found in the
# LCG_generators_*.txt file.
macro(check_generators)
  set(_idx 0)
  set(_missing_generators)
  list(LENGTH generators_versions _length)
  while(_idx LESS _length)
    list(GET generators_versions ${_idx} _gen_name)
    list(FIND found_generators ${_gen_name} _found_idx)
    if(_found_idx EQUAL -1)
      math(EXPR _gen_vers_idx "${_idx} + 1")
      list(GET generators_versions ${_gen_vers_idx} _gen_ver)
      set(_missing_generators "${_missing_generators} ${_gen_name}(${_gen_ver})")
    endif()
    math(EXPR _idx "${_idx} + 2")
  endwhile()
  if(_missing_generators)
    message(WARNING "Missing generators: ${_missing_generators}")
  endif()
endmacro()

# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(inherit_heptools_module)
  include(${inherit_heptools_module})
  inherit_heptools()
  set(BINARY_TAGCOMP ${BINARY_TAG})
  if(BINARY_TAG STREQUAL "x86_64-centos7-gcc7-do0")
    set(BINARY_TAGCOMP "x86_64-centos7-gcc7-dbg")
  endif()

  message(STATUS "BINARY_TAGCOMP=${BINARY_TAGCOMP}")
  message(STATUS "BINARY_TAG=${BINARY_TAG}")

  if(EXISTS ${CMAKE_SOURCE_DIR}/generators_versions_LCG${heptools_version}.txt)
    file(READ ${CMAKE_SOURCE_DIR}/generators_versions_LCG${heptools_version}.txt generators_versions)
    string(REGEX REPLACE "[ \t\n]+" ";" generators_versions "${generators_versions}")
    set(generators_versions ${generators_versions})
  endif()

  if(LCG_TOOLCHAIN_INFO)
    # prepare list of toolchain info files for generators
    set(_info_name "LCG_generators_${BINARY_TAGCOMP}.txt")
    # look for alternative info files
    file(TO_CMAKE_PATH "$ENV{CMAKE_PREFIX_PATH}" _a)
    file(TO_CMAKE_PATH "$ENV{CMTPROJECTPATH}" _b)
    set(_search_path ${LCG_releases} ${_a} ${_b} ${CMAKE_PREFIX_PATH})
    foreach(_d ${_search_path})
      foreach(_suff "LCG_${heptools_version}" ".")
        foreach(_f "${_d}/${_suff}/${_info_name}.installed"
                   "${_d}/${_suff}/${_info_name}")
          if(EXISTS "${_f}")
            get_filename_component(_f "${_f}" ABSOLUTE)
            list(APPEND LCG_GENERATOR_INFOS "${_f}")
            break()
          endif()
        endforeach()
      endforeach()
    endforeach()
    if(NOT LCG_GENERATOR_INFOS)
      message(FATAL_ERROR "Generators infos not found for ${heptools_version}")
    endif()
    message(STATUS "Using generators infos from:")
    list(REMOVE_DUPLICATES LCG_GENERATOR_INFOS)
    foreach(_i ${LCG_GENERATOR_INFOS})
      message(STATUS "  * ${_i}")
    endforeach()

    set(found_generators)
    foreach(_i ${LCG_GENERATOR_INFOS})
      get_filename_component(_info_dir "${_i}" PATH)
      file(STRINGS "${_i}" _lcg_infos)
      foreach(_l ${_lcg_infos})
        if(NOT _l MATCHES "^(PLATFORM|VERSION|COMPILER):")
          string(REGEX REPLACE "; *" ";" _l "${_l}")
          lcg_set_generator(${_info_dir} ${_l})
        endif()
      endforeach()
    endforeach()
    check_generators()
  else()
    # This check is to handle the special case of test builds internal to CMake
    if(NOT CMAKE_SOURCE_DIR MATCHES "CMakeTmp")
      message(FATAL_ERROR "Only LCG >= 68 is supported")
    endif()
  endif()
endif()

if(DEFINED ENV{LBLOCALSOFT})
  message(STATUS "Manually prepending $ENV{LBLOCALSOFT} to CMAKE_PREFIX_PATH")
  set(CMAKE_PREFIX_PATH "$ENV{LBLOCALSOFT};${CMAKE_PREFIX_PATH}")
endif()

# FIXME: make sure we do not pick up geant from LCG (it requires LD_LIBRARY_PATH set)
if(CMAKE_PREFIX_PATH AND CMAKE_VERSION VERSION_GREATER "3.6.0")
  list(FILTER CMAKE_PREFIX_PATH EXCLUDE REGEX "(LCG_|lcg/nightlies).*Geant4")
endif()

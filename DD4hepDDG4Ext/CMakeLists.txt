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
gaudi_subdir(DD4hepDDG4Ext v1r0)

# Define EvtGen external project
set(DD4hep_GIT_SOURCE "https://gitlab.cern.ch/Gaussino/DD4hep.git" CACHE STRING "URL of the DD4hep git repository")
set(DD4hep_GIT_TAG "v01-16-01-patches" CACHE STRING "Tag of DD4hep to be used")

find_package(ROOT)
find_package(Boost)
find_package(DD4hep REQUIRED)
find_package(LCIO REQUIRED)

include(ExternalProject)
message(STATUS "DDG4 from DD4hep will use tag \"${DD4hep_GIT_TAG}\" from repository ${DD4hep_GIT_SOURCE}")
string(REPLACE "c++" "" DD4HepDDG4_CXX_STANDARD "${GAUDI_CXX_STANDARD}")
message(STATUS "Setting CMAKE_CXX_STANDARD to \"${DD4HepDDG4_CXX_STANDARD}\"")
    
ExternalProject_Add(DD4hepDDG4Ext
  GIT_REPOSITORY ${DD4hep_GIT_SOURCE}
  GIT_TAG ${DD4hep_GIT_TAG}
  CMAKE_ARGS 
    -Dheptools_version=${heptools_version}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/dd4hepddg4_toolchain.cmake
    -DDD4HEP_USE_EXISTING_DD4HEP=${DD4hep_DIR}
    -DDD4HEP_BUILD_PACKAGES=DDG4
    -DDD4HEP_USE_GEANT4=ON
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_CXX_STANDARD=${DD4HepDDG4_CXX_STANDARD}
    -DDD4HEP_SELECTED_NAME=DD4hepDDG4
    "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -Wno-suggest-override"  # less strict warnings
  # This works around a CMake issue with the Ninja generator where it
  # does not understand imported libraries, and instead needs `BUILD_BYPRODUCTS`
  # explicitly set.
  # https://cmake.org/pipermail/cmake/2015-April/060234.html
  BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libDDG4.so
  CMAKE_COMMAND ${env_cmd} --xml ${env_xml} cmake
  BUILD_COMMAND ${env_cmd} --xml ${env_xml} cmake --build .
)

ExternalProject_Get_property(DD4hepDDG4Ext INSTALL_DIR)

# Inspired from http://mesos.apache.org/documentation/latest/cmake-examples/#imported-libraries
add_library(DD4hep::DDG4 SHARED IMPORTED GLOBAL)
set_target_properties(DD4hep::DDG4 PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/lib/libDDG4.so)
target_include_directories(DD4hep::DDG4 SYSTEM INTERFACE ${INSTALL_DIR}/include)
# This works around a CMake issue with setting include directories of
# imported libraries built with `ExternalProject_Add`.cmake/DD4hepBuild.cmake
# https://gitlab.kitware.com/cmake/cmake/issues/15052
file(MAKE_DIRECTORY ${INSTALL_DIR}/include)

# The external project's install rules are not part of the main project's
# install rules, so if anything from the external project should be
# installed as part of the main build, these need to be specified in the main
# build as additional install() commands.
# https://cmake.org/cmake/help/v3.20/module/ExternalProject.html
#
# Use DD4hep's installation location (see cmake/DD4hepBuild.cmake)
# > SET(DD4HEP_PYTHON_INSTALL_DIR lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages)
install(DIRECTORY "${INSTALL_DIR}/include/"
        DESTINATION include)
install(DIRECTORY "${INSTALL_DIR}/lib/"
        DESTINATION lib
        PATTERN "python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}" EXCLUDE)
install(DIRECTORY "${INSTALL_DIR}/lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/"
        DESTINATION python)

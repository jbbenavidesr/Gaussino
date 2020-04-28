gaudi_subdir(DD4hepDDG4 v1r0)

find_package(ROOT)

# Define EvtGen external project
set(DD4hep_GIT_SOURCE "ssh://git@gitlab.cern.ch:7999/Gaussino/DD4hep.git" CACHE STRING "URL of the DD4hep git repository")
set(DD4hep_GIT_TAG "patched_cmake" CACHE STRING "Tag of DD4hep to be used")

find_package(ROOT)
find_package(Boost)
find_package(DD4hep REQUIRED)
find_package(LCIO REQUIRED)
find_package(DD4hepDDG4 QUIET)
if(NOT DD4hepDDG4_FOUND)
  message(STATUS "DD4hepDDG4 not found, I'm building it")
  include(ExternalProject)
  message(STATUS "")
  message(STATUS "DDG4 from DD4hep will use tag \"${DD4hep_GIT_TAG}\" from repository ${DD4hep_GIT_SOURCE}")
  string(REPLACE "c++" "" DD4HepDDG4_CXX_STANDARD "${GAUDI_CXX_STANDARD}")
  string(REPLACE ";" " " DUMMY_COMMAND "${CMAKE_COMMAND}")
  message(STATUS "Setting CMAKE_CXX_STANDARD to \"${DD4HepDDG4_CXX_STANDARD}\"")
      
  ExternalProject_Add(DD4hepDDG4
    GIT_REPOSITORY ${DD4hep_GIT_SOURCE}
    GIT_TAG ${DD4hep_GIT_TAG}
    CMAKE_ARGS 
      -Dheptools_version=${heptools_version}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/dd4hepddg4_toolchain.cmake
      -DDD4HEP_USE_EXISTING_DD4HEP=${DD4hep_DIR}
      -DDD4HEP_BUILD_PACKAGES=DDG4
      -DDD4HEP_USE_GEANT4=ON
      -DLCG_USE_NATIVE_COMPILER=${LCG_USE_NATIVE_COMPILER}
      -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      -DCMAKE_CXX_STANDARD=${DD4HepDDG4_CXX_STANDARD}
      #-DCMAKE_COMMAND:INTERNAL=${DUMMY_COMMAND}
      #-DCMAKE_COMMAND:INTERNAL=${env_cmd} --xml ${env_xml} cmake
      -DCMAKE_INSTALL_LIBDIR=lib
    CMAKE_COMMAND ${env_cmd} --xml ${env_xml} cmake
    BUILD_COMMAND ${env_cmd} --xml ${env_xml} cmake --build .
  )
                                                                                                 
set(DD4hepDDG4_INCLUDE_DIRS  "${CMAKE_CURRENT_BINARY_DIR}/DD4hepDDG4-prefix/src/DD4hepDDG4" "${CMAKE_CURRENT_BINARY_DIR}/DD4hepDDG4-prefix/src/DD4hepDDG4/external" CACHE PATH "")
set(DD4hepDDG4_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/DD4hepDDG4-prefix/src/DD4hepDDG4-build/libDelphes.so" CACHE STRING "")
  set(DD4hepDDG4_FOUND YES CACHE BOOL "")
endif()

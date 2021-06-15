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
    -DLCG_USE_NATIVE_COMPILER=${LCG_USE_NATIVE_COMPILER}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_CXX_STANDARD=${DD4HepDDG4_CXX_STANDARD}
    -DDD4HEP_SELECTED_NAME=DD4hepDDG4
    -DCMAKE_INSTALL_LIBDIR=lib
  CMAKE_COMMAND ${env_cmd} --xml ${env_xml} cmake
  BUILD_COMMAND ${env_cmd} --xml ${env_xml} cmake --build .
)
                                                                                                
#set(DD4hepDDG4_INCLUDE_DIRS  "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "" FORCE)
#set(DD4hepDDG4_DIR "${CMAKE_INSTALL_PREFIX}/cmake" CACHE PATH "" FORCE)
#set(DD4hepDDG4_LIBRARIES "${CMAKE_INSTALL_PREFIX}/lib/libDDG4LCIO.so" "${CMAKE_INSTALL_PREFIX}/lib/libDDG4Legacy.so" "${CMAKE_INSTALL_PREFIX}/lib/libDDG4Plugins.so" "${CMAKE_INSTALL_PREFIX}/lib/libDDG4Python.so" "${CMAKE_INSTALL_PREFIX}/lib/libDDG4.so" CACHE STRING "" FORCE)
#set(DD4hepDDG4_FOUND YES CACHE BOOL "" FORCE)

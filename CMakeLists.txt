CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

# Reduce the total number of warnings by A LOT until ROOT has been modified
option(GAUDI_SUGGEST_OVERRIDE "enable warnings for missing override keyword" OFF)
# Create the compile commands json for YCM superpowers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GEANT4_BUILD_MULTITHREADED ON)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Set the CMP0074 policy that dictates how the packages are looked up
# this should allow overriding the LCG versions of package XXX,
# provided, XXX_ROOT is set
if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
  message(STATUS "Setting the policy CMP0074 to NEW")
else()
  message(STATUS "Policy CMP0074 not available: LCG packages NOT overriden by _ROOT environment variable")
endif()

# hide warnings from some external projects
find_package(Boost)
find_package(ROOT)
find_package(CLHEP)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                           ${CLHEP_INCLUDE_DIRS})

# Add -O0 to remove optimizations when using gcc
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0" CACHE STRING "bla" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0" CACHE STRING "bla" FORCE)

macro(FindG4libs)
    # FIXME: this should be an imported target
    foreach(name ${ARGN})
        find_library(GEANT4_${name}_LIBRARY
                     G4${name}
                     HINTS ${Geant4_DIR}/lib
                     PATHS ${Geant4_LIBRARY_DIRS})
        list(APPEND GEANT4_LIBS ${GEANT4_${name}_LIBRARY})
    endforeach()
    link_directories(${Geant4_LIBRARY_DIRS})
endmacro()

macro(AddHepMC3)
  # FIXME: This is just an work-around to include the locally compiled HepMC3 as the HepMC3Config.cmake
  # is not yet available during configuration. 
  include_directories(SYSTEM ${CMAKE_INSTALL_PREFIX}/include ${CMAKE_INSTALL_PREFIX}/share/HepMC3/interfaces)
  link_directories(${CMAKE_INSTALL_PREFIX}/lib64)
  link_directories(${CMAKE_INSTALL_PREFIX}/lib)
  #add_dependencies(HepMC3Ext)

  set(HEPMC3_LIBRARIES
    -lHepMC3
    # FIXME: disabled because of HepMC & HepMC3 namespace collision
    # -lHepMC3rootIO
    -lHepMC3search)
endmacro()

macro(AddDD4hepDDG4)
    include_directories(SYSTEM ${CMAKE_INSTALL_PREFIX}/include)
    link_directories(${CMAKE_INSTALL_PREFIX}/lib)

    set(DD4hepDDG4_LIBRARIES
          #-lDDG4LCIO
          -lDDG4Legacy  
          -lDDG4Plugins  
          -lDDG4Python  
          -lDDG4  
          -lDDPython)
endmacro()

# make sure pkg-config looks for packages on CMAKE_PREFIX_PATH and system
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
list(APPEND CMAKE_PREFIX_PATH /usr)

# Declare project name and version
gaudi_project(Gaussino v1r0
              FORTRAN
              USE Run2Support v0r0
                  Geant4 v104r1
              DATA Gen/PGunsData VERSION v1r*
                   Gen/DecFiles VERSION v30r*
                   LHAPDFSets VERSION v2r*
                   Vis/XmlVis VERSION v2r*)

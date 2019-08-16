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
    -lHepMC3search)
    #-lHepMC3rootIO)
endmacro()

# Declare project name and version
gaudi_project(Gaussino v1r0
              FORTRAN
              USE LHCb v1r0
                  Geant4 v104r1
              DATA Gen/PGunsData VERSION v1r*
                   Gen/DecFiles VERSION v30r*
                   LHAPDFSets VERSION v2r*
                   Vis/XmlVis VERSION v2r*)

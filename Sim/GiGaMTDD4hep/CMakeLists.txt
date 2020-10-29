###############################################################################
# (c) Copyright 2018 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
################################################################################
# Package: GiGaMTDD4hep
# Interfaces to load the detector from the DD4hep geometry service
################################################################################
gaudi_subdir(GiGaMTDD4hep v1r0)


gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTFactories
                         Sim/GiGaMTCore
                         Sim/SimInterfaces
                         Sim/GiGaMTGeo
                         DD4hepDDG4Ext
                         Det/LbDD4hep)

find_package(ROOT COMPONENTS Geom Core GenVector Matrix Physics)

find_package(Boost COMPONENTS system thread filesystem)

include_directories(SYSTEM  ${ROOT_INCLUDE_DIRS})
AddHepMC3()
AddDD4hepDDG4()

message(STATUS "ROOTSYS -> ${ROOTSYS}")
set(ENV{ROOTSYS} ${ROOTSYS})

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()

find_package(DD4hep REQUIRED CONFIG COMPONENTS DDCore)

message(STATUS "DD4hepDDG4_INCLUDE_DIRS: ${DD4hepDDG4_INCLUDE_DIRS}")
message(STATUS "DD4hepDDG4_LIBRARIES: ${DD4hepDDG4_LIBRARIES}")

include_directories(SYSTEM  ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${DD4hep_INCLUDE_DIRS}  ${DD4hepDDG4_INCLUDE_DIRS})

gaudi_add_library(GiGaMTDD4hepLib
                  src/lib/*.cpp 
                  PUBLIC_HEADERS GiGaMTDD4hep
                  INCLUDE_DIRS LbDD4hep DD4hep
                  LINK_LIBRARIES DD4hep ${DD4hep_COMPONENT_LIBRARIES} ROOT GiGaMTCoreRunLib ${DD4hepDDG4_LIBRARIES}) 

add_dependencies(GiGaMTDD4hepLib DD4hepDDG4Ext) 

gaudi_add_module(GiGaMTDD4hep
                 src/components/*.cpp 
                 LINK_LIBRARIES GiGaMTDD4hepLib GaudiAlgLib)

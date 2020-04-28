################################################################################
# Package: GiGaMTReDecay
#
# This package contains all the optional ReDecay configuration to coordinate 
# the re-use of parts over the event between multiple events.
# 
#################################################################################
gaudi_subdir(GiGaMTReDecay v1r0)

gaudi_depends_on_subdirs(NewRnd
                         Gen/GenBeam
                         Gen/Generators
                         EDMdev/MCTruthToEDM
                         Gen/RndInit
                         Sim/GiGaMTTruth
                         Sim/GiGaMT)

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()

AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_library(GiGaMTReDecayLib
                  src/lib/token/*.cpp
                  PUBLIC_HEADERS GiGaMTReDecay
                  INCLUDE_DIRS GenBeam RndInit GiGaMTTruth
                  LINK_LIBRARIES GiGaMTLib NewRnd GeneratorsLib)

gaudi_add_module(GiGaMTReDecay
        src/components/service/*.cpp
        src/components/gen_tools/*.cpp
        src/components/*.cpp
        src/components/sorters/*.cpp
        src/components/sim/*.cpp
        src/components/edm/*.cpp
        LINK_LIBRARIES GiGaMTReDecayLib ROOT RndInitLib MCTruthToEDMLib)

add_dependencies(GiGaMTReDecayLib HepMC3Ext)

#gaudi_add_module(GiGaMT
        #src/components/*.cpp
        #LINK_LIBRARIES GiGaMTLib ROOT)

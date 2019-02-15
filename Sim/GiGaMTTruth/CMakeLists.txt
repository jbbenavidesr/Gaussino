################################################################################
# Package: GiGaMTTruth 
#
# This package contains all code related to the tracking and extraction of truth
# information from the Geant4 propagation
#################################################################################
gaudi_subdir(GiGaMTTruth v1r0)
gaudi_depends_on_subdirs(NewRnd
                         GaudiAlg
                         HepMCUser
                         Sim/GiGaMTCore)

find_package(CLHEP COMPONENTS Random Vector)
find_package(XercesC)
find_package(HepMC CONFIG)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

gaudi_add_library(GiGaMTInfosLib
                 src/infos/*.cpp
                 PUBLIC_HEADERS GiGaMTTruth
                 INCLUDE_DIRS HepMC CLHEP Geant4 XercesC NewRnd GiGaMTCore
                 LINK_LIBRARIES HepMC CLHEP ${GEANT4_LIBS} XercesC)

gaudi_add_library(GiGaMTManagerLib
                 src/manager/*.cpp
                 PUBLIC_HEADERS GiGaMTTruth
                 INCLUDE_DIRS HepMC CLHEP Geant4 XercesC NewRnd GiGaMTCore
                 LINK_LIBRARIES HepMC CLHEP ${GEANT4_LIBS} XercesC)

gaudi_add_module(GiGaMTTruth
        src/components/*.cpp
        LINK_LIBRARIES GaudiAlgLib GiGaMTManagerLib HepMCUtils)

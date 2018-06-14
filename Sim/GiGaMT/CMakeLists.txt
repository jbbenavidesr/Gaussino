################################################################################
# Package: GiGaMT 
# This contains the fundamental GiGaMT service that sets up the entire mess in
# its initialization and provides the function to call that handles the simulation.
# Also there is the main simulation algorithm that actually uses the service
#################################################################################
gaudi_subdir(GiGaMT v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTFactories)

add_definitions(-DG4MULTITHREADED)

gaudi_add_library(GiGaMTLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GiGaMT
                  INCLUDE_DIRS CLHEP Geant4
                  LINK_LIBRARIES GaudiAlgLib 
                                 GiGaMTFactoriesLib)

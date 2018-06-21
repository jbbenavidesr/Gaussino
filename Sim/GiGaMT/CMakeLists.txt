################################################################################
# Package: GiGaMT 
# This contains the fundamental GiGaMT service that sets up the entire mess in
# its initialization and provides the function to call that handles the simulation.
# Also there is the main simulation algorithm that actually uses the service
#################################################################################
gaudi_subdir(GiGaMT v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTFactories
                         Sim/GiGaMTCore)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

gaudi_add_library(GiGaMTLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GiGaMT 
                  INCLUDE_DIRS GiGaMTFactories
                  LINK_LIBRARIES GaudiAlgLib GiGaMTCoreLib)

gaudi_add_module(GiGaMT
        src/components/*.cpp
        LINK_LIBRARIES GiGaMTLib)

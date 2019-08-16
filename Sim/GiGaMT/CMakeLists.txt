################################################################################
# Package: GiGaMT 
# This contains the fundamental GiGaMT service that sets up the entire mess in
# its initialization and provides the function to call that handles the simulation.
# Also there is the main simulation algorithm that actually uses the service
#################################################################################
gaudi_subdir(GiGaMT v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         HepMCUser
                         NewRnd
                         Sim/SimInterfaces
                         Sim/GiGaMTFactories
                         Sim/GiGaMTTruth
                         Sim/GiGaMTCore)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)
AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_library(GiGaMTLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GiGaMT 
                  INCLUDE_DIRS HepMC GiGaMTFactories SimInterfaces HepMCUser GiGaMTTruth ${HEPMC3_INCLUDE_DIR}
                  LINK_LIBRARIES GaudiAlgLib GiGaMTCoreRunLib ${HEPMC3_LIBRARIES} NewRnd )
add_dependencies(GiGaMTLib HepMC3Ext)

gaudi_add_module(GiGaMT
        src/components/*.cpp
        LINK_LIBRARIES GiGaMTLib ROOT)

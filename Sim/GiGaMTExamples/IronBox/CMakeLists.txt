################################################################################
# Package: GiGaMTExamples/IronBox
# All GiGa extensions to create a simple running example to test MT prototype.
# Detector is a 1m^3 iron cube in the center.
#################################################################################
gaudi_subdir(IronBox v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Gen/GenInterfaces 
                         Sim/GiGaMTFactories
                         Sim/GiGaMTCore
                         Sim/GiGaMTGeo)
find_package(HepMC CONFIG)
add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

gaudi_add_module(IronBoxExample
                  src/*.cpp
                  INCLUDE_DIRS GiGaMTFactories GenInterfaces HepMC
                  LINK_LIBRARIES GaudiAlgLib GiGaMTCoreRunLib HepMC)

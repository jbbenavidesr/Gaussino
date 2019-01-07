################################################################################
# Package: GiGaMTFactories 
#
# This package will contain the factories to construct the various G4 objects.
# This is done to clearly separate the G4 world from any Gaudi configurables parts.
# All factories should be thread-safe and implement a constant method 'construct' which
# and returns the constructed type by value.
# FIXME: This requirement should probably be enforced somehow...
# 
#################################################################################
gaudi_subdir(GiGaMTFactories v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTCore
                         Sim/GiGaMTGeo) 

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

# GigaMTCoreLib should pull in all the dependencies on Geant 4 etc
gaudi_add_module(GiGaMTRunFactories
        src/run/*.cpp
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

gaudi_add_module(GiGaMTPhysFactories
        src/phys/*.cpp
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

gaudi_add_module(GiGaMTDetFactories
        src/det/*.cpp
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

#gaudi_add_module(GiGaMTSensDetFactories
        #src/sensdet/*.cpp
        #LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

gaudi_install_headers(GiGaMTFactories)

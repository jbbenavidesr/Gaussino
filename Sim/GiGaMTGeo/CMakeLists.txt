################################################################################
# Package: GiGaMTGeo 
#
# This package will contain the factories to construct the various G4 objects.
# This is done to clearly separate the G4 world from any Gaudi configurables parts.
# All factories should be thread-safe and implement a constant method 'construct' which
# and returns the constructed type by value.
# FIXME: This requirement should probably be enforced somehow...
# 
#################################################################################
gaudi_subdir(GiGaMTGeo v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/SimInterfaces
                         Sim/GiGaMTCore) 

AddHepMC3()
add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

gaudi_add_module(GiGaMTGeoUtils
        src/*.cpp
        INCLUDE_DIRS SimInterfaces
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib GiGaMTCoreCutLib)

gaudi_install_headers(GiGaMTGeo)
gaudi_install_headers(GiGaMTRegions)

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
                         Sim/GiGaMTCore) 

# GigaMTCoreLib should pull in all the dependencies on Geant 4 etc
gaudi_add_library(GiGaMTFactoriesLib
        src/*.cpp
        PUBLIC_HEADERS GiGaMTFactories
        LINK_LIBRARIES GiGaMTCoreLib)

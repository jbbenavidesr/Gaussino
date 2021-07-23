###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
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
                         Sim/SimInterfaces
                         HepMC3
                         Sim/GiGaMTGeo) 

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()

AddHepMC3()

# GigaMTCoreLib should pull in all the dependencies on Geant 4 etc
gaudi_add_module(GiGaMTRunFactories
        src/run/*.cpp
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

gaudi_add_module(GiGaMTPhysFactories
        src/phys/*.cpp
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

gaudi_add_module(GiGaMTDetFactories
        src/det/*.cpp
        INCLUDE_DIRS SimInterfaces
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib)

gaudi_add_module(GiGaMTTruthFactories
        src/truth/*.cpp
        LINK_LIBRARIES GiGaMTCoreTruthLib GaudiAlgLib)

gaudi_add_library(GiGaMTMagnetFactoriesLib
                  src/magnet/lib/*.cpp
                  PUBLIC_HEADERS GiGaMTFactories
                  LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib GaudiKernel)

gaudi_add_module(GiGaMTMagnetFactories
        src/magnet/components/*.cpp
        LINK_LIBRARIES GiGaMTMagnetFactoriesLib)

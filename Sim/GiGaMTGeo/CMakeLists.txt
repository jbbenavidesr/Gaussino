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
if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()

gaudi_add_module(GiGaMTGeoUtils
        src/*.cpp
        INCLUDE_DIRS SimInterfaces
        LINK_LIBRARIES GiGaMTCoreRunLib GaudiAlgLib GiGaMTCoreCutLib)

gaudi_install_headers(GiGaMTGeo)
gaudi_install_headers(GiGaMTRegions)
gaudi_add_test(QMTest QMTEST)

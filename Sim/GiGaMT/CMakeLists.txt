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

AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()

gaudi_add_library(GiGaMTLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GiGaMT 
                  INCLUDE_DIRS HepMC GiGaMTFactories SimInterfaces HepMCUser GiGaMTTruth ${HEPMC3_INCLUDE_DIR}
                  LINK_LIBRARIES GaudiAlgLib GiGaMTCoreRunLib ${HEPMC3_LIBRARIES} NewRnd )
add_dependencies(GiGaMTLib HepMC3Ext)

gaudi_add_module(GiGaMT
        src/components/*.cpp
        LINK_LIBRARIES GiGaMTLib ROOT)

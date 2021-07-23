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
# Package: GiGaMTTruth 
#
# This package contains all code related to the tracking and extraction of truth
# information from the Geant4 propagation
#################################################################################
gaudi_subdir(GiGaMTTruth v1r0)
gaudi_depends_on_subdirs(NewRnd
                         GaudiAlg
                         HepMCUser
                         Kernel/PartProp
                         Sim/GiGaMTCore)

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()


AddHepMC3()

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

gaudi_install_headers(GiGaMTTruth)

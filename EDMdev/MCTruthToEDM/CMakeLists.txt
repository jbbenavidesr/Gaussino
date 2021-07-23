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
gaudi_subdir(MCTruthToEDM v1r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         Sim/GiGaMTCore
                         Defaults
                         HepMC3
                         HepMCUser)

AddHepMC3()
find_package(HepMC)


gaudi_add_library(MCTruthToEDMLib
                 src/lib/*.cpp
                 PUBLIC_HEADERS MCTruthToEDM
                 LINK_LIBRARIES GiGaMTCoreTruthLib MCEvent)

gaudi_add_module(MCTruthToEDM
                 src/components/*.cpp
                 INCLUDE_DIRS ${HEPMC_INCLUDE_DIR} Defaults
                 LINK_LIBRARIES MCTruthToEDMLib)

add_dependencies(MCTruthToEDM HepMC3Ext)

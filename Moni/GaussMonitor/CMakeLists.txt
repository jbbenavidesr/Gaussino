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
# Package: GaussMonitor
################################################################################
gaudi_subdir(GaussMonitor v8r2)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         GaudiAlg
                         Defaults
                         HepMC3
                         HepMCUser
                         Kernel/PartProp)

AddHepMC3()

gaudi_add_module(GaussMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA ${HEPMC3_INCLUDE_DIR} HepMCUser
                 LINK_LIBRARIES GaudiAlgLib GenEvent PartPropLib ${HEPMC3_LIBRARIES} MCEvent)

add_dependencies(GaussMonitor HepMC3Ext)

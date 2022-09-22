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
#[=======================================================================[.rst:
Moni/GaussMonitor
-----------------
#]=======================================================================]

gaudi_add_module(GaussMonitor
    SOURCES
        src/Components/GaussGenUtil.cpp
        src/Components/GenMonitorAlg.cpp
        src/Components/MCTruthMonitor.cpp
    LINK
        HepMC3::HepMC3
        LHCb::MCEvent     # TODO: [LHCb DEPENDENCY]
        LHCb::PartPropLib # TODO: [LHCb DEPENDENCY]
        Gaussino::Defaults
        Gaussino::HepMCUserLib
)

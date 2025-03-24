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
Sim/MCCollector
---------------
#]=======================================================================]

gaudi_add_library(MCCollectorLib
    SOURCES
        src/Lib/MCCollectorHit.cpp
        src/Lib/MCCollectorSensDet.cpp
        src/Lib/MCCollectorTuple.cpp
    LINK
        PUBLIC
            Gaussino::GiGaMTCoreRunLib
            Gaussino::GiGaMTFactoriesLib
            Gaussino::GiGaMTDetFactoriesLib
            Gaussino::MCTruthToEDMLib
            Gaussino::EDMLib
)

gaudi_add_module(MCCollector
    SOURCES
        src/Components/GetMCCollectorHitsAlg.cpp
        src/Components/MCCollector.cpp
        src/Components/MCCollectorSensDetComponent.cpp
    LINK
    	Gaussino::MCCollectorLib
)

gaudi_add_pytest()

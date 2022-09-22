###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
#[=======================================================================[.rst:
Sim/MCCollector
---------------

Author: Michał Mazurek
#]=======================================================================]
gaudi_add_module(MCCollector
    SOURCES
        src/Components/GetMCCollectorHitsAlg.cpp
        src/Components/MCCollectorHit.cpp
        src/Components/MCCollectorSensDet.cpp
    LINK
        Gaussino::GiGaMTCoreRunLib
        Gaussino::GiGaMTFactoriesLib
        Gaussino::GiGaMTDetFactoriesLib
        Gaussino::MCTruthToEDMLib
)

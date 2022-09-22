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
Sim/GiGaMTExamples/LHCbFromGDML
-------------------------------
#]=======================================================================]
################################################################################
# Package: LHCbFromGDML
# Simple setup to add the frozen LHCb geometry from a GDML file to Gaussino.
#
# Author: Dominik Muller, GDML file from Ben Couturier
#
################################################################################
gaudi_add_module(LHCbFromGDML
    SOURCES
        src/MinimalStableInLHCb.cpp
    LINK
        HepMC3::HepMC3
        Gaudi::GaudiAlgLib
        LHCb::GenEvent  # TODO: [LHCb DEPENDENCY]
        Gaussino::GenInterfacesLib
        Gaussino::HepMCUserLib
)

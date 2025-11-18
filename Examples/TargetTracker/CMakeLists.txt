###############################################################################
# (c) Copyright 2025 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
#[=======================================================================[.rst:
Examples/TargetTracker
----------------------

Target-tracker example based on Geant4's B2a example.
Demonstrates fixed-target geometry with tracking chambers.
#]=======================================================================]

# Build SimpleCollector library
gaudi_add_library(SimpleCollectorLib
    SOURCES
        src/Lib/SimpleCollectorHit.cpp
        src/Lib/SimpleCollectorSensDet.cpp
    LINK
        PUBLIC
            Gaussino::GiGaMTCoreRunLib
            Gaussino::GiGaMTFactoriesLib
            Gaussino::GiGaMTDetFactoriesLib
)

# Build SimpleCollector module
gaudi_add_module(SimpleCollector
    SOURCES
        src/Components/SimpleCollectorSensDetComponent.cpp
    LINK
        Gaussino::SimpleCollectorLib
)

# Install Python configuration modules
gaudi_install(PYTHON)

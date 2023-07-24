###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
#[=======================================================================[.rst:
Sim/GiGaMTExamples/G4Par04
---------------------------------------
#]=======================================================================]

gaudi_add_module(G4Par04
    SOURCES
        src/CaloG4Hit.cpp
        src/CaloSensitiveDetector.cpp
        src/CollectorG4Hit.cpp
        src/CollectorSensitiveDetector.cpp
        src/GetCaloHitsAlg.cpp
        src/GetCollectorHitsAlg.cpp
        src/Monitoring.cpp
    LINK
        Gaussino::CustomSimulationLib
        Gaussino::MCCollectorLib
        Gaussino::GiGaMTCoreRunLib
)

gaudi_install(PYTHON)
gaudi_add_pytest()
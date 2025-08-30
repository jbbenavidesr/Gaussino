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
Sim/ExampleCollectors
---------------
#]=======================================================================]

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

gaudi_add_module(SimpleCollector
    SOURCES
        src/Components/SimpleCollectorSensDetComponent.cpp
    LINK
    	Gaussino::SimpleCollectorLib
)

gaudi_add_library(CalorimeterCollectorLib
    SOURCES
        src/Lib/CalorimeterCollectorHit.cpp
        src/Lib/CalorimeterCollectorSensDet.cpp
    LINK
        PUBLIC
            Gaussino::GiGaMTCoreRunLib
            Gaussino::GiGaMTFactoriesLib
            Gaussino::GiGaMTDetFactoriesLib
)

gaudi_add_module(CalorimeterCollector
    SOURCES
        src/Components/CalorimeterCollectorSensDetComponent.cpp
	src/Components/CalorimeterMonitoring.cpp
    LINK
        Gaussino::CalorimeterCollectorLib
)


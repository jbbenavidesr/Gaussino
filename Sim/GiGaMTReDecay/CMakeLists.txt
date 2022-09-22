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
Sim/GiGaMTReDecay
-----------------

This package contains all the optional ReDecay configuration to coordinate 
the re-use of parts over the event between multiple events.
#]=======================================================================]

gaudi_add_library(GiGaMTReDecayLib
    SOURCES
        src/Lib/Token.cpp
    LINK
        PUBLIC
	    Gaussino::RndInitLib
	    Gaussino::GiGaMTLib
	    Gaussino::NewRndLib
	    Gaussino::GiGaMTCoreRunLib
	    Gaussino::GeneratorsLib
)

gaudi_add_module(GiGaMTReDecay
    SOURCES
	src/Components/ReDecayInit.cpp
        src/Components/service/ReDecaySvc.cpp
	src/Components/gen_tools/ReDecayGeneration.cpp
	src/Components/gen_tools/ReDecayPileUp.cpp
	src/Components/gen_tools/ReDecayProduction.cpp
	src/Components/gen_tools/ReDecaySignalGeneration.cpp
	src/Components/service/ReDecaySvc.cpp
	src/Components/sorters/BaseSorter.cpp
	src/Components/sorters/SignalOnly.cpp
	src/Components/sim/ReDecaySimAlg.cpp
	src/Components/sim/ReDecaySkipSimAlg.cpp
	src/Components/edm/ReDecayMCTruthToEDM.cpp
    LINK
        Gaussino::GiGaMTReDecayLib
	Gaussino::MCTruthToEDMLib
)

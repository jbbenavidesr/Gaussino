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
Gen/RndInit
-----------
#]=======================================================================]

gaudi_add_library(RndInitLib
    SOURCES
        src/Lib/GenRndInit.cpp
    LINK
        PUBLIC
	    Gaudi::GaudiAlgLib
	    LHCb::GenEvent # TODO: [LHCb DEPENDENCY]
	    Gaussino::Defaults
	    Gaussino::NewRndLib
	    Gaussino::GenBeamLib
)

gaudi_add_module(RndInit
    SOURCES
        src/Components/GenRndInitComp.cpp
	src/Components/SeedingTool.cpp
    LINK
        Gaussino::RndInitLib
)

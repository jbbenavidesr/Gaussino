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
HepMCUser
---------
#]=======================================================================]

gaudi_add_header_only_library(HepMCUserLib
    LINK
        HepMC3::HepMC3
	    LHCb::PartPropLib     # TODO: [LHCb DEPENDENCY]
        Rangev3::rangev3
)

gaudi_add_library(HepMCUtilsLib
    SOURCES
        src/Lib/CompareGenEvent.cpp
        src/Lib/PrintDecayTree.cpp
	src/Lib/Relatives.cpp
    LINK
    	PUBLIC
            HepMC3::HepMC3
            HepMC3::search
	    Gaudi::GaudiAlgLib
	    LHCb::PartPropLib     # TODO: [LHCb DEPENDENCY]
	    Gaussino::Defaults
            Gaussino::HepMCUserLib
)

gaudi_add_executable(compareHepMCEvents
    SOURCES
        exec/compare.cpp
    LINK
        HepMCUtilsLib
)
gaudi_add_pytest()

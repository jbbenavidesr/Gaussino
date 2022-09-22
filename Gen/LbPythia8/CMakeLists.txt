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
Gen/LbPytha8
------------
#]=======================================================================]
gaudi_add_library(LbPythia8Lib
    SOURCES
        src/Lib/BeamToolForPythia8.cpp
    	src/Lib/LhcbHooks.cpp
	src/Lib/Pythia8Production.cpp
	src/Lib/Pythia8ProductionMT.cpp
    LINK
        PUBLIC
	    Pythia8::Pythia8
	    HepMC3::HepMC3
	    Gaudi::GaudiAlgLib
	    LHCb::GenEvent     # TODO: [LHCb DEPENDENCY]
	    LHCb::PartPropLib  # TODO: [LHCb DEPENDENCY]
	    Gaussino::GenInterfacesLib
	    Gaussino::UtilsLib
	    Gaussino::NewRndLib
	    Gaussino::HepMCUserLib
	    Gaussino::Defaults
)

gaudi_add_module(LbPythia8
    SOURCES
        src/Components/Pythia8ProductionFactory.cpp
    LINK
    	Gaussino::LbPythia8Lib
)

# TODO: [NEW CMAKE] needed?
#gaudi_env(SET PYTHIA8XML ${PYTHIA8_XML})

gaudi_add_executable(Pythia8Reproducibility
    SOURCES
        exec/minimal.cxx
    LINK
        Pythia8::Pythia8
	HepMC3::HepMC3
	Gaussino::HepMCUtilsLib
)

gaudi_add_executable(HepMCConverterTest
    SOURCES
        exec/convtest.cxx
    LINK
    	Pythia8::Pythia8
	HepMC3::HepMC3
	HepMC::HepMC
	Gaussino::HepMCUtilsLib
)

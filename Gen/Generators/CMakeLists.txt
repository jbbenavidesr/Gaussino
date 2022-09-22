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
Gen/Generators
--------------
#]=======================================================================]

gaudi_add_library(GeneratorsLib
    SOURCES
    	src/Lib/f77units.F
        src/Lib/getaddr.cpp
	src/Lib/pythiaoutput_utils.F
	src/Lib/ExternalGenerator.cpp
	src/Lib/F77Utils.cpp
	src/Lib/Generation.cpp
	src/Lib/GenCounters.cpp
	src/Lib/LbPhotos.cpp
	src/Lib/LbTaula.cpp
	src/Lib/LhaPdf.cpp
	src/Lib/LBPHOTOS.F
	src/Lib/LBTAULA.F
	src/Lib/LHAPDFCommonBlocks.cpp
	src/Lib/LHAPDFUtils.F
	src/Lib/Scale.cpp
	src/Lib/Signal.cpp
	src/Lib/StreamForGenerator.cpp
	src/Lib/StringParse.cpp
    LINK
    	PUBLIC
	    Gaudi::GaudiAlgLib
	    LHCb::GenEvent # TODO: [LHCb DEPENDENCY]
	    Gaussino::HepMCUtilsLib
	    Gaussino::NewRndLib
	    Gaussino::UtilsLib
	    Gaussino::GenInterfacesLib
	    Gaussino::EventLib

)

gaudi_add_module(Generators
    SOURCES
        src/Components/AsymmetricCollidingBeams.cpp
	src/Components/BeamSpotMarkovChainSampleVertex.cpp
	src/Components/BeamSpotSmearVertex.cpp
	src/Components/CollidingBeams.cpp
	src/Components/FixedLuminosity.cpp
	src/Components/FixedLuminosityForRareProcess.cpp
	src/Components/FixedLuminosityForSpillOver.cpp
	src/Components/FixedNInteractions.cpp
	src/Components/FixedTarget.cpp
	src/Components/FlatSmearVertex.cpp
	src/Components/FlatZSmearVertex.cpp
	src/Components/GenerationComp.cpp
	src/Components/HepMCWriter.cpp
	src/Components/HistoSmearVertex.cpp
	src/Components/Inclusive.cpp
	src/Components/MergedEventsFilter.cpp
	src/Components/MinimumBias.cpp
	src/Components/PoissonPileUp.cpp
	src/Components/ReadHepMCAsciiFile.cpp
	src/Components/SaveSignalBInformation.cpp
	src/Components/SignalForcedFragmentation.cpp
	src/Components/SignalPlain.cpp
	src/Components/SignalRepeatedHadronization.cpp
	src/Components/StandAloneDecayTool.cpp
	src/Components/UniformSmearVertex.cpp
	src/Components/VariableLuminosity.cpp
	src/Components/XmlCounterLogFile.cpp
    LINK
        Gaussino::GeneratorsLib
)

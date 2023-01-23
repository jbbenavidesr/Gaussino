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
NewRnd
------
#]=======================================================================]

gaudi_add_library(NewRndLib
    SOURCES
        src/Lib/RndAlgSeeder.cpp
        src/Lib/RndCommon.cpp
        src/Lib/RndGlobal.cpp
    LINK
        PUBLIC
            CLHEP::Random
            Gaudi::GaudiKernel
            Gaudi::GaudiAlgLib
            Gaussino::UtilsLib
)

gaudi_add_module(NewRndComponents
    SOURCES
        src/Components/Engines.cpp
    LINK
        NewRndLib
)

gaudi_add_executable(GaussinoTestgRandom
    SOURCES
        exec/GaussinoTestgRandom.cxx
    LINK
        NewRndLib
)

gaudi_add_tests(pytest)

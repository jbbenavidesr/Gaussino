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
GaussinoML/GaussinoONNX
-----------------------
#]=======================================================================]


gaudi_add_library(GaussinoONNXLib
    SOURCES
        src/Lib/ONNXModelServer.cpp
    LINK
        PUBLIC
            onnxruntime::onnxruntime
            Gaussino::GaussinoMLLib
)

gaudi_add_module(GaussinoONNX
    SOURCES
        src/Components/ONNXModelServerSvc.cpp
    LINK
        Gaussino::GaussinoONNXLib
)
if(BUILD_TESTING)
    gaudi_add_executable(test_ONNXModelServing
        SOURCES tests/src/UnitTestONNXModelServing.cpp
        LINK
            Boost::unit_test_framework
            Gaussino::GaussinoONNXLib
        TEST
    )
endif()

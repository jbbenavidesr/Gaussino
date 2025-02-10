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
GaussinoML/GaussinoTorch
------------------------
#]=======================================================================]


gaudi_add_library(GaussinoTorchLib
    SOURCES
        src/Lib/TorchModelServer.cpp
    LINK
        PUBLIC
            ${BLAS_LIBRARIES}
            torch::torch
            torch::c10
            torch::torch_cpu
            Gaussino::GaussinoMLLib
)

gaudi_add_module(GaussinoTorch
    SOURCES
        src/Components/TorchModelServerSvc.cpp
    LINK
        Gaussino::GaussinoTorchLib
)

if(BUILD_TESTING)
    gaudi_add_executable(test_TorchModelServing
        SOURCES tests/src/UnitTestTorchModelServing.cpp
        LINK
            Boost::unit_test_framework
            Gaussino::GaussinoTorchLib
        TEST
    )
endif()

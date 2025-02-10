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
GaussinoML/GaussinoMLBase
-------------------------
#]=======================================================================]

gaudi_add_library(GaussinoMLLib
    SOURCES
        src/Lib/ModelServerBase.cpp
    LINK
        PUBLIC
            Gaudi::GaudiKernel
)

if(BUILD_TESTING AND (GSINO_USE_TORCH OR GSINO_USE_ONNXRUNTIME))
    gaudi_add_module(TestGaussinoML
    SOURCES
        tests/src/ModelServingTestingAlgs.cpp
    LINK
        Gaudi::GaudiAlgLib
        Gaussino::NewRndLib
    )

    if(GSINO_USE_TORCH)
        target_link_libraries(TestGaussinoML
            PUBLIC
                Gaussino::GaussinoTorchLib
        )
    endif()

    if(GSINO_USE_ONNXRUNTIME)
        target_link_libraries(TestGaussinoML
            PUBLIC
                Gaussino::GaussinoONNXLib
        )
    endif()

    gaudi_add_pytest()
endif()

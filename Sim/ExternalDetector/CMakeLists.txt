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
Sim/ExternalDetector
--------------------

Author: Michał Mazurek
#]=======================================================================]
gaudi_add_library(ExternalDetectorLib
    SOURCES
        src/Lib/Embedder.cpp
        src/Lib/MaterialFactory.cpp
    LINK
        PUBLIC
            Gaudi::GaudiAlgLib
            XercesC::XercesC
            Geant4::G4digits_hits
            Geant4::G4geometry
            Gaussino::GiGaMTFactoriesLib
            Gaussino::GiGaMTCoreDetLib
)

gaudi_add_module(ExternalDetector
    SOURCES
        src/Components/CuboidEmbedder.cpp
        src/Components/MaterialFromChemicalPropertiesFactory.cpp
        src/Components/MaterialFromElements.cpp
        src/Components/WorldCreator.cpp
    LINK
        Gaussino::ExternalDetectorLib
        Gaussino::GiGaMTGeoLib
)

gaudi_install(PYTHON)
gaudi_generate_confuserdb()
gaudi_add_tests(pytest)

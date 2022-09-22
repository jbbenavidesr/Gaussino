###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
#[=======================================================================[.rst:
Sim/ParallelGeometry
--------------------
#]=======================================================================]
gaudi_add_library(ParallelGeometryLib
    SOURCES
        src/Lib/Physics.cpp
        src/Lib/World.cpp
    LINK
        PUBLIC
            XercesC::XercesC
            Geant4::G4physicslists
            Gaussino::GiGaMTPhysFactoriesLib
            Gaussino::GiGaMTCoreRunLib
)

gaudi_add_module(ParallelGeometry
    SOURCES
        src/Components/DefaultPhysicsFactory.cpp
        src/Components/DefaultWorldFactory.cpp
    LINK
        Geant4::G4persistency
        Gaussino::ParallelGeometryLib
)

gaudi_install(PYTHON)
gaudi_generate_confuserdb()
gaudi_add_tests(QMTest)

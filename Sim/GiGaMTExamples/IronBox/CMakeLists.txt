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
Sim/GiGaMTExamples/IronBox
--------------------------
#]=======================================================================]
################################################################################
# Package: GiGaMTExamples/IronBox
# All GiGa extensions to create a simple running example to test MT prototype.
# Detector is a 1m^3 iron cube in the center.
#################################################################################
gaudi_add_module(IronBoxExample
    SOURCES
        src/IronBoxGeo.cpp
        src/MinimalParticles.cpp
    LINK
        HepMC::HepMC
        Gaussino::GiGaMTCoreRunLib
        Gaussino::GiGaMTGeoLib
        Gaussino::GenInterfacesLib
)

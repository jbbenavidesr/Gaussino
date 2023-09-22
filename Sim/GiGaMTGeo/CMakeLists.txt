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
Sim/GiGaMTGeo
-------------
#]=======================================================================]
###############################################################################
# Package: GiGaMTGeo
#
# This package will contain the factories to construct the various G4 objects.
# This is done to clearly separate the G4 world from any Gaudi configurables parts.
# All factories should be thread-safe and implement a constant method 'construct' which
# and returns the constructed type by value.
# FIXME: This requirement should probably be enforced somehow...
#
###############################################################################

gaudi_add_header_only_library(GiGaMTGeoLib
    LINK
        Gaudi::GaudiKernel
)
gaudi_add_header_only_library(GiGaMTRegionsLib
    LINK
        Gaudi::GaudiKernel
)

gaudi_add_module(GiGaMTGeoUtils
    SOURCES
	src/Components/GDMLReader.cpp
        src/Components/GiGaRegionsTool.cpp
	src/Components/GiGaRegionTool.cpp
	src/Components/GiGaSetSimAttributes.cpp
    LINK
    	Gaudi::GaudiAlgLib
	Gaussino::GiGaMTRegionsLib
        Gaussino::SimInterfacesLib
	Gaussino::GiGaMTCoreCutLib
	Gaussino::GiGaMTCoreRunLib
	Geant4::G4persistency
)

gaudi_add_pytest()

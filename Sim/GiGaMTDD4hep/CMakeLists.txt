###############################################################################
# (c) Copyright 2018 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
#[=======================================================================[.rst:
Sim/GiGaMTDD4hep
----------------
#]=======================================================================]
################################################################################
# Package: GiGaMTDD4hep
# Interfaces to load the detector from the DD4hep geometry service
################################################################################

if(USE_DD4HEP)
    gaudi_add_library(GiGaMTDD4hepLib
        SOURCES
            src/Lib/DD4hepCnvSvc.cpp
            src/Lib/DD4hepDetectorConstruction.cpp
            src/Lib/Utilities.cpp
        LINK
            PUBLIC
                DD4hep::DDCore
                DD4hep::DDG4
                Gaussino::GiGaMTFactoriesLib
		Gaussino::GiGaMTGeoLib
                Gaussino::GiGaMTCoreRunLib
    ) 

    gaudi_add_module(GiGaMTDD4hep
        SOURCES
            src/Components/DD4hepCnvSvcConf.cpp
            src/Components/DD4hepDetectorConstructionFAC.cpp
        LINK
            LHCb::LbDD4hepLib # FIXME: [LHCb DEPENDENCY]
            Gaussino::GiGaMTDD4hepLib
    )
endif()

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
################################################################################
# Package: GiGaMTCore 
#
# This package should contain all modified core Geant4 classes (e.g.
# run managers). While the Gaudi cmake plugins are used to handle this package,
# none of the modified classes should depend on anything Gaudi related.
#################################################################################
gaudi_subdir(GiGaMTCore v1r0)
gaudi_depends_on_subdirs(NewRnd
                         Event/MCEvent  # FIXME: Don't like this
                         HepMC3
                         HepMCUser)

find_package(CLHEP COMPONENTS Random Vector)
find_package(XercesC)
AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()


gaudi_add_library(GiGaMTCoreMessageLib
                 src/message/*.cpp
                 PUBLIC_HEADERS GiGaMTCoreMessage)

gaudi_add_library(GiGaMTCoreTruthLib
                 src/truth/*.cpp
                 PUBLIC_HEADERS GiGaMTCoreTruth
                 INCLUDE_DIRS ${HEPMC3_INCLUDE_DIR} CLHEP Geant4 XercesC NewRnd HepMCUtils
                 LINK_LIBRARIES ${HEPMC3_LIBRARIES} CLHEP ${GEANT4_LIBS} XercesC ${ROOT_LIBRARIES} HepMCUtils MCEvent  GiGaMTCoreMessageLib)

add_dependencies(GiGaMTCoreTruthLib HepMC3Ext)

gaudi_add_library(GiGaMTCoreRunLib
                 src/run/*.cpp
                 PUBLIC_HEADERS GiGaMTCoreRun
                 INCLUDE_DIRS ${HEPMC3_INCLUDE_DIR} CLHEP Geant4 XercesC NewRnd
                 LINK_LIBRARIES ${HEPMC3_LIBRARIES} CLHEP ${GEANT4_LIBS} XercesC GiGaMTCoreTruthLib GiGaMTCoreMessageLib)

gaudi_add_library(GiGaMTCoreCutLib
                 src/cut/*.cpp
                 PUBLIC_HEADERS GiGaMTCoreCut
                 INCLUDE_DIRS CLHEP Geant4
                 LINK_LIBRARIES CLHEP ${GEANT4_LIBS}  GiGaMTCoreMessageLib)

gaudi_install_headers(GiGaMTCoreDet)

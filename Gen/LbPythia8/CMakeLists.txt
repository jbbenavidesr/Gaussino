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
# Package: LbPythia8
################################################################################
gaudi_subdir(LbPythia8 v12r0)

gaudi_depends_on_subdirs(Gen/GenInterfaces
                         Event/GenEvent
                         Kernel/PartProp
                         GaudiAlg
                         NewRnd
                         HepMC3
                         HepMCUser
                         Utils
                         Defaults)

find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
AddHepMC3()
find_package(LHAPDF)
find_package(Boost COMPONENTS filesystem system)
find_package(ROOT)
find_package(CLHEP)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${HEPMC3_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS})

gaudi_add_library(LbPythia8Lib
                  src/Lib/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Boost Pythia8 ROOT ${HEPMC3_INCLUDE_DIR} Gen/GenInterfaces Defaults HepMCUser Utils NewRnd
                  LINK_LIBRARIES Boost GaudiAlgLib Pythia8 ${HEPMC3_LIBRARIES} ROOT PartPropLib LHAPDF GenEvent)

add_dependencies(LbPythia8Lib HepMC3Ext)

gaudi_add_module(LbPythia8
                 src/component/*.cpp
                 LINK_LIBRARIES LbPythia8Lib)

gaudi_env(SET PYTHIA8XML ${PYTHIA8_XML})

gaudi_add_executable(Pythia8Reproducibility
                     exec/minimal.cxx
                     INCLUDE_DIRS Pythia8 ${HEPMC3_INCLUDE_DIR} CLHEP HepMCUtils
                     LINK_LIBRARIES Pythia8 ${HEPMC3_LIBRARIES} CLHEP HepMCUtils)

find_package(HepMC)

gaudi_add_executable(HepMCConverterTest
                     exec/convtest.cxx
                     INCLUDE_DIRS Pythia8 ${HEPMC3_INCLUDE_DIR} CLHEP HepMCUtils HepMC
                     LINK_LIBRARIES Pythia8 ${HEPMC3_LIBRARIES} CLHEP HepMCUtils HepMC)

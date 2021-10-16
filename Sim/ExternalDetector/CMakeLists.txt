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
# Package: ExternalDetector
################################################################################
gaudi_subdir(ExternalDetector v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTCore
                         Sim/GiGaMTFactories)


AddHepMC3()                    
find_package(Boost)
find_package(CLHEP)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})

gaudi_add_module(ExternalDetector
                 src/*.cpp
                 INCLUDE_DIRS GiGaMTFactories XercesC ${HEPMC3_INCLUDE_DIR}
                 LINK_LIBRARIES GaudiAlgLib GiGaMTCoreRunLib DetDescLib XercesC ${HEPMC3_LIBRARIES} ${GEANT4_LIBS})

gaudi_install_python_modules()
gaudi_add_test(QMTest QMTEST)

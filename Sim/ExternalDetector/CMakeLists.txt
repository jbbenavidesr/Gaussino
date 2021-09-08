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
################################################################################
# Package: ExternalDetector
################################################################################
gaudi_subdir(ExternalDetector v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GaussTools)


find_package(XercesC)
find_package(Boost)
find_package(CLHEP)

FindG4libs(persistency)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})

gaudi_add_module(ExternalDetector
                 src/*.cpp
                 INCLUDE_DIRS XercesC
                 LINK_LIBRARIES GaudiAlgLib GaussToolsLib XercesC ${GEANT4_LIBS})

gaudi_install_python_modules()
gaudi_add_test(QMTest QMTEST)

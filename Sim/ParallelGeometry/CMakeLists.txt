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
# Package: ParallelGeometry
################################################################################
gaudi_subdir(ParallelGeometry v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTCore
                         Sim/GiGaMTFactories)

AddHepMC3()                                              
find_package(Boost)
find_package(CLHEP)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})

gaudi_add_library(ParallelGeometryLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS ParallelGeometry
                  INCLUDE_DIRS Sim/GiGaMTFactories ${RANGEV3_INCLUDE_DIRS}
                  LINK_LIBRARIES GaudiAlgLib GiGaMTCoreRunLib ${HEPMC3_LIBRARIES} ${GEANT4_LIBS})

gaudi_add_module(ParallelGeometry
                 src/Components/*.cpp
                 LINK_LIBRARIES ParallelGeometryLib)

gaudi_install_python_modules()
gaudi_add_test(QMTest QMTEST)

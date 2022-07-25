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
# Package: GiGaMTVis
################################################################################
gaudi_subdir(GiGaMTVis v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GiGaMTCore
                         Sim/GiGaMTFactories)

FindG4libs(
  event 
  graphics_reps
  vis_management
  Tree
)

find_package(Boost)
find_package(CLHEP)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})


gaudi_add_module(GiGaMTVis
                 src/*.cpp
                 INCLUDE_DIRS Sim/GiGaMTFactories 
                 LINK_LIBRARIES GaudiAlgLib GiGaMTCoreMessageLib ${GEANT4_LIBS})

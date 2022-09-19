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
# Package: GiGaMTPhoenix
################################################################################
gaudi_subdir(GiGaMTPhoenix v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Vis/Phoenix
                         Sim/GiGaMTCore)

AddHepMC3()

find_package(CLHEP)

include_directories(SYSTEM ${CLHEP_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})


gaudi_add_module(GiGaMTPhoenixAlgs
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GiGaMTCoreRunLib PhoenixLib ${GEANT4_LIBS})

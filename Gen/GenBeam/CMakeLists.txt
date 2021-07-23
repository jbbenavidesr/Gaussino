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
# Package: GenBeam
################################################################################
gaudi_subdir(GenBeam v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/GenEvent
                         NewRnd
                         HepMCUser
                         Gen/GenInterfaces
                         #FIXME: For platform and kernel streaming, I think ?!?!
                         Kernel/LHCbKernel)

AddHepMC3()

gaudi_add_module(GenBeam
                 src/*.cpp
                 INCLUDE_DIRS Gen/GenInterfaces ${HEPMC3_INCLUDE_DIR} HepMCUser
                 LINK_LIBRARIES GaudiAlgLib GenEvent LHCbKernel NewRnd ${HEPMC3_LIBRARIES})

add_dependencies(GenBeam HepMC3Ext)

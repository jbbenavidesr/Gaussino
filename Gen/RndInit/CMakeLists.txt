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
# Package: GaussAlgs
################################################################################
gaudi_subdir(RndInit v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         NewRnd
                         Gen/GenBeam
                         Event/GenEvent
                         #FIXME: For platform and kernel streaming, I think ?!?!
                         Kernel/LHCbKernel
                         Defaults)

gaudi_add_library(RndInitLib
                 src/lib/*.cpp
                 PUBLIC_HEADERS RndInit
                 INCLUDE_DIRS GenBeam
                 LINK_LIBRARIES GaudiAlgLib GenEvent LHCbKernel NewRnd)

gaudi_add_module(RndInit
                 src/components/*.cpp
                 LINK_LIBRARIES RndInitLib)

###############################################################################
# (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from GaudiKernel.Configurable import (
    applyConfigurableUsers,
    Configurable,
)

from Gaudi.Configuration import importOptions

# small workaround to reset confiurables
# between the tests
import GaudiKernel.Configurable  # needed to change global var
import GaudiKernel.ProcessJobOptions  # needed to change global var


def reset_configurables(example):
    def wrapper(*args, **kwargs):
        for conf in Configurable.allConfigurables.values():
            conf.configurables.clear()
        Configurable.allConfigurables.clear()
        GaudiKernel.Configurable._appliedConfigurableUsers_ = False
        GaudiKernel.ProcessJobOptions._included_files = set()
        example(*args, **kwargs)

    return wrapper


def em_physics(example):
    def wrapper(*args, **kwargs):
        importOptions("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")
        example(*args, **kwargs)

    return wrapper


def photon(example):
    def wrapper(*args, **kwargs):
        importOptions(
            "$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py"
        )
        example(*args, **kwargs)

    return wrapper


def one_event(example):
    def wrapper(*args, **kwargs):
        importOptions("$GAUSSINOOPTS/General/Events-1.py")
        example(*args, **kwargs)

    return wrapper


def cube(example):
    def wrapper(*args, **kwargs):
        importOptions("$GAUSSINOOPTS/Geometry/ExternalDetector-SimpleCube.py")
        example(*args, **kwargs)

    return wrapper

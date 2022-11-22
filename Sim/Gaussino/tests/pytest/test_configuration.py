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
import pytest
from helpers import *


@reset_configurables
@one_event
@em_physics
@photon
@cube
def test_minimum_working_example():
    applyConfigurableUsers()


@reset_configurables
@em_physics
@photon
@cube
def test_undefined_evt_max():
    with pytest.raises(ValueError, match=r".*EvtMax.*"):
        applyConfigurableUsers()


@reset_configurables
@one_event
@em_physics
@cube
def test_no_particle_gun():
    with pytest.raises(AttributeError, match=r".*ParticleGun.*"):
        from Configurables import GaussinoGeneration

        GaussinoGeneration().ParticleGun = True
        applyConfigurableUsers()


@reset_configurables
@one_event
@em_physics
@photon
@cube
def test_no_hive():
    with pytest.raises(ValueError, match=r".*EnableHive.*"):
        from Configurables import Gaussino

        Gaussino().EnableHive = False
        applyConfigurableUsers()

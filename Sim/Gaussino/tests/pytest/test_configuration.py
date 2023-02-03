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
from Gaussino.pytest.helpers import *
from Gaussino.pytest.options import *


@reset_configurables
@events_1
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
        from Configurables import Gaussino
        Gaussino()
        applyConfigurableUsers()


@reset_configurables
@events_1
@em_physics
@cube
def test_no_particle_gun():
    with pytest.raises(AttributeError, match=r".*ParticleGun.*"):
        from Configurables import GaussinoGeneration

        GaussinoGeneration().ParticleGun = True
        applyConfigurableUsers()


@reset_configurables
@events_1
@em_physics
@photon
@cube
def test_no_hive():
    with pytest.raises(ValueError, match=r".*EnableHive.*"):
        from Configurables import Gaussino

        Gaussino().EnableHive = False
        applyConfigurableUsers()

@reset_configurables
@events_1
@only_generation
@photon
@pytest.mark.parametrize(
    "writer, correct", [
        ("WriterRootTree", True),
        ("WriterRootTree", True),
        ("WriterAscii", True),
        ("WriterHEPEVT", True),
        ("WriterThatDoesNotExist", False),
    ]
)
def test_hepmcwriter(writer, correct):
    from Configurables import (
        GaussinoGeneration,
        HepMCWriter,
    )
    GaussinoGeneration().WriteHepMC = True
    HepMCWriter().Writer = writer
    if correct:
        applyConfigurableUsers()
    else:
        with pytest.raises(ValueError, match=r".*HepMCWriter.*"):
            applyConfigurableUsers()

@reset_configurables
@events_1
@em_physics
@photon
@cube
@pytest.mark.parametrize(
    "redecay", [
        True,
        False,
    ]
)
def test_no_run_number_through_genrndinit(redecay):
    with pytest.raises(ValueError, match=r".*RunNumber.*"):
        from Configurables import Gaussino
        Gaussino().ReDecay = redecay
        if not redecay:
            from Configurables import GenRndInit
            GenRndInit().RunNumber = 1
        else:
            from Configurables import GenReDecayInit
            GenReDecayInit().RunNumber = 1
        applyConfigurableUsers()

@reset_configurables
@events_1
@em_physics
@photon
@cube
@pytest.mark.parametrize(
    "redecay", [
        True,
        False,
    ]
)
def test_correct_genrndinit_setup(redecay):
    from Configurables import Gaussino
    Gaussino().ReDecay = redecay
    run_no = 0
    if not redecay:
        from Configurables import GenRndInit
        run_no = GenRndInit().getProp("RunNumber")
    else:
        from Configurables import GenReDecayInit
        run_no = GenReDecayInit().getProp("RunNumber")
    Gaussino().RunNumber = run_no + 1
    applyConfigurableUsers()
    if not redecay:
        assert GenRndInit().getProp("RunNumber") == run_no + 1
    else:
        assert GenReDecayInit().getProp("RunNumber") == run_no + 1


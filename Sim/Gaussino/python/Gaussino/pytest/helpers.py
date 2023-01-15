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
import decorator
from contextlib import redirect_stdout
import io

from GaudiKernel.Configurable import (
    applyConfigurableUsers,
    Configurable,
)
# small workaround to reset confiurables
# between the tests
import GaudiKernel.Configurable  # needed to change global var
import GaudiKernel.ProcessJobOptions  # needed to change global var

def reset_configurables_now():
    for conf in Configurable.allConfigurables.values():
        conf.configurables.clear()
    Configurable.allConfigurables.clear()
    GaudiKernel.Configurable._appliedConfigurableUsers_ = False
    GaudiKernel.ProcessJobOptions._included_files = set()

def reset_configurables(example):
    def wrapper(example, *args, **kwargs):
        reset_configurables_now()
        example(*args, **kwargs)

    return decorator.decorator(wrapper, example)


def run_gaudi(*args, **kwargs):
    # from GaudiTests
    from subprocess import run

    cmd = ["gaudirun.py"]
    cmd += GaudiKernel.ProcessJobOptions._included_files
    cmd.extend(str(x) for x in args)
    ex = run(cmd, capture_output=True, text=True, **kwargs)
    print(ex.stdout)  # will print only with `pytest ... -s`
    print(ex.stderr)
    return ex

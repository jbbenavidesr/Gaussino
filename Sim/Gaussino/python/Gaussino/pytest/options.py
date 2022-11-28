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
from Gaudi.Configuration import importOptions

SHORT_OPTIONS = {
    "debug": "$GAUSSINOOPTS/General/Debug.py",
    "em_physics": "$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py",
    "edm": "$GAUSSINOOPTS/General/ConvertEDM.py",
    "photon": "$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py",
    "events_1": "$GAUSSINOOPTS/General/Events-1.py",
    "events_10": "$GAUSSINOOPTS/General/Events-10.py",
    "cube": "$GAUSSINOOPTS/Geometry/ExternalDetector-SimpleCube.py",
    "empty_world": "$GAUSSINOOPTS/Geometry/ExternalDetector-EmptyWorld.py",
    "only_generation": "$GAUSSINOOPTS/General/OnlyGeneration.py",
    "minbias": "$GAUSSINOOPTS/Generation/MinBias_Pythia8TL.py",
    "pp": "$GAUSSINOOPTS/Generation/pp-900GeV-HeadOn.py",
    "debug": "$GAUSSINOOPTS/General/Debug.py",
}


SHORT_OPTIONS_TEMPLATE = """
def {0}(example):
    def wrapper(example, *args, **kwargs):
        importOptions("{1}")
        example(*args, **kwargs)

    return decorator.decorator(wrapper, example)
"""


for short_name, option_file in SHORT_OPTIONS.items():
    exec(SHORT_OPTIONS_TEMPLATE.format(short_name, option_file))

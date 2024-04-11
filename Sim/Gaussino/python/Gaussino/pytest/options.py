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


def add_option(option_file):
    def wrapper(example, *args, **kwargs):
        importOptions(option_file)
        example(*args, **kwargs)

    return lambda example: decorator.decorator(wrapper, example)


debug = add_option("$GAUSSINOOPTS/General/Debug.py")
em_physics = add_option("$GAUSSINOOPTS/Simulation/G4EmStandardPhysics.py")
edm = add_option("$GAUSSINOOPTS/General/ConvertEDM.py")
photon = add_option("$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Photon1GeV.py")
electron = add_option(
    "$GAUSSINOOPTS/Generation/ParticleGun-FixedMomentum-Electron1GeV.py"
)
events_1 = add_option("$GAUSSINOOPTS/General/Events-1.py")
events_10 = add_option("$GAUSSINOOPTS/General/Events-10.py")
cube = add_option("$GAUSSINOOPTS/Geometry/ExternalDetector-SimpleCube.py")
empty_world = add_option("$GAUSSINOOPTS/Geometry/ExternalDetector-EmptyWorld.py")
only_generation = add_option("$GAUSSINOOPTS/General/OnlyGeneration.py")
minbias = add_option("$GAUSSINOOPTS/Generation/MinBias_Pythia8TL.py")
pp = add_option("$GAUSSINOOPTS/Generation/pp-900GeV-HeadOn.py")
pp_7_tev = add_option("$GAUSSINOOPTS/Generation/pp-7TeV-DefaultOptics.py")
flat = add_option("$GAUSSINOOPTS/Generation/SetFlatOptics.py")

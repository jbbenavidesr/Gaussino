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
from Configurables import GaussinoGeneration
from GaudiKernel import SystemOfUnits as units

GaussinoGeneration(
    BeamMomentum=900 * units.GeV,
    RevolutionFrequency=11.245 * units.kilohertz,
    TotalCrossSection=65.3 * units.millibarn,
    InteractionPosition=[0.0] * 3,
    BeamHCrossingAngle=0.0 * units.mrad,
    BeamVCrossingAngle=0.0 * units.mrad,
    BeamLineAngles=[0.0, 0.0],
)

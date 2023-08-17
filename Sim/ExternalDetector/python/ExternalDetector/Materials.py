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
from GaudiKernel import SystemOfUnits as units

OUTER_SPACE = {
    "AtomicNumber": 1.0,
    "MassNumber": 1.01 * units.g / units.mole,
    "Density": 1.0e-25 * units.g / units.cm3,
    "Pressure": 3.0e-18 * units.pascal,
    "Temperature": 2.73 * units.kelvin,
}

LEAD = {
    "Type": "MaterialFromElements",
    "Symbols": ["Pb"],
    "AtomicNumbers": [82.0],
    "MassNumbers": [207.2 * units.g / units.mole],
    "MassFractions": [1.0],
    "Density": 11.29 * units.g / units.cm3,
    "State": "Solid",
}

SILICA = {
    "Type": "MaterialFromElements",
    "Symbols": ["Si"],
    "AtomicNumbers": [14.0],
    "MassNumbers": [27.9769 * units.g / units.mole],
    "MassFractions": [1.0],
    "Density": 2.329 * units.g / units.cm3,
    "State": "Solid",
}

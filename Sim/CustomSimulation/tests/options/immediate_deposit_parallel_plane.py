###############################################################################
# (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Configurables import (
    CustomSimulation,
    ExternalDetectorEmbedder,
    GaussinoGeometry,
    GaussinoSimulation,
    ParallelGeometry,
)
from ExternalDetector.Materials import LEAD, OUTER_SPACE
from GaudiKernel import SystemOfUnits as units

GaussinoSimulation(
    CustomSimulation="MassFastSimulation",
)

GaussinoGeometry(
    ExternalDetectorEmbedder="MassGeometryEmbedder",
)

ExternalDetectorEmbedder(
    "MassGeometryEmbedder",
    Shapes={
        "MassVacuumCalo": {
            "Type": "Cuboid",
            "zPos": 10.0 * units.m,
            "xSize": 10.0 * units.m,
            "ySize": 10.0 * units.m,
            "zSize": 2.0 * units.m,
        },
    },
    Sensitive={
        "MassVacuumCalo": {
            "Type": "MCCollectorSensDet",
            "PrintStats": True,
        },
    },
    World={
        "WorldMaterial": "OuterSpace",
        "Type": "ExternalWorldCreator",
    },
    Materials={
        "OuterSpace": OUTER_SPACE,
        "Pb": LEAD,
    },
)

CustomSimulation(
    "MassFastSimulation",
    Model={
        "MassImmediateDeposit": {
            "Type": "ImmediateDepositModel",
        }
    },
    Region={
        "MassImmediateDeposit": {
            "SensitiveDetectorName": "MassVacuumCaloSDet",
        }
    },
    Physics={
        "ParticlePIDs": [22],
    },
)

ExternalDetectorEmbedder(
    "ParallelGeometryEmbedder",
    Shapes={
        "ParallelLeadCalo": {
            "Type": "Cuboid",
            "zPos": 9.99999 * units.m,
            "xSize": 10.0 * units.m,
            "ySize": 10.0 * units.m,
            "zSize": 2 * units.m,
            "MaterialName": "Pb",
        },
    },
    Sensitive={
        "ParallelLeadCalo": {
            "Type": "MCCollectorSensDet",
            "PrintStats": True,
        },
    },
)

ParallelGeometry(
    ParallelWorlds={
        "ParallelWorld": {
            "ExternalDetectorEmbedder": "ParallelGeometryEmbedder",
            "CustomSimulation": "ParallelFastSimulation",
        },
    },
    ParallelPhysics={
        "ParallelWorld": {
            "LayeredMass": True,
            "ParticlePIDs": [22],
        },
    },
)

CustomSimulation(
    "ParallelFastSimulation",
    Model={
        "ParallelImmediateDeposit": {
            "Type": "ImmediateDepositModel",
        }
    },
    Region={
        "ParallelImmediateDeposit": {
            "SensitiveDetectorName": "ParallelLeadCaloSDet",
        }
    },
    Physics={
        "ParticlePIDs": [22],
    },
)

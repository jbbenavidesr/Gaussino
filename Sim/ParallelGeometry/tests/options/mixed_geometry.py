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
from Configurables import ExternalDetectorEmbedder, GaussinoGeometry, ParallelGeometry
from ExternalDetector.Materials import LEAD, OUTER_SPACE
from GaudiKernel import SystemOfUnits as units

# plain/testing geometry service
world = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
}

# material needed for the external world
materials = {
    "OuterSpace": OUTER_SPACE,
    "Pb": LEAD,
}

# Generic options for all detectors
generic_shape = {
    "Type": "Cuboid",
    "xPos": 0.0 * units.m,
    "yPos": 0.0 * units.m,
    "zPos": 5.0 * units.m,
    "xSize": 5.0 * units.m,
    "ySize": 5.0 * units.m,
    "zSize": 1.0 * units.m,
}

generic_sensitive = {
    "Type": "MCCollectorSensDet",
    "RequireEDep": False,
    "OnlyForward": False,
    "PrintStats": True,
}

generic_hit = {
    "Type": "GetMCCollectorHitsAlg",
}

# External detector embedders in mass & parallel geometry
mass_embedder = ExternalDetectorEmbedder("MassEmbedder")
# here embedding of the geometry takes place
GaussinoGeometry().ExternalDetectorEmbedder = "MassEmbedder"
parallel_embedder_1 = ExternalDetectorEmbedder("ParallelEmbedder1")
parallel_embedder_2 = ExternalDetectorEmbedder("ParallelEmbedder2")

mass_embedder.Shapes = {"MassPlane": dict(generic_shape)}
mass_embedder.Shapes["MassPlane"]["MaterialName"] = "Pb"
mass_embedder.Sensitive = {"MassPlane": dict(generic_sensitive)}
mass_embedder.Hit = {"MassPlane": dict(generic_hit)}
mass_embedder.Materials = materials
mass_embedder.World = world

parallel_embedder_1.Shapes = {"ParallelPlane1": dict(generic_shape)}
parallel_embedder_1.Shapes["ParallelPlane1"]["MaterialName"] = "OuterSpace"
parallel_embedder_1.Sensitive = {"ParallelPlane1": dict(generic_sensitive)}
parallel_embedder_1.Hit = {"ParallelPlane1": dict(generic_hit)}

parallel_embedder_2.Shapes = {"ParallelPlane2": dict(generic_shape)}
parallel_embedder_2.Shapes["ParallelPlane2"]["MaterialName"] = "Pb"
parallel_embedder_2.Sensitive = {"ParallelPlane2": dict(generic_sensitive)}
parallel_embedder_2.Hit = {"ParallelPlane2": dict(generic_hit)}

ParallelGeometry().ParallelWorlds = {
    "ParallelWorld1": {
        "ExternalDetectorEmbedder": "ParallelEmbedder1",
        "ExportGDML": {
            "GDMLFileName": "ParallelWorld1.gdml",
            "GDMLFileNameOverwrite": True,
            "GDMLExportSD": True,
            "GDMLExportEnergyCuts": True,
        },
    },
    "ParallelWorld2": {
        "ExternalDetectorEmbedder": "ParallelEmbedder2",
        "ExportGDML": {
            "GDMLFileName": "ParallelWorld2.gdml",
            "GDMLFileNameOverwrite": True,
            "GDMLExportSD": True,
            "GDMLExportEnergyCuts": True,
        },
    },
}
ParallelGeometry().ParallelPhysics = {
    "ParallelWorld1": {
        "LayeredMass": True,
        "ParticlePIDs": [22],
    },
    "ParallelWorld2": {
        "LayeredMass": False,
        "ParticlePIDs": [22],
    },
}

GaussinoGeometry().ExportGDML = {
    "GDMLFileName": "MassWorld.gdml",
    "GDMLFileNameOverwrite": True,
    "GDMLExportSD": True,
    "GDMLExportEnergyCuts": True,
}

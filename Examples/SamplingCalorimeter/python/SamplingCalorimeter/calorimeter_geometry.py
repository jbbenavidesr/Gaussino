###############################################################################
# (c) Copyright 2025 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

"""
Geometry configuration for SamplingCalorimeter example.

This module provides functions to configure a layered sampling calorimeter
geometry based on the Geant4 B4c example. The calorimeter consists of alternating
layers of absorber (lead) and active gap material (argon gas).
"""

from GaudiKernel import SystemOfUnits as units
from Configurables import GaussinoGeometry, ExternalDetectorEmbedder
from ExternalDetector.Materials import OUTER_SPACE


# Predefined detector options
calorimeter_default_options = {
    "n_layers": 10,
    "absorber_thickness": 10 * units.mm,
    "gap_thickness": 5 * units.mm,
    "calor_size_xy": 10 * units.cm,
    "absorber_material": "G4_Pb",
    "gap_material": "G4_Ar",
    "world_material": "OUTER_SPACE",
}


def create_materials(absorber_material, gap_material, world_material="OUTER_SPACE"):
    """
    Create material definitions for the calorimeter.

    Args:
        absorber_material: NIST material name for absorber layers (e.g., "G4_Pb")
        gap_material: NIST material name for gap (active) layers (e.g., "G4_Ar")
        world_material: Material name for world volume (default: "OUTER_SPACE")

    Returns:
        Dictionary of material definitions
    """
    materials = {
        world_material: OUTER_SPACE,
        absorber_material: {"Type": "MaterialFromNIST"},
        gap_material: {"Type": "MaterialFromNIST"},
    }
    return materials


def make_layers_sensitive(
    emb_name, layer_base_name, n_layers, absorber_name=None, gap_name=None
):
    """
    Configure sensitive detectors for absorber and gap layers.

    Args:
        emb_name: Name of the ExternalDetectorEmbedder
        layer_base_name: Base name for layer volumes
        n_layers: Number of calorimeter layers
        absorber_name: Name override for absorber volume (default: layer_base_name + "_Absorber")
        gap_name: Name override for gap volume (default: layer_base_name + "_Gap")

    Returns:
        Dictionary of sensitive detector configurations
    """
    if absorber_name is None:
        absorber_name = f"{layer_base_name}_Absorber"
    if gap_name is None:
        gap_name = f"{layer_base_name}_Gap"

    sensitive = {
        absorber_name: {
            "Type": "CalorimeterCollectorSensDet",
            "NofLayers": n_layers,
        },
        gap_name: {
            "Type": "CalorimeterCollectorSensDet",
            "NofLayers": n_layers,
        },
    }
    return sensitive


def set_sampling_calorimeter(opts, emb_name="SamplingCalorimeter"):
    """
    Configure a complete sampling calorimeter geometry.

    This function sets up a sampling calorimeter with alternating absorber and
    gap layers, following the Geant4 B4c example design.

    Args:
        opts: Dictionary with calorimeter options. Expected keys:
            - n_layers: Number of layer pairs (default: 10)
            - absorber_thickness: Thickness of absorber layer (default: 10 mm)
            - gap_thickness: Thickness of gap layer (default: 5 mm)
            - calor_size_xy: Transverse size of calorimeter (default: 10 cm)
            - absorber_material: Material for absorber (default: "G4_Pb")
            - gap_material: Material for gap (default: "G4_Ar")
            - world_material: World volume material (default: "OUTER_SPACE")
        emb_name: Name for the ExternalDetectorEmbedder instance

    Returns:
        ExternalDetectorEmbedder instance configured with the calorimeter geometry
    """
    # Extract options with defaults
    n_layers = opts.get("n_layers", 10)
    absorber_thickness = opts.get("absorber_thickness", 10 * units.mm)
    gap_thickness = opts.get("gap_thickness", 5 * units.mm)
    calor_size_xy = opts.get("calor_size_xy", 10 * units.cm)
    absorber_material = opts.get("absorber_material", "G4_Pb")
    gap_material = opts.get("gap_material", "G4_Ar")
    world_material = opts.get("world_material", "OUTER_SPACE")

    # Configure GaussinoGeometry
    GaussinoGeometry().ExternalDetectorEmbedder = emb_name
    external = ExternalDetectorEmbedder(emb_name)

    # Calculate dimensions
    layer_thickness = absorber_thickness + gap_thickness
    calor_thickness = layer_thickness * n_layers
    world_size_XY = 1.2 * calor_size_xy
    world_size_Z = 1.2 * calor_thickness

    # World volume
    external.World = {
        "WorldMaterial": world_material,
        "Type": "ExternalWorldCreator",
        "WorldSizeX": world_size_XY / 2,
        "WorldSizeY": world_size_XY / 2,
        "WorldSizeZ": world_size_Z / 2,
    }

    shapes = {}

    # Calorimeter envelope
    calor_name = f"{emb_name}_Calorimeter"
    calor_lvol_name = f"{calor_name}_lVol"

    shapes[calor_name] = {
        "Type": "Cuboid",
        "LogicalVolumeName": calor_lvol_name,
        "MaterialName": world_material,
        "xSize": calor_size_xy,
        "ySize": calor_size_xy,
        "zSize": calor_thickness,
        "xPos": 0.0,
        "yPos": 0.0,
        "zPos": 0.0,
    }

    # Layer template
    layer_base_name = f"{emb_name}_Layer"
    layer_lvol_name = "Layer_lVol"
    layer_config = {
        "Type": "Cuboid",
        "MotherVolumeName": calor_lvol_name,
        "LogicalVolumeName": layer_lvol_name,
        "MaterialName": world_material,
        "xSize": calor_size_xy,
        "ySize": calor_size_xy,
        "zSize": layer_thickness,
        "xPos": 0.0,
        "yPos": 0.0,
    }

    # Create individual layers
    z_position = -calor_thickness / 2.0
    for i in range(n_layers):
        layer_name = f"{layer_base_name}_{i}"
        layer_position = z_position + (layer_thickness / 2.0)
        shapes[layer_name] = {
            **layer_config,
            "zPos": layer_position,
            "pCopyNo": i,
        }
        z_position += layer_thickness

    # Absorber volume (within layer)
    absorber_name = f"{layer_base_name}_Absorber"
    shapes[absorber_name] = {
        "Type": "Cuboid",
        "MotherVolumeName": layer_lvol_name,
        "MaterialName": absorber_material,
        "xSize": calor_size_xy,
        "ySize": calor_size_xy,
        "zSize": absorber_thickness,
        "xPos": 0.0,
        "yPos": 0.0,
        "zPos": -gap_thickness / 2.0,
    }

    # Gap volume (active detector within layer)
    gap_name = f"{layer_base_name}_Gap"
    shapes[gap_name] = {
        "Type": "Cuboid",
        "MotherVolumeName": layer_lvol_name,
        "MaterialName": gap_material,
        "xSize": calor_size_xy,
        "ySize": calor_size_xy,
        "zSize": gap_thickness,
        "xPos": 0.0,
        "yPos": 0.0,
        "zPos": absorber_thickness / 2.0,
    }

    # Configure sensitive detectors
    sensitive = make_layers_sensitive(emb_name, layer_base_name, n_layers)

    # Create materials
    materials = create_materials(absorber_material, gap_material, world_material)

    # Assign to external detector
    external.Shapes = shapes
    external.Sensitive = sensitive
    external.Hit = {}
    external.Moni = {}
    external.Materials = materials

    return external

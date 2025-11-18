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
Geometry configuration for TargetTracker example.

This module provides functions to configure a fixed-target tracking detector
geometry based on the Geant4 B2a example. The detector consists of a cylindrical
target and multiple tracking chambers with increasing radius.
"""

from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Configurables import GaussinoGeometry, ExternalDetectorEmbedder


# Predefined detector options
tracker_default_options = {
    "n_chambers": 5,
    "chamber_spacing": 80 * units.cm,
    "chamber_width": 20 * units.cm,
    "target_length": 5 * units.cm,
    "target_radius": 2.5 * units.cm,
    "world_material": "G4_AIR",
    "target_material": "G4_Pb",
    "tracker_material": "G4_AIR",
    "chamber_material": "G4_Xe",
}


def create_materials(
    world_material, target_material, tracker_material, chamber_material
):
    """
    Create material definitions for the tracker.

    Args:
        world_material: NIST material name for world volume (e.g., "G4_AIR")
        target_material: NIST material name for target (e.g., "G4_Pb")
        tracker_material: NIST material name for tracker volume (e.g., "G4_AIR")
        chamber_material: NIST material name for chambers (e.g., "G4_Xe")

    Returns:
        Dictionary of material definitions
    """
    materials = {
        world_material: {"Type": "MaterialFromNIST"},
        target_material: {"Type": "MaterialFromNIST"},
        tracker_material: {"Type": "MaterialFromNIST"},
        chamber_material: {"Type": "MaterialFromNIST"},
    }
    return materials


def make_chambers_sensitive(emb_name, n_chambers):
    """
    Configure sensitive detectors for tracking chambers.

    Args:
        emb_name: Name of the ExternalDetectorEmbedder
        n_chambers: Number of tracking chambers

    Returns:
        Dictionary of sensitive detector configurations
    """
    sensitive = {}
    for chamber_no in range(n_chambers):
        chamber_name = f"{emb_name}_Chamber_{chamber_no}"
        sensitive[chamber_name] = {
            "Type": "SimpleCollectorSensDet",
        }
    return sensitive


def set_target_tracker(opts, emb_name="TargetTracker"):
    """
    Configure a complete target-tracker geometry.

    This function sets up a fixed-target tracker with a cylindrical target
    and multiple tracking chambers with increasing radius, following the
    Geant4 B2a example design.

    Args:
        opts: Dictionary with tracker options. Expected keys:
            - n_chambers: Number of tracking chambers (default: 5)
            - chamber_spacing: Distance between chambers (default: 80 cm)
            - chamber_width: Width of each chamber (default: 20 cm)
            - target_length: Length of target cylinder (default: 5 cm)
            - target_radius: Radius of target cylinder (default: 2.5 cm)
            - world_material: World volume material (default: "G4_AIR")
            - target_material: Target material (default: "G4_Pb")
            - tracker_material: Tracker volume material (default: "G4_AIR")
            - chamber_material: Chamber material (default: "G4_Pb")
        emb_name: Name for the ExternalDetectorEmbedder instance

    Returns:
        ExternalDetectorEmbedder instance configured with the tracker geometry
    """
    # Extract options with defaults
    n_chambers = opts.get("n_chambers", 5)
    chamber_spacing = opts.get("chamber_spacing", 80 * units.cm)
    chamber_width = opts.get("chamber_width", 20 * units.cm)
    target_length = opts.get("target_length", 5 * units.cm)
    target_radius = opts.get("target_radius", 2.5 * units.cm)
    world_material = opts.get("world_material", "G4_AIR")
    target_material = opts.get("target_material", "G4_Pb")
    tracker_material = opts.get("tracker_material", "G4_AIR")
    chamber_material = opts.get("chamber_material", "G4_Xe")

    # Configure GaussinoGeometry
    GaussinoGeometry().ExternalDetectorEmbedder = emb_name
    external = ExternalDetectorEmbedder(emb_name)

    # Calculate dimensions
    tracker_length = (n_chambers + 1) * chamber_spacing
    world_length = 1.2 * (2 * target_length + tracker_length)

    # World volume
    external.World = {
        "WorldMaterial": world_material,
        "Type": "ExternalWorldCreator",
        "WorldSizeX": world_length / 2,
        "WorldSizeY": world_length / 2,
        "WorldSizeZ": world_length / 2,
    }

    shapes = {}

    # Target (cylindrical)
    target_name = f"{emb_name}_Target"
    target_z_pos = -(target_length + tracker_length) * 0.5

    shapes[target_name] = {
        "Type": "Tube",
        "MaterialName": target_material,
        "RMin": 0,
        "RMax": target_radius,
        "Dz": target_length / 2.0,
        "SPhi": 0,
        "DPhi": 2 * constants.pi * units.radian,
        "zPos": target_z_pos,
    }

    # Tracker volume (cylindrical)
    tracker_name = f"{emb_name}_Tracker"
    tracker_lvol_name = f"{tracker_name}_lVol"

    shapes[tracker_name] = {
        "Type": "Tube",
        "LogicalVolumeName": tracker_lvol_name,
        "MaterialName": tracker_material,
        "RMin": 0,
        "RMax": tracker_length / 2.0,
        "Dz": tracker_length / 2.0,
        "SPhi": 0,
        "DPhi": 2 * constants.pi * units.radian,
    }

    # Chambers (with increasing radius)
    first_position = -0.5 * tracker_length + chamber_spacing
    first_length = tracker_length / 10
    last_length = tracker_length

    half_width = 0.5 * chamber_width
    rmax_first = 0.5 * first_length
    rmax_incr = 0.5 * (last_length - first_length) / (n_chambers - 1)

    for chamber_no in range(n_chambers):
        chamber_name = f"{emb_name}_Chamber_{chamber_no}"
        z_position = first_position + chamber_no * chamber_spacing
        rmax = rmax_first + chamber_no * rmax_incr

        shapes[chamber_name] = {
            "Type": "Tube",
            "MaterialName": chamber_material,
            "RMin": 0,
            "RMax": rmax,
            "Dz": half_width,
            "SPhi": 0,
            "DPhi": 2 * constants.pi * units.radian,
            "zPos": z_position,
            "MotherVolumeName": tracker_lvol_name,
        }

    # Configure sensitive detectors
    sensitive = make_chambers_sensitive(emb_name, n_chambers)

    # Create materials
    materials = create_materials(
        world_material, target_material, tracker_material, chamber_material
    )

    # Assign to external detector
    external.Shapes = shapes
    external.Sensitive = sensitive
    external.Hit = {}
    external.Moni = {}
    external.Materials = materials

    return external

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

import numpy as np
from ExternalDetector.Materials import OUTER_SPACE
from GaudiKernel import PhysicalConstants as constants
from GaudiKernel import SystemOfUnits as units

detector_general_options = {
    "size_of_rho_cells": 2.325 * units.mm,
    "size_of_z_cells": 3.4 * units.mm,
    "nb_of_rho_cells": 18,
    "nb_of_phi_cells": 50,
    "nb_of_z_cells": 45,
    "collector_width": 1e-10 * units.m,
}

cylindrical_detector_general_options = {
    "detector_inner_radius": 80 * units.cm,
    "detector_length": 2 * units.m,
    "nb_of_layers": 90,
    **detector_general_options,
}

planar_detector_general_options = {
    "detector_x_size": 200 * units.cm,
    "detector_y_size": 200 * units.cm,
    # z size to be deduced from the number of layers
    "detector_x_pos": 0 * units.m,  # centered
    "detector_y_pos": 0 * units.m,  # centered
    "detector_z_pos": 80 * units.cm,  # beginning of the detector
    "nb_of_layers": 90,
    **detector_general_options,
}

detector_SiW_layers = {
    "absorbers": [
        {
            "Material": "G4_W",
            "Thickness": 1.4 * units.mm,
            "Sensitive": False,
        },
        {
            "Material": "G4_Si",
            "Thickness": 0.3 * units.mm,
            "Sensitive": True,
        },
    ],
}

cylindrical_detector_SiW_options = {
    **cylindrical_detector_general_options,
    **detector_SiW_layers,
}

planar_detector_SiW_options = {
    **planar_detector_general_options,
    **detector_SiW_layers,
}


def create_materials(absorbers: list) -> dict:
    materials = {
        "G4_AIR": {"Type": "MaterialFromNIST"},
        "OUTER_SPACE": OUTER_SPACE,
    }

    for absorber in absorbers:
        materials[absorber["Material"]] = {"Type": "MaterialFromNIST"}

    return materials


def make_collector_sensitive(
    sens_det_names: list = ["Collector"],
    collector_hits_location: str = "MC/CaloChallenge/Collector/Hits",
    use_custom_trigger_plane: bool = False,
    trigger_plane_z: float = 0.0,
    trigger_plane_tilt: float = 0.0,
    trigger_plane_y_shift: float = 0.0,
    use_total_momentum_cuts: bool = False,
    min_total_momentum: float = -np.inf,
    max_total_momentum: float = np.inf,
    use_momentum_x_cuts: bool = False,
    min_momentum_x: float = -np.inf,
    max_momentum_x: float = np.inf,
    use_momentum_y_cuts: bool = False,
    min_momentum_y: float = -np.inf,
    max_momentum_y: float = np.inf,
    use_momentum_z_cuts: bool = False,
    min_momentum_z: float = -np.inf,
    max_momentum_z: float = np.inf,
    **kwargs,
) -> dict:
    from Configurables import ApplicationMgr, Gsino__CaloChallenge__GetCollectorHitsAlg

    # External Detector adds one sensitive detector per volume
    # so we have to modify a bit the standard way of doing this
    # (not the cleanest one, but this is what it is)
    first_sens_det = sens_det_names[0]
    extra_vols = [
        f"{sens_det}LVol" for sens_det in sens_det_names if sens_det != first_sens_det
    ]

    sensitives = {
        first_sens_det: {
            "Type": "Gsino__CaloChallenge__CollectorSensDetFactory",
            "ExtraVolumesToSensDet": extra_vols,
            "UseCustomTriggerPlane": use_custom_trigger_plane,
            "TriggerPlaneZ": trigger_plane_z,
            "TriggerPlaneTilt": trigger_plane_tilt,
            "TriggerPlaneYShift": trigger_plane_y_shift,
            "UseTotalMomentumCuts": use_total_momentum_cuts,
            "MinTotalMomentum": min_total_momentum,
            "MaxTotalMomentum": max_total_momentum,
            "UseMomentumXCuts": use_momentum_x_cuts,
            "MinMomentumX": min_momentum_x,
            "MaxMomentumX": max_momentum_x,
            "UseMomentumYCuts": use_momentum_y_cuts,
            "MinMomentumY": min_momentum_y,
            "MaxMomentumY": max_momentum_y,
            "UseMomentumZCuts": use_momentum_z_cuts,
            "MinMomentumZ": min_momentum_z,
            "MaxMomentumZ": max_momentum_z,
        }
    }
    ApplicationMgr().TopAlg.append(
        Gsino__CaloChallenge__GetCollectorHitsAlg(
            "CaloChallengeGetCollectorHitsAlg",
            G4HitsCollectionNames=[f"{first_sens_det}SDet/Hits"],
            OutputHitsLocation=collector_hits_location,
            **kwargs,
        )
    )
    return sensitives


def make_layers_sensitive(
    opts: dict,
    sens_det_names: list,
    detailed_hits_location: str = "MC/CaloChallenge/Calo/Hits",
    ignore_unmatched_hits: bool = False,
    **kwargs,
) -> dict:
    from Configurables import ApplicationMgr, Gsino__CaloChallenge__GetCaloHitsAlg

    # External Detector adds one sensitive detector per volume
    # so we have to modify a bit the standard way of doing this
    # (not the cleanest one, but this is what it is)
    first_sens_det = sens_det_names[0]
    extra_vols = [
        f"{sens_det}LVol" for sens_det in sens_det_names if sens_det != first_sens_det
    ]
    sensitives = {}
    sensitives[first_sens_det] = {
        "Type": "Gsino__CaloChallenge__CaloSensDetFactory",
        "ExtraVolumesToSensDet": extra_vols,
        "SizeOfRhoCells": opts["size_of_rho_cells"],
        "SizeOfZCells": opts["size_of_z_cells"],
        "NbOfRhoCells": opts["nb_of_rho_cells"],
        "NbOfPhiCells": opts["nb_of_phi_cells"],
        "NbOfZCells": opts["nb_of_z_cells"],
        "IgnoreUnmatchedHits": ignore_unmatched_hits,
    }
    ApplicationMgr().TopAlg.append(
        Gsino__CaloChallenge__GetCaloHitsAlg(
            "CaloChallengeGetCaloHitsAlg",
            G4HitsCollectionNames=[f"{first_sens_det}SDet/Hits"],
            OutputHitsLocation=detailed_hits_location,
            **kwargs,
        )
    )
    return sensitives


def set_cylindrical_calo(
    opts: dict,
    external_detector_name: str = "CaloChallengeDetector",
    detector_name: str = "Detector",
):
    from Configurables import ExternalDetectorEmbedder, GaussinoGeometry

    GaussinoGeometry().ExternalDetectorEmbedder = external_detector_name
    # deduced properties as in CaloChallenge configuration
    layer_thickness = sum([absorber["Thickness"] for absorber in opts["absorbers"]])
    detector_outer_radius = (
        opts["detector_inner_radius"] + opts["nb_of_layers"] * layer_thickness
    )

    external = ExternalDetectorEmbedder(external_detector_name)

    external.World = {
        "WorldMaterial": "OUTER_SPACE",
        "Type": "ExternalWorldCreator",
        "WorldSizeX": detector_outer_radius,
        "WorldSizeY": detector_outer_radius,
        "WorldSizeZ": opts["detector_length"],
    }

    external.Materials = create_materials(opts["absorbers"])

    external.Shapes = {
        detector_name: {
            "Type": "Tube",
            "MaterialName": "OUTER_SPACE",
            "RMin": opts["detector_inner_radius"],
            "RMax": detector_outer_radius,
            "Dz": opts["detector_length"] / 2.0,
            "SPhi": 0,
            "DPhi": 2 * constants.pi * units.radian,
        },
        "Collector": {
            "Type": "Tube",
            "MaterialName": "OUTER_SPACE",
            "RMin": opts["detector_inner_radius"] - opts["collector_width"],
            "RMax": opts["detector_inner_radius"],
            "Dz": opts["detector_length"],
            "SPhi": 0,
            "DPhi": 2 * constants.pi * units.radian,
        },
    }

    external.Sensitive = make_collector_sensitive()

    sens_det_names = []
    inner_radius = opts["detector_inner_radius"]
    for l_id in range(opts["nb_of_layers"]):
        for absorber in opts["absorbers"]:
            name = f"Layer{l_id}_{absorber['Material']}"
            external.Shapes[name] = {
                "Type": "Tube",
                "MotherVolumeName": f"{detector_name}LVol",
                "MaterialName": absorber["Material"],
                "RMin": inner_radius,
                "RMax": inner_radius + absorber["Thickness"],
                "Dz": opts["detector_length"] / 2.0,
                "SPhi": 0,
                "DPhi": 2 * constants.pi * units.radian,
            }
            if absorber["Sensitive"]:
                sens_det_names.append(name)
            inner_radius += absorber["Thickness"]

    external.Sensitive.update(
        make_layers_sensitive(opts=opts, sens_det_names=sens_det_names)
    )


def set_planar_calo(
    opts: dict,
    external_detector_name: str = "CaloChallengeDetector",
    detector_name: str = "Detector",
):
    from Configurables import ExternalDetectorEmbedder, GaussinoGeometry

    GaussinoGeometry().ExternalDetectorEmbedder = external_detector_name
    layer_thickness = sum([absorber["Thickness"] for absorber in opts["absorbers"]])

    detector_z_size = opts["nb_of_layers"] * layer_thickness

    external = ExternalDetectorEmbedder(external_detector_name)

    external.World = {
        "WorldMaterial": "OUTER_SPACE",
        "Type": "ExternalWorldCreator",
        "WorldSizeX": opts["detector_x_size"] / 2.0,
        "WorldSizeY": opts["detector_y_size"] / 2.0,
        "WorldSizeZ": (opts["detector_z_pos"] + detector_z_size),
    }

    external.Materials = create_materials(opts["absorbers"])

    external.Shapes = {
        detector_name: {
            "Type": "Cuboid",
            "MaterialName": "OUTER_SPACE",
            "xPos": opts["detector_x_pos"],
            "yPos": opts["detector_y_pos"],
            "zPos": opts["detector_z_pos"] + detector_z_size / 2.0,
            "xSize": opts["detector_x_size"],
            "ySize": opts["detector_y_size"],
            "zSize": detector_z_size,
        },
        "Collector": {
            "Type": "Cuboid",
            "MaterialName": "OUTER_SPACE",
            "xPos": opts["detector_x_pos"],
            "yPos": opts["detector_y_pos"],
            "zPos": opts["detector_z_pos"] - opts["collector_width"] / 2.0,
            "xSize": opts["detector_x_size"],
            "ySize": opts["detector_y_size"],
            "zSize": opts["collector_width"],
        },
    }

    external.Sensitive = make_collector_sensitive()

    sens_det_names = []
    zPos = -detector_z_size / 2.0
    for l_id in range(opts["nb_of_layers"]):
        for absorber in opts["absorbers"]:
            name = f"Layer{l_id}_{absorber['Material']}"
            external.Shapes[name] = {
                "Type": "Cuboid",
                "MotherVolumeName": f"{detector_name}LVol",
                "MaterialName": absorber["Material"],
                # with respect to the center of the mother volume
                "xPos": 0.0,
                "yPos": 0.0,
                "zPos": zPos + absorber["Thickness"] / 2.0,
                "xSize": opts["detector_x_size"],
                "ySize": opts["detector_y_size"],
                "zSize": absorber["Thickness"],
            }
            if absorber["Sensitive"]:
                sens_det_names.append(name)
            zPos += absorber["Thickness"]

    external.Sensitive.update(
        make_layers_sensitive(opts=opts, sens_det_names=sens_det_names)
    )

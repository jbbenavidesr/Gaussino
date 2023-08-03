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

from Configurables import ApplicationMgr, ExternalDetectorEmbedder
from Configurables import Gaussino__G4Par04__GetCaloHitsAlg as GetCaloHitsAlg
from Configurables import Gaussino__G4Par04__GetCollectorHitsAlg as GetCollectorHitsAlg
from ExternalDetector.Materials import OUTER_SPACE
from GaudiKernel import PhysicalConstants as constants
from GaudiKernel import SystemOfUnits as units


def set_cylindrical_calo(
    opts: dict,
    external_detector_name: str,
    detector_name: str = "Detector",
    collector_width: float = 1e-5 * units.mm,
):
    # deduced properties as in Par04 onfiguration
    layer_thickness = sum([absorber["Thickness"] for absorber in opts["absorbers"]])
    detector_outer_radius = (
        opts["detector_inner_radius"] + opts["nb_of_layers"] * layer_thickness
    )
    world_size_xy = detector_outer_radius * 4.0
    world_size_Z = opts["detector_length"] * 2

    external = ExternalDetectorEmbedder(external_detector_name)

    external.World = {
        "WorldMaterial": "OUTER_SPACE",
        "Type": "ExternalWorldCreator",
        "WorldSizeX": world_size_xy / 2.0,
        "WorldSizeY": world_size_xy / 2.0,
        "WorldSizeZ": world_size_Z / 2.0,
    }

    external.Materials = {
        "G4_AIR": {"Type": "MaterialFromNIST"},
        "OUTER_SPACE": OUTER_SPACE,
    }

    for absorber in opts["absorbers"]:
        external.Materials[absorber["Material"]] = {"Type": "MaterialFromNIST"}

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
            "RMin": opts["detector_inner_radius"] - collector_width,
            "RMax": opts["detector_inner_radius"],
            "Dz": world_size_Z / 2.0,
            "SPhi": 0,
            "DPhi": 2 * constants.pi * units.radian,
        },
    }

    external.Sensitive = {
        "Collector": {
            "Type": "Gaussino__G4Par04__CollectorSensDetFactory",
        }
    }

    ApplicationMgr().TopAlg.append(
        GetCollectorHitsAlg(
            "Par04GetCollectorHitsAlg",
            G4HitsCollectionNames=["CollectorSDet/Hits"],
            OutputHitsLocation="MC/Par04/Collector/Hits",
        )
    )

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

    # External Detector adds one sensitive detector per volume
    # so we have to modify a bit the standard way of doing this
    # (not the cleanest one, but this is what it is)
    first_sens_det = sens_det_names[0]
    extra_vols = [
        f"{sens_det}LVol" for sens_det in sens_det_names if sens_det != first_sens_det
    ]
    external.Sensitive[first_sens_det] = {
        "Type": "Gaussino__G4Par04__CaloSensDetFactory",
        "ExtraVolumesToSensDet": extra_vols,
        "SizeOfRhoCells": opts["size_of_rho_cells"],
        "SizeOfZCells": opts["size_of_z_cells"],
        "NbOfRhoCells": opts["nb_of_rho_cells"],
        "NbOfPhiCells": opts["nb_of_phi_cells"],
        "NbOfZCells": opts["nb_of_z_cells"],
    }
    ApplicationMgr().TopAlg.append(
        GetCaloHitsAlg(
            "Par04GetCaloHitsAlg",
            G4HitsCollectionNames=[f"{first_sens_det}SDet/Hits"],
            OutputHitsLocation="MC/Par04/Calo/Hits",
        )
    )

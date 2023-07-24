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
    GaussinoSimulation,
    CustomSimulation,
)


def set_mesh_model(
    pdg_codes: list = [22, 11, -11],
    detector_name: str = "Collector",
):
    GaussinoSimulation().CustomSimulation = "MeshModelSimulation"
    customsim = CustomSimulation("MeshModelSimulation")
    customsim.Model = {
        "MeshModel": {
            "Type": "Gaussino__G4Par04__MeshModelFactory",
        }
    }
    customsim.Region = {
        "MeshModel": {
            "SensitiveDetectorName": f"{detector_name}SDet",
        }
    }
    customsim.Physics = {
        "ParticlePIDs": pdg_codes,
    }

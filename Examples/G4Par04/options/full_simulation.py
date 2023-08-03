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

import math

from Configurables import (
    Gaussino,
    GaussinoGeneration,
    GaussinoGeometry,
    GaussinoSimulation,
)
from Gaudi.Configuration import appendPostConfigAction
from GaudiKernel import PhysicalConstants as constants
from GaudiKernel import SystemOfUnits as units
from Par04.generation import (
    set_particle_gun_fixed_momentum,
    set_particle_gun_momentum_range,
)
from Par04.geometry import set_cylindrical_calo
from Par04.monitoring import set_monitoring

deterministic = True
dataset_name = "SiW-FixedMomentum_10GeV_angle90"
opts = {
    "detector_inner_radius": 80 * units.cm,
    "detector_length": 2 * units.m,
    "nb_of_layers": 90,
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
    "size_of_rho_cells": 2.325 * units.mm,
    "size_of_z_cells": 3.4 * units.mm,
    "nb_of_rho_cells": 18,
    "nb_of_phi_cells": 50,
    "nb_of_z_cells": 45,
    "inference": False,
}


# Generation
GaussinoGeneration().ParticleGun = True
if deterministic:
    set_particle_gun_fixed_momentum(
        pdg_codes=[22],
        momentum_x=0.0 * units.GeV,
        momentum_y=10.0 * units.GeV,
        momentum_z=0.0 * units.GeV,
    )
else:
    set_particle_gun_momentum_range(
        pdg_codes=[22],
        min_momentum=0.5 * units.GeV,
        max_momentum=50.0 * units.GeV,
        theta_min=constants.pi / 2.0
        - math.atan(opts["detector_length"] / 2.0 / opts["detector_inner_radius"]),
        theta_max=constants.pi / 2.0
        + math.atan(opts["detector_length"] / 2.0 / opts["detector_inner_radius"]),
    )

# Geometry
external_detector_name = "Par04Detector"
collector_width = 1e-10 * units.m
GaussinoGeometry().ExternalDetectorEmbedder = "Par04Detector"
sens_det_names = set_cylindrical_calo(
    opts=opts,
    external_detector_name=external_detector_name,
    detector_name="Detector",
    collector_width=collector_width,
)

# Simulation
# -> physics lists and production cuts as in FTFP_BERT
GaussinoSimulation(
    PhysicsConstructors=[
        "GiGaMT_G4EmStandardPhysics",
        "GiGaMT_G4EmExtraPhysics",
        "GiGaMT_G4DecayPhysics",
        "GiGaMT_G4HadronElasticPhysics",
        "GiGaMT_G4HadronPhysicsFTFP_BERT",
        "GiGaMT_G4StoppingPhysics",
        "GiGaMT_G4IonPhysics",
        "GiGaMT_G4NeutronTrackingCut",
    ],
    CutForElectron=700 * units.micrometer,
    CutForPositron=700 * units.micrometer,
    CutForGamma=700 * units.micrometer,
    DumpCutsTable=True,
)

# Monitoring
set_monitoring(
    opts=opts,
    max_energy_hist=51.0 * units.GeV,
    ntuple_name=f"{Gaussino()._get_output_name()}-TrainingData.root",
    training_data=not opts["inference"],
)


# setting up the TruthFlaggingTrackAction options
def updateRhoMax():
    from Configurables import TruthFlaggingTrackAction

    trth = TruthFlaggingTrackAction(
        "GiGaMT.GiGaActionInitializer.TruthFlaggingTrackAction"
    )
    trth.StoreUpToRho = True
    trth.StoreUpToZ = False
    trth.RhomaxForStoring = opts["detector_inner_radius"] - collector_width
    # optimization for the training dataset
    # make sure you only have one collector hit per particle
    trth.StorePrimaries = True
    trth.StoreByOwnEnergy = True
    trth.OwnEnergyThreshold = 0.1 * units.GeV
    trth.StoreAll = False
    trth.StoreForcedDecays = False
    trth.StoreByOwnProcess = False
    trth.StoreByOwnType = False
    trth.StoreByChildProcess = False
    trth.StoreByChildEnergy = False
    trth.StoreByChildType = False


appendPostConfigAction(updateRhoMax)

# Visualization
# # TODO: not yet on master
#
# from Configurables import GaussinoVisualization
# GaussinoVisualization(
#     Framework=["Geant4"],
#     Driver="OpenGLStoredX",
#     DrawGeometry=True,
#     DrawTrajectories=True,
#     DrawG4Hits=True,
#     StoreTrajectories="All",
#     CameraPhi=10,
#     CameraTheta=30,
#     Zoom=1,
#     GeometryStyle="surface",
# )

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
from Gaudi.Configuration import appendPostConfigAction
from GaudiKernel import SystemOfUnits as units


def set_full_simulation(
    opts: dict,
    calo_type: str = "cylindrical",
    own_energy_threshold: float = 0.1 * units.GeV,
):
    from Configurables import GaussinoSimulation

    # -> physics lists ans production cuts as in FTFP_BERT
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

    # setting up the TruthFlaggingTrackAction options
    def updateStoreMax():
        from Configurables import TruthFlaggingTrackAction

        trth = TruthFlaggingTrackAction(
            "GiGaMT.GiGaActionInitializer.TruthFlaggingTrackAction"
        )
        if calo_type == "cylindrical":
            trth.StoreUpToRho = True
            trth.StoreUpToZ = False
            trth.RhomaxForStoring = (
                opts["detector_inner_radius"] - opts["collector_width"]
            )
        elif calo_type == "planar":
            trth.StoreUpToRho = False
            trth.StoreUpToZ = True
            trth.ZmaxForStoring = opts["detector_z_pos"]
        else:
            raise NotImplementedError(f"Not supported calorimeter type: '{calo_type}'.")
        # optimization for the training dataset
        # make sure you only have one collector hit per particle
        trth.StorePrimaries = True
        trth.StoreByOwnEnergy = True
        trth.OwnEnergyThreshold = own_energy_threshold
        trth.StoreAll = False
        trth.StoreForcedDecays = False
        trth.StoreByOwnProcess = False
        trth.StoreByOwnType = False
        trth.StoreByChildProcess = False
        trth.StoreByChildEnergy = False
        trth.StoreByChildType = False

    appendPostConfigAction(updateStoreMax)


def set_fast_simulation(
    opts: dict,
    pdg_codes: list = [22, 11, -11],
    detector_name: str = "Collector",
    parallel_world_name: str = "",
    ml_type: str = "torch",
    model_path: str = "",
    model_name: str = "",
    model_extra_options: dict = {},
    auto_input_types: bool = True,
    auto_output_types: bool = True,
    intraop_threads: int = 0,
    interop_threads: int = 0,
    latent_vector_size: int = 10,
    max_energy: float = 1024.0 * units.GeV,
    max_angle: float = 90.0 * units.degree,
    collector_hits_location: str = "MC/CaloChallenge/Collector/Hits",
    fast_hits_location: str = "MC/CaloChallenge/Calo/FastHits",
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
):
    import Configurables
    from Configurables import (
        ApplicationMgr,
        CustomSimulation,
        Gaussino,
        GaussinoSimulation,
        Gsino__CaloChallenge__GetMLCaloHitsAlg,
    )

    if parallel_world_name:
        from Configurables import ParallelGeometry

        ParallelGeometry().ParallelWorlds[parallel_world_name][
            "CustomSimulation"
        ] = "ImmediateDepositSimulation"
    else:
        GaussinoSimulation().CustomSimulation = "ImmediateDepositSimulation"
    customsim = CustomSimulation("ImmediateDepositSimulation")
    customsim.Model = {
        "ImmediateDepositModel": {
            "Type": "ImmediateDepositWithConditionsModel",
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
    customsim.Region = {
        "ImmediateDepositModel": {
            "SensitiveDetectorName": f"{detector_name}SDet",
        }
    }
    customsim.Physics = {
        "ParticlePIDs": pdg_codes,
    }
    appMgr = ApplicationMgr()
    svc_name = None
    if ml_type == "torch":
        svc_name = "Gsino::ML::Torch::ModelServerSvc"
    elif ml_type == "onnx":
        svc_name = "Gsino::ML::ONNX::ModelServerSvc"
    else:
        raise NotImplementedError(f"Not supported ML interface: '{ml_type}'.")

    ml_opts = {
        "Backend": ml_type,
        "ModelPath": model_path,
        "ModelName": model_name,
        "AutoInputType": auto_input_types,
        "AutoOutputTypes": auto_output_types,
        "IntraOpThreads": intraop_threads,
        "InterOpThreads": interop_threads,
    }

    Gaussino(MLOptions=ml_opts)

    alg_conf = Gsino__CaloChallenge__GetMLCaloHitsAlg(
        ModelName=model_name,
        CollectorHitsLocation=collector_hits_location,
        OutputHitsLocation=fast_hits_location,
        MeshNumber=(
            opts["nb_of_rho_cells"],
            opts["nb_of_phi_cells"],
            opts["nb_of_z_cells"],
        ),
        MeshSize=(
            opts["size_of_rho_cells"],
            1,
            opts["size_of_z_cells"],
        ),
    )

    model_tool_conf = getattr(Configurables, model_name)
    alg_conf.addTool(model_tool_conf, name=model_name)
    model_tool = getattr(alg_conf, model_name)
    model_tool.ModelServerSvc = svc_name
    model_tool.MaxEnergy = max_energy
    if model_name.startswith("VAEWithProfiles"):
        model_tool.MaxTheta = max_angle
    else:
        model_tool.MaxAngle = max_angle
    model_tool.LatentVectorSize = latent_vector_size
    for k, v in model_extra_options.items():
        setattr(model_tool, k, v)

    appMgr.TopAlg.append(alg_conf)

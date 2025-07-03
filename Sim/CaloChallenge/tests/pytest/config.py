###############################################################################
# (c) Copyright 2024 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

import json
import os
import re
from typing import Optional

import pytest
from Gaussino.pytest.helpers import run_gaudi


def config():
    from CaloChallenge.cc_generation import set_particle_gun
    from CaloChallenge.cc_monitoring import set_monitoring
    from CaloChallenge.cc_simulation import set_fast_simulation, set_full_simulation
    from Configurables import Gaussino
    from GaudiKernel import SystemOfUnits as units

    opts = json.loads(os.environ["TEST_OPTS"])

    detector_options = {}
    if opts["calo_type"] == "cylindrical":
        from CaloChallenge.cc_geometry import (
            cylindrical_detector_SiW_options as options,
        )

        detector_options = options
    elif opts["calo_type"] == "planar":
        from CaloChallenge.cc_geometry import planar_detector_SiW_options as options

        detector_options = options
    else:
        raise NotImplementedError(
            f"Not supported calorimeter type: '{opts['calo_type']}'."
        )

    # General
    dataset_name = f"{opts['calo_type'].capitalize()}_SiW_FixedMomentum_10GeV_gamma_angle_DetailedSimulation"
    if opts["backend"] == "torch":
        dataset_name = dataset_name.replace(
            "DetailedSimulation", f"FastSimulation{opts['model_name']}"
        )
    elif opts["backend"] == "onnx":
        dataset_name = dataset_name.replace(
            "DetailedSimulation", f"FastSimulation{opts['model_name']}"
        )

    Gaussino(
        DatasetName=dataset_name,
        EvtMax=100,
        RunNumber=1,
        FirstEventNumber=1,
        ConvertEDM=True,
        ThreadPoolSize=opts["nthreads"],
        EventSlots=opts["nthreads"],
        TimingSkipAtStart=opts["nthreads"],
    )

    # Generation
    momenta = {}
    if opts["calo_type"] == "cylindrical":
        momenta["momentum_y"] = 10 * units.GeV
        dataset_name = dataset_name.replace("angle", "angle90")
    elif opts["calo_type"] == "planar":
        momenta["momentum_z"] = 10 * units.GeV
        dataset_name = dataset_name.replace("angle", "angle0")
    set_particle_gun(
        opts=detector_options,
        calo_type=opts["calo_type"],
        type="FixedMomentum",
        pdg_codes=[22],
        **momenta,
    )

    # Geometry
    if opts["calo_type"] == "cylindrical":
        from CaloChallenge.cc_geometry import set_cylindrical_calo

        set_cylindrical_calo(opts=detector_options)
    elif opts["calo_type"] == "planar":
        from CaloChallenge.cc_geometry import set_planar_calo

        set_planar_calo(opts=detector_options)

    # Detailed Simulation
    set_full_simulation(opts=detector_options, calo_type=opts["calo_type"])

    # Fast simulation
    fast_simulation_on = opts["backend"] != "geant4"
    if fast_simulation_on:
        set_fast_simulation(
            opts=detector_options,
            pdg_codes=[22, 11, -11],
            detector_name="Collector",
            ml_type=opts["backend"],
            model_path=opts["model_path"],
            model_name=opts["model_name"],
            latent_vector_size=10,
            max_energy=1024.0 * units.GeV,
            max_angle=90.0 * units.degree,
            interop_threads=opts["nthreads"],
            intraop_threads=1,
        )

    # Monitoring
    set_monitoring(
        opts=detector_options,
        max_energy_hist=51.0 * units.GeV,
        training_data=True,
        fast_simulation=fast_simulation_on,
    )


def run_test(
    backend: str,
    calo_type: str,
    nthreads: int,
    file_name: Optional[str] = None,
    model_name: Optional[str] = None,
):
    if backend == "torch" and os.getenv("GSINO_USE_TORCH") != "ON":
        pytest.skip("Torch backend is not enabled!")
    if backend == "onnx" and os.getenv("GSINO_USE_ONNXRUNTIME") != "ON":
        pytest.skip("ONNXRuntime backend is not enabled!")
    env = os.environ.copy()
    opts = {
        "backend": backend,
        "calo_type": calo_type,
        "nthreads": nthreads,
    }
    if backend != "geant4":
        opts["model_name"] = model_name
        opts["model_path"] = os.path.join(
            os.path.dirname(__file__), "../models", file_name
        )
    env["TEST_OPTS"] = json.dumps(opts)
    ex = run_gaudi(f"{__file__}:config", env=env)

    if backend == "geant4":
        # do not check the hits no. for Geant4
        # (information that the right counter is used is enough)
        patterns = [r"\"\#FullSimHits\"\s+\|\s+100\s+\|"]
    else:
        # here we are using test models that write a fixed number of hits
        # with a fixed energy, there should be:
        # 100 events x 18 (rho) x 50 (phi) x 45 (z) = 4050000 hits
        # each with energy 1.0 MeV
        patterns = [
            r"\"\#FastSimHits\"\s+\|\s+100\s+\|\s+4050000\s+\|\s+40500.\s+\|\s+0.0000",
            r"\"Energy Deposit \[MeV\]\"\s+\|\s+100\s+\|\s+4050000\s+\|\s+40500.\s+\|\s+0.0000",
        ]
    for pattern in patterns:
        if not re.findall(pattern, ex.stdout):
            raise AssertionError(
                f"Could not find expected output in stdout: '{pattern}'"
            )

    assert ex.returncode == 0

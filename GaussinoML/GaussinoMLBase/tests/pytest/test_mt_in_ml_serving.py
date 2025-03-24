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
import json
import multiprocessing
import os
import re

import pytest
from Gaussino.pytest.helpers import reset_configurables, run_gaudi


def config():
    import Configurables

    opts = json.loads(os.environ["TEST_OPTS"])

    from Configurables import ApplicationMgr, Gaussino

    app = ApplicationMgr()

    namespace = "Torch" if opts["backend"] == "torch" else "ONNX"
    alg = getattr(Configurables, f"Gsino__ML__{namespace}__TestMMThroughputAlg")

    file_suffix = ".pt" if opts["backend"] == "torch" else ".onnx"
    package = "GAUSSINOTORCHROOT" if opts["backend"] == "torch" else "GAUSSINOONNXROOT"
    model_path = f"${package}/models/Multiply2TensorsModel{file_suffix}"

    ml_opts = {
        "Backend": opts["backend"],
        "ModelPath": model_path,
        "ModelName": "Multiply2TensorsModel",
        "AutoInputType": True,
        "AutoOutputTypes": True,
        "IntraOpThreads": opts["nthreads"],
        "InterOpThreads": opts["nthreads"],
    }

    Gaussino(
        EvtMax=100,
        Phases=[],
        EnableHive=True,
        ThreadPoolSize=opts["nthreads"],
        EventSlots=opts["nthreads"],
        MLOptions=ml_opts,
    )

    app.TopAlg.append(
        alg(
            DimX=1024,
            DimY=1024,
            ModelServingSvc=f"Gsino::ML::{namespace}::ModelServerSvc",
        )
    )


@reset_configurables
@pytest.mark.parametrize(
    "backend",
    [
        "torch",
        "onnx",
        # any other, tensorflow, etc.
    ],
)
def test_mt_in_ml_serving(backend):
    if backend == "torch" and os.getenv("GSINO_USE_TORCH") != "ON":
        pytest.skip("Torch backend is not enabled!")
    if backend == "onnx" and os.getenv("GSINO_USE_ONNXRUNTIME") != "ON":
        pytest.skip("ONNXRuntime backend is not enabled!")
    env = os.environ.copy()
    opts = {
        "backend": backend,
        "nthreads": multiprocessing.cpu_count() // 2,
    }
    env["TEST_OPTS"] = json.dumps(opts)
    reg = r"Check sum\:\W\d+"
    ex1 = run_gaudi(f"{__file__}:config", env=env)
    assert ex1.returncode == 0
    matches1 = re.findall(reg, ex1.stdout)
    if not matches1:
        raise AssertionError("No check sum info!")
    check_sum1 = int(matches1[0].split()[2])
    ex2 = run_gaudi(f"{__file__}:config", env=env)
    assert ex2.returncode == 0
    matches2 = re.findall(reg, ex2.stdout)
    if not matches2:
        raise AssertionError("No check sum info!")
    check_sum2 = int(matches2[0].split()[2])
    if check_sum1 != check_sum2 or check_sum2 == 0:
        raise AssertionError("Check sums are not the same!")

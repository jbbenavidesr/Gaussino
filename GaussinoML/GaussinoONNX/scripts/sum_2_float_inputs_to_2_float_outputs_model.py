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
import argparse

import onnx
import torch as T


class Model(T.nn.Module):
    def forward(self, input_0, input_1):
        return input_0.sum(), input_1.sum()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        "ONNX converter of the Sum2FloatInputsTo2FloatOutputsModel"
    )
    parser.add_argument(
        "--file",
        action="store",
        default="Sum2FloatInputsTo2FloatOutputsModel.onnx",
        help="The name of the output file with the onnx converted model",
    )
    parsed = parser.parse_args()

    # 1st approach: tracing, does not wok with dynamic models
    # model = Model()
    # model.eval()

    # 2nd approach: scripting, works with dynamic models
    model = T.jit.script(Model())

    T.onnx.export(
        model,
        (T.ones((1,), dtype=T.float32), T.ones((1,), dtype=T.float32)),
        parsed.file,
        verbose=True,
        input_names=["InputFloat0", "InputFloat1"],
        output_names=["OutputFloat0", "OutputFloat1"],
        export_params=True,
    )

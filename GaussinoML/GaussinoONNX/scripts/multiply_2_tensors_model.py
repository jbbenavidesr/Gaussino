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

import torch as T


class Model(T.nn.Module):
    def forward(self, x, y):
        return T.mm(x, y)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("ONNX converter of the Multiply2TensorsModel")
    parser.add_argument(
        "--file",
        action="store",
        default="Multiply2TensorsModel.onnx",
        help="The name of the output file with the onnx converted model.",
    )
    parser.add_argument(
        "--dimensions",
        action="store",
        default=(3, 3),
        nargs=2,
        type=int,
        help="Dimensions of the input matrices.",
    )
    parsed = parser.parse_args()

    # 1st approach: tracing, does not wok with dynamic models
    # model = Model()
    # model.eval()

    # 2nd approach: scripting, works with dynamic models
    model = T.jit.script(Model())

    dims = parsed.dimensions
    T.onnx.export(
        model,
        (T.ones(dims, dtype=T.float32), T.ones(dims, dtype=T.float32)),
        parsed.file,
        verbose=True,
        input_names=["InputTensor0", "InputTensor1"],
        output_names=["OutputTensor0"],
        export_params=True,
    )

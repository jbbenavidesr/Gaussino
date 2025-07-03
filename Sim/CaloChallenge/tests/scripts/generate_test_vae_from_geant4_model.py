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

import argparse
import os

import torch as T


class TestVAEFromGeant4Model(T.nn.Module):
    def __init__(self, *args, output_elements_no, **kwargs):
        self.output_elements_no = output_elements_no
        super().__init__(*args, **kwargs)

    def forward(
        self,
        input_tensor,
    ):
        # some operations here to include the input_tensor
        input_tensor = T.cat(
            [
                input_tensor,
                T.ones(
                    self.output_elements_no - input_tensor.shape[0], dtype=T.float32
                ),
            ]
        )
        return input_tensor * 0.0 + 1.0


def serialize(
    directory: str,
    output_elements_no: int,
    input_elements_no: int,
):
    model = TestVAEFromGeant4Model(
        output_elements_no=output_elements_no,
    )
    model = T.jit.script(model)
    # serialize to .pt
    model.save(os.path.join(directory, "TestVAEFromGeant4Model.pt"))

    # serialize to .onnx
    T.onnx.export(
        model,
        (T.ones(input_elements_no, dtype=T.float32)),
        os.path.join(directory, "TestVAEFromGeant4Model.onnx"),
        verbose=True,
        input_names=["InputTensor0"],
        output_names=["OutputTensor0"],
        export_params=True,
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Serializer of the TestVAEFromGeant4Model.")
    parser.add_argument(
        "--directory",
        action="store",
        required=True,
        help="The name of the output dir for the serialized models.",
    )
    parser.add_argument(
        "--output_elements_no",
        action="store",
        # default:
        # = 18 (rho) + 50 (phi) + 45 (z)
        default=40500,
        type=int,
        help="The number of elements in the output tensor.",
    )
    parser.add_argument(
        "--input_elements_no",
        action="store",
        # default:
        # = 10 (latent) + 1 (energy) + 1 (theta) + 2 (0, 1)
        default=14,
        type=int,
        help="The number of elements in the input tensor.",
    )
    parsed = parser.parse_args()
    serialize(
        parsed.directory,
        parsed.output_elements_no,
        parsed.input_elements_no,
    )

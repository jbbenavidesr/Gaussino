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
    # reproduces the matrix multiplication throughput test at
    # -> https://pytorch.org/docs/stable/notes/cpu_threading_torchscript_inference.html
    def forward(self, x_dim: int = 1024, y_dim: int = 1024) -> int:
        x = T.randn(x_dim, x_dim)
        y = T.randn(y_dim, y_dim)
        T.mm(x, y)
        return x_dim + y_dim  # just to check if succeeded


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Serializer of the MatrixMultiplicationModel")
    parser.add_argument(
        "--file",
        action="store",
        default="MatrixMultiplicationModel.pt",
        help="The name of the output file with the serialized model.",
    )
    parsed = parser.parse_args()

    T.jit.script(Model()).save(parsed.file)

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
    def forward(self, input_0, input_1):
        return input_0.sum() + input_1.sum()


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Serializer of the Sum2InputsTo1OutputModel")
    parser.add_argument(
        "--file",
        action="store",
        default="Sum2InputsTo1OutputModel.pt",
        help="The name of the output file with the serialized model.",
    )
    parsed = parser.parse_args()

    T.jit.script(Model()).save(parsed.file)

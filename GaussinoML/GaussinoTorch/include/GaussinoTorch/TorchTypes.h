/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#pragma once

#include <torch/script.h>
#include <torch/torch.h>

namespace Gsino::ML::Torch {
  using Tensor           = at::Tensor;
  using IValue           = torch::jit::IValue;
  using SerializedModule = torch::jit::Module;
} // namespace Gsino::ML::Torch

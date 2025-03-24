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

// ONNX
#include "core/session/onnxruntime_cxx_api.h"

namespace Gsino::ML::ONNX {
  using Tensor         = Ort::Value;
  using Value          = Ort::Value;
  using Session        = Ort::Session;
  using SessionOptions = Ort::SessionOptions;
  using Env            = Ort::Env;
  using MemoryInfo     = Ort::MemoryInfo;
  //   using SerializedModule = torch::jit::Module;
} // namespace Gsino::ML::ONNX

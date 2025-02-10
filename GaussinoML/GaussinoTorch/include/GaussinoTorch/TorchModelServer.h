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

#include <type_traits>

// Gaussino
#include "GaussinoMLBase/ModelServerBase.h"
#include "TorchTypes.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

namespace Gsino::ML::Torch {

  using InputShapes = std::vector<std::vector<int64_t>>;

  struct ModelServer : public ModelServerBase {

    using ModelServerBase::ModelServerBase;

    std::unique_ptr<SerializedModule> m_model;

    bool load( const std::string& model_path );

    void setAutoInputTypes( bool autoTypes ) { m_autoInputTypes = autoTypes; }
    void setAutoOutputTypes( bool autoTypes ) { m_autoOutputTypes = autoTypes; }
    void setVectorInputShapes( InputShapes shapes ) { m_vectorInputShapes = shapes; }
    void setInterOpThreads( size_t threads_no );
    void setIntraOpThreads( size_t threads_no );
    void printThreadInfo();

    template <class OUTPUT, class... INPUTS>
    OUTPUT evaluate( INPUTS&&... ) const;

    template <class T>
    std::vector<T> makeContiguous( Tensor& ) const;

  protected:
    bool        m_autoInputTypes    = true;
    bool        m_autoOutputTypes   = true;
    InputShapes m_vectorInputShapes = {};

    GaudiException evalError( const std::string&, const std::string& ) const;
  };

} // namespace Gsino::ML::Torch

template <class OUTPUT, class... INPUTS>
OUTPUT Gsino::ML::Torch::ModelServer::evaluate( INPUTS&&... inputs ) const {
  std::vector<IValue> inputValues;
  size_t              vectorInputsNo = 0;
  this->print( "Evaluating the model '" + this->m_name + "'...", MSG::Level::VERBOSE );
  (
      [&] {
        try {
          using INPUT = typename std::remove_reference<INPUTS>::type;
          if ( !m_autoInputTypes ) {
            inputValues.push_back( inputs );
          } else if constexpr ( is_specialization<INPUT, std::vector>::value ) {
            // torch won't tell us the dimension of the input tensor, so we need to
            // fetch it from the properties
            if ( m_vectorInputShapes.size() >= vectorInputsNo + 1 ) {
              inputValues.push_back(
                  torch::from_blob( inputs.data(), c10::IntArrayRef( m_vectorInputShapes[vectorInputsNo] ) ) );
              vectorInputsNo++;
            } else {
              inputValues.push_back( torch::tensor( inputs ) );
            }
          } else if constexpr ( std::is_same_v<INPUT, Tensor> || std::is_same_v<INPUT, IValue> ) {
            inputValues.push_back( inputs );
          } else {
            inputValues.push_back( torch::tensor( inputs ) );
          }
        } catch ( const c10::Error& e ) { throw evalError( e.what(), "Could not convert the input type." ); }
      }(),
      ... );
  if ( this->level() <= MSG::Level::VERBOSE ) {
    this->print( "-> input: \n" + str( inputValues ), MSG::Level::VERBOSE );
  }

  IValue raw_output;
  try {
    raw_output = m_model->forward( inputValues );
    if ( this->level() <= MSG::Level::VERBOSE ) {
      this->print( "-> output: \n" + str( raw_output ), MSG::Level::VERBOSE );
    }
  } catch ( const c10::Error& e ) {
    std::string msg = "Most likely, the 'forward' method in the serialized model"
                      " in PyTorch accepts a different number of parameters in its"
                      " signature or their types are incorrect.";
    throw evalError( e.what(), msg );
  }

  if constexpr ( std::is_same_v<OUTPUT, IValue> ) return raw_output;

  if ( !m_autoOutputTypes ) {
    std::string msg = "Automatic model types deduction is turned off."
                      " Only OUTPUT = 'Gsino::ML::Torch::IValue' is supported in this case.";
    throw evalError( msg, msg );
  }

  if ( !raw_output.isTensor() ) {
    std::string msg = "Automatic model types deduction does not support multiple outputs yet."
                      " Only OUTPUT = 'Gsino::ML::Torch::IValue' is supported in this case.";
    throw evalError( msg, msg );
  }

  auto* ts = &raw_output.toTensor();
  if constexpr ( std::is_arithmetic_v<OUTPUT> ) {
    try {
      return *( ts->data_ptr<OUTPUT>() );
    } catch ( const c10::Error& e ) {
      throw evalError( e.what(), "Expected a different type in the output: " + str( ts->scalar_type() ) );
    }
  }

  if constexpr ( is_specialization<OUTPUT, std::vector>::value ) {
    return makeContiguous<typename OUTPUT::value_type>( *ts );
  }

  if constexpr ( std::is_same_v<OUTPUT, Tensor> ) { return *ts; }

  std::string msg = "Unsupported output type for the model.";
  throw evalError( msg, msg );
}

template <class T>
std::vector<T> Gsino::ML::Torch::ModelServer::makeContiguous( Tensor& tensor ) const {
  at::Tensor output_tensor = tensor.contiguous();
  try {
    auto           tensor_start = output_tensor.data_ptr<T>();
    std::vector<T> output( tensor_start, tensor_start + output_tensor.numel() );
    return output;
  } catch ( const c10::Error& e ) {
    std::string msg = "There's a mismatch between the expected"
                      " type in the contiguous vector and the output type in the tensor."
                      " Make sure you have the right type.";
    throw evalError( e.what(), msg );
  }
}

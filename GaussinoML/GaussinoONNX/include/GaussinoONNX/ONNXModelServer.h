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

#include <thread>

// Gaussino
#include "GaussinoMLBase/ModelServerBase.h"
#include "ONNXTypes.h"
// Gaudi
#include "GaudiKernel/GaudiException.h"

namespace Gsino::ML::ONNX {
  struct ModelServer : public ModelServerBase {

    using ModelServerBase::ModelServerBase;

    inline void setAutoInputTypes( bool autoTypes ) { m_autoInputTypes = autoTypes; }

    inline void setAutoOutputTypes( bool autoTypes ) { m_autoOutputTypes = autoTypes; }

    void setInterOpThreads( size_t threads_no );

    void setIntraOpThreads( size_t threads_no );

    void setProfiling( const std::string& filePath );

    void loadModel( const std::string& modelPath );

    void setupSession();

    void setGraphOptimization( const std::string& lvl );

    MemoryInfo& getMemoryInfo() const { return *m_memoryInfo; }

    template <class OUTPUT, class... INPUTS>
    OUTPUT evaluate( INPUTS&&... ) const;

    template <class T>
    std::vector<T> makeContiguous( Tensor& ) const;

  protected:
    // Session
    std::array<std::vector<const char*>, 2>             m_nodeNames;
    std::array<std::vector<Ort::AllocatedStringPtr>, 2> m_namePtrs;
    std::array<std::vector<std::vector<int64_t>>, 2>    m_nodeDims;

    // Env
    std::unique_ptr<SessionOptions>       m_sessionOptions = std::make_unique<SessionOptions>();
    std::unique_ptr<Env>                  m_env;
    std::vector<std::unique_ptr<Session>> m_sessions;
    std::unique_ptr<MemoryInfo>           m_memoryInfo;

    bool m_autoInputTypes  = true;
    bool m_autoOutputTypes = true;

    // TODO: working with a simplified interface for now;
    //       will need to implement interOp threads for multiple sessions
    size_t m_interOpThreads = 1;

    GaudiException evalError( const std::string&, const std::string& = "" ) const;

  private:
    OrtLoggingLevel getLoggerLevel() const;
    // mutable std::map<std::thread::id, size_t> m_threadsMap;
    mutable std::vector<bool> m_sessionsPoll = {};
    mutable std::mutex        m_threadsLock;
  };
} // namespace Gsino::ML::ONNX

template <class OUTPUT, class... INPUTS>
OUTPUT Gsino::ML::ONNX::ModelServer::evaluate( INPUTS&&... inputs ) const {
  std::vector<Tensor> inputTensors;
  std::vector<Tensor> outputTensors;
  this->print( "Evaluating the model '" + this->m_name + "'...", MSG::Level::VERBOSE );
  size_t i = 0;
  (
      [&] {
        try {
          using INPUT = typename std::remove_reference<INPUTS>::type;
          if constexpr ( std::is_same_v<INPUT, Tensor> ) {
            // FIXME: no move constructor for Tensor
            inputTensors.emplace_back( std::forward<INPUT>( inputs ) );
          } else if ( !m_autoInputTypes ) {
            std::string msg = "Automatic model types deduction is turned off."
                              " Only INPUT = 'Gsino::ML::ONNX::Tensor' is supported in this case.";
            throw evalError( msg, msg );
          } else if constexpr ( std::is_arithmetic_v<INPUT> ) {
            std::array<int64_t, 1> emptyDim{ 1 };
            inputTensors.emplace_back(
                Tensor::CreateTensor<INPUT>( *m_memoryInfo, &inputs, 1, emptyDim.data(), emptyDim.size() ) );
          } else if constexpr ( is_specialization<INPUT, std::vector>::value ) {
            inputTensors.emplace_back( Tensor::CreateTensor<typename INPUT::value_type>(
                *m_memoryInfo, inputs.data(), inputs.size(), this->m_nodeDims[0][i].data(),
                this->m_nodeDims[0][i].size() ) );
          } else {
            throw evalError( "Unsupported input type." );
          }
          i++;
        } catch ( const Ort::Exception& e ) { throw evalError( e.what(), "Could not convert the input type." ); }
      }(),
      ... );

  try {

    size_t sessID = 0;
    if ( m_interOpThreads > 1 ) {
      // TODO: working with a simplified interface for now;
      //       will need to implement interOp threads for multiple sessions
      //       as well as support for multiple models
      std::lock_guard<std::mutex> lock( m_threadsLock );
      auto                        sessIt = std::find( m_sessionsPoll.begin(), m_sessionsPoll.end(), false );
      if ( sessIt == m_sessionsPoll.end() ) { throw evalError( "Not enough allocated interOpThreads!" ); }
      sessID                 = std::distance( m_sessionsPoll.begin(), sessIt );
      m_sessionsPoll[sessID] = true;
    }

    outputTensors = m_sessions[sessID]->Run( Ort::RunOptions{ nullptr }, m_nodeNames[0].data(), inputTensors.data(),
                                             inputTensors.size(), m_nodeNames[1].data(), m_nodeNames[1].size() );
    if ( m_interOpThreads > 1 ) {
      // TODO: working with a simplified interface for now;
      //       will need to implement interOp threads for multiple sessions
      //       as well as support for multiple models
      std::lock_guard<std::mutex> lock( m_threadsLock );
      m_sessionsPoll[sessID] = false;
    }
  } catch ( const Ort::Exception& e ) {
    std::string msg = "Most likely, the 'Run' method in the serialized model"
                      " in PyTorch accepts a different number of parameters in its"
                      " signature or their types are incorrect.";
    throw evalError( e.what(), msg );
  }

  if constexpr ( is_specialization<OUTPUT, std::vector>::value ) {
    using OUTPUT_VALUE = typename OUTPUT::value_type;
    if constexpr ( std::is_same_v<OUTPUT_VALUE, Tensor> ) {
      return outputTensors;
    } else if ( outputTensors.size() == 1 ) {
      if ( m_autoOutputTypes && Ort::TypeToTensorType<OUTPUT_VALUE>::type !=
                                    outputTensors[0].GetTensorTypeAndShapeInfo().GetElementType() ) {
        std::string msg = "The output type of the model is different from the requested one.";
        throw evalError( msg, msg );
      }
      return this->makeContiguous<OUTPUT_VALUE>( outputTensors[0] );
    }
  }

  if ( !m_autoOutputTypes ) {
    std::string msg = "Automatic model types deduction is turned off."
                      " Only OUTPUT = 'std::vector<Gsino::ML::ONNX::Tensor>' is supported in this case.";
    throw evalError( msg, msg );
  }

  if ( outputTensors.size() == 1 ) {
    if constexpr ( std::is_arithmetic_v<OUTPUT> ) {
      if ( Ort::TypeToTensorType<OUTPUT>::type != outputTensors[0].GetTensorTypeAndShapeInfo().GetElementType() ) {
        std::string msg = "The output type of the model is different from the requested one.";
        throw evalError( msg, msg );
      }
      try {
        return outputTensors[0].GetTensorData<OUTPUT>()[0];
      } catch ( const Ort::Exception& e ) { throw evalError( e.what(), "Expected a different type in the output." ); }
    } else if constexpr ( std::is_same_v<OUTPUT, Tensor> ) {
      return std::move( outputTensors[0] );
    }
  }

  std::string msg = "Unsupported output type for the model.";
  throw evalError( msg, msg );
}

template <class T>
std::vector<T> Gsino::ML::ONNX::ModelServer::makeContiguous( Tensor& tensor ) const {
  try {
    auto           tensorStart = tensor.GetTensorData<T>();
    auto           tensorSize  = tensor.GetTensorTypeAndShapeInfo().GetElementCount();
    std::vector<T> output( tensorStart, tensorStart + tensorSize );
    return output;
  } catch ( const Ort::Exception& e ) {
    std::string msg = "There's a mismatch between the expected"
                      " type in the contiguous vector and the output type in the tensor."
                      " Make sure you have the right type.";
    throw evalError( e.what(), msg );
  }
}

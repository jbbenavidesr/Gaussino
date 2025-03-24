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

#include "GaussinoONNX/ONNXModelServer.h"

void Gsino::ML::ONNX::ModelServer::setIntraOpThreads( size_t threads_no ) {
  m_sessionOptions->SetIntraOpNumThreads( threads_no );
};

void Gsino::ML::ONNX::ModelServer::setInterOpThreads( size_t threads_no ) {
  // TODO: working with a simplified interface for now;
  //       will need to implement interOp threads for multiple sessions
  //       as well as support for multiple models
  //
  // m_sessionOptions->SetInterOpNumThreads( threads_no );
  //
  m_interOpThreads = threads_no;
  std::vector<bool> sessions( threads_no, false );
  m_sessionsPoll = sessions;
};

void Gsino::ML::ONNX::ModelServer::setProfiling( const std::string& filePath ) {
  m_sessionOptions->EnableProfiling( filePath.c_str() );
};

void Gsino::ML::ONNX::ModelServer::loadModel( const std::string& modelPath ) {
  m_env = std::make_unique<Env>( getLoggerLevel(), ( m_name + "_Environment" ).c_str() );
  for ( size_t i = 0; i < m_interOpThreads; i++ ) {
    m_sessions.push_back( std::make_unique<Session>( *m_env, modelPath.c_str(), *m_sessionOptions ) );
  }
  m_memoryInfo = std::make_unique<MemoryInfo>(
      MemoryInfo::CreateCpu( OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault ) );
};

void Gsino::ML::ONNX::ModelServer::setupSession() {
  Ort::AllocatorWithDefaultOptions allocator;

  size_t inputNodesNo = m_sessions[0]->GetInputCount();
  for ( size_t i = 0; i < inputNodesNo; i++ ) {
    auto input_name = m_sessions[0]->GetInputNameAllocated( i, allocator );
    m_nodeNames[0].push_back( input_name.get() );
    m_namePtrs[0].push_back( std::move( input_name ) );
    m_nodeDims[0].push_back( m_sessions[0]->GetInputTypeInfo( i ).GetTensorTypeAndShapeInfo().GetShape() );
    for ( auto& node : m_nodeDims[0].back() ) {
      if ( node < 0 ) node = 1;
    }
  }

  size_t outputNodesNo = m_sessions[0]->GetOutputCount();
  for ( size_t i = 0; i < outputNodesNo; i++ ) {
    auto output_name = m_sessions[0]->GetOutputNameAllocated( i, allocator );
    m_nodeNames[1].push_back( output_name.get() );
    m_namePtrs[1].push_back( std::move( output_name ) );
    m_nodeDims[1].push_back( m_sessions[0]->GetOutputTypeInfo( i ).GetTensorTypeAndShapeInfo().GetShape() );
    for ( auto& node : m_nodeDims[1].back() ) {
      if ( node < 0 ) node = 1;
    }
  }
};

void Gsino::ML::ONNX::ModelServer::setGraphOptimization( const std::string& lvl ) {
  //   m_sessionOptions.SetOptimizedModelFilePath("opt-graph");
  if ( lvl == "ENABLE_ALL" ) {
    m_sessionOptions->SetGraphOptimizationLevel( GraphOptimizationLevel::ORT_ENABLE_ALL );
  } else if ( lvl == "DISABLE_ALL" ) {
    m_sessionOptions->SetGraphOptimizationLevel( GraphOptimizationLevel::ORT_DISABLE_ALL );
  } else if ( lvl == "ENABLE_BASIC" ) {
    m_sessionOptions->SetGraphOptimizationLevel( GraphOptimizationLevel::ORT_ENABLE_BASIC );
  } else if ( lvl == "ENABLE_EXTENDED" ) {
    m_sessionOptions->SetGraphOptimizationLevel( GraphOptimizationLevel::ORT_ENABLE_EXTENDED );
  } else {
    throw evalError( "Graph optimization level not recognized" );
  }
};

OrtLoggingLevel Gsino::ML::ONNX::ModelServer::getLoggerLevel() const {
  switch ( level() ) {
  case MSG::Level::VERBOSE:
  case MSG::Level::DEBUG:
    return ORT_LOGGING_LEVEL_VERBOSE;
  case MSG::Level::INFO:
    return ORT_LOGGING_LEVEL_INFO;
  case MSG::Level::WARNING:
    return ORT_LOGGING_LEVEL_WARNING;
  case MSG::Level::ERROR:
    return ORT_LOGGING_LEVEL_ERROR;
  case MSG::Level::FATAL:
    return ORT_LOGGING_LEVEL_FATAL;
  default:
    return ORT_LOGGING_LEVEL_INFO;
  };
};

GaudiException Gsino::ML::ONNX::ModelServer::evalError( const std::string& errMsg, const std::string& msg ) const {
  std::string printable = "Error with the model '" + this->m_name + "'. ";
  this->print( printable + msg, MSG::Level::ERROR );
  return GaudiException( errMsg, "Gsino::ML::ONNX::ModelServer", StatusCode::FAILURE );
}

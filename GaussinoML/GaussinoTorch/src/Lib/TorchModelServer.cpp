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

// LHCb
#include "GaussinoTorch/TorchModelServer.h"

GaudiException Gsino::ML::Torch::ModelServer::evalError( const std::string& errMsg, const std::string& msg ) const {
  std::string printable = "Error with the model '" + this->m_name + "'. ";
  this->print( printable + msg, MSG::Level::ERROR );
  return GaudiException( errMsg, "Gsino::ML::Torch::ModelServer", StatusCode::FAILURE );
}

bool Gsino::ML::Torch::ModelServer::load( const std::string& model_path ) {
  this->print( "Loading the torch model from '" + model_path + "'", MSG::Level::DEBUG );
  try {
    m_model = std::make_unique<SerializedModule>( std::move( torch::jit::load( model_path ) ) );
  } catch ( const c10::Error& e ) {
    this->print( "Error loading a model from '" + model_path + "'", MSG::Level::ERROR );
    throw GaudiException( e.what(), "Gsino::ML::Torch::ModelServer", StatusCode::FAILURE );
  }
  return true;
}

void Gsino::ML::Torch::ModelServer::setInterOpThreads( size_t threads_no ) {
  at::set_num_interop_threads( threads_no );
};

void Gsino::ML::Torch::ModelServer::setIntraOpThreads( size_t threads_no ) { at::set_num_threads( threads_no ); };

void Gsino::ML::Torch::ModelServer::printThreadInfo() { this->print( at::get_parallel_info(), MSG::Level::INFO ); };

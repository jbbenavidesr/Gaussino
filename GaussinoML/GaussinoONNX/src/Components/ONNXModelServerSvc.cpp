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

// Gaudi
#include "GaudiKernel/Service.h"

// Gaussino
#include "GaussinoMLBase/IModelServerSvc.h"
#include "GaussinoONNX/ONNXModelServer.h"

namespace Gsino::ML::ONNX {

  struct ModelServerSvc : public extends<Service, IModelServerSvc<ModelServer>> {
    using extends::extends;

    virtual ModelServer* getServer() const override { return m_server.get(); };

    StatusCode initialize() override {
      return extends::initialize().andThen( [&]() -> StatusCode {
        if ( m_modelPath.value().empty() ) {
          error() << "You must provide a path to the model!" << endmsg;
          return StatusCode::FAILURE;
        }

        if ( m_modelName.value().empty() ) {
          error() << "The model must have a name!" << endmsg;
          return StatusCode::FAILURE;
        }

        m_server = std::make_unique<ModelServer>( m_modelName.value() );
        m_server->setMsgStream( std::make_unique<MsgStream>( msgSvc(), name() ) );
        m_server->msgStream()->setLevel( msgLevel() );
        if ( m_intraopThreads > 0 ) { m_server->setIntraOpThreads( m_intraopThreads.value() ); }
        if ( m_interopThreads > 0 ) { m_server->setInterOpThreads( m_interopThreads.value() ); }
        if ( !m_profilingFilePath.value().empty() ) { m_server->setProfiling( m_profilingFilePath.value() ); }
        m_server->setAutoInputTypes( m_autoInputTypes );
        m_server->setAutoOutputTypes( m_autoOutputTypes );
        m_server->loadModel( m_modelPath.value() );
        m_server->setupSession();
        return StatusCode::SUCCESS;
      } );
    }

  private:
    std::unique_ptr<ModelServer> m_server;

    Gaudi::Property<std::string> m_modelPath{ this, "ModelPath", "" };
    Gaudi::Property<std::string> m_modelName{ this, "ModelName", "UnspecifiedONNXModel" };
    Gaudi::Property<std::string> m_graphOptimizationLevel{ this, "GraphOptimizationLevel", "ENABLE_ALL" };
    Gaudi::Property<std::string> m_profilingFilePath{ this, "ProfilingFilePath", "" };
    Gaudi::Property<bool>        m_autoInputTypes{ this, "AutoInputTypes", true, "Auto-detect input types" };
    Gaudi::Property<bool>        m_autoOutputTypes{ this, "AutoOutputTypes", true, "Auto-detect output types" };
    Gaudi::Property<size_t>      m_intraopThreads{ this, "IntraOpThreads", 0,
                                              "Number of intra-op threads (0 -> ONNX default)" };
    Gaudi::Property<size_t>      m_interopThreads{ this, "InterOpThreads", 0,
                                              "Number of inter-op threads (0 -> ONNX default)" };
  };
} // namespace Gsino::ML::ONNX

DECLARE_COMPONENT( Gsino::ML::ONNX::ModelServerSvc )

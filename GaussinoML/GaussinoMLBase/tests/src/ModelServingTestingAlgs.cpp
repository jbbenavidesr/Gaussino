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
#include <chrono>

// Gaussino

#ifdef GSINO_USE_TORCH
#  include "GaussinoTorch/TorchModelServer.h"
#endif

#ifdef GSINO_USE_ONNXRUNTIME
#  include "GaussinoONNX/ONNXModelServer.h"
#endif

#include "GaussinoMLBase/IModelServerSvc.h"
#include "NewRnd/RndAlgSeeder.h"
#include "NewRnd/RndGlobal.h"

// Gaudi
#include "GaudiAlg/Consumer.h"
#include <GaudiKernel/ServiceHandle.h>

namespace Gsino::ML {
  template <class TModelServer>

  struct TestMMThroughputAlg
      : public Gaudi::Functional::Consumer<void(), Gaudi::Functional::Traits::BaseClass_t<RndAlgSeeder>> {
    using Consumer::Consumer;
    using Clock = std::chrono::high_resolution_clock;

    // virtual std::vector<float> evaluate() const = 0;

    void operator()() const override {
      // generate random numbers
      auto                     engine = createRndmEngine();
      ThreadLocalEngine::Guard guard( engine );
      CLHEP::RandFlat          flatGenerator{ *engine.get(), 0, 1 };
      std::vector<float>       x( m_xDim * m_yDim, 1 );
      std::vector<float>       y( m_xDim * m_yDim, 1 );
      std::generate( x.begin(), x.end(), [&]() { return round( flatGenerator() * 100 ); } );
      std::generate( y.begin(), y.end(), [&]() { return round( flatGenerator() * 100 ); } );

      // run and measure the inference time
      auto start_time = Clock::now();
      auto result     = m_server_svc->getServer()->template evaluate<std::vector<float>>( x, y );
      auto end_time   = Clock::now();

      // monitor the results
      m_checkSum += std::reduce( result.begin(), result.end() );
      auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time ).count();
      m_time += diff;
    };

    StatusCode finalize() override {
      info() << "Check sum: " << m_checkSum.sum() << endmsg;
      if ( !m_csv_path.value().empty() ) {
        std::ofstream outf;
        outf.open( m_csv_path, std::ios_base::app );
        if ( !outf.is_open() ) {
          error() << "Could not open file " << m_csv_path << endmsg;
          return StatusCode::FAILURE;
        }
        outf << m_name.value() << "," << m_time.mean() << "," << m_time.standard_deviation() << std::endl;
        outf.close();
      }
      return Consumer::finalize();
    }

    mutable Gaudi::Accumulators::StatCounter<>           m_time{ this, "Inference time (ns)" };
    mutable Gaudi::Accumulators::SummingCounter<int64_t> m_checkSum{ this, "Check sum" };
    ServiceHandle<IModelServerSvc<TModelServer>>         m_server_svc{ this, "ModelServingSvc", "" };

    Gaudi::Property<int>         m_xDim{ this, "DimX", 1024 };
    Gaudi::Property<int>         m_yDim{ this, "DimY", 1024 };
    Gaudi::Property<std::string> m_csv_path{ this, "CSVFilePath", "" };
    Gaudi::Property<std::string> m_name{ this, "Name", "" };
  };

} // namespace Gsino::ML

#ifdef GSINO_USE_TORCH
namespace Gsino::ML::Torch {
  struct TestMMThroughputAlg : public ML::TestMMThroughputAlg<ModelServer> {
    using ML::TestMMThroughputAlg<ModelServer>::TestMMThroughputAlg;
    StatusCode initialize() override {
      return ML::TestMMThroughputAlg<ModelServer>::initialize().andThen( [&]() -> StatusCode {
        m_server_svc->getServer()->setVectorInputShapes( { { m_xDim, m_yDim }, { m_xDim, m_yDim } } );
        return StatusCode::SUCCESS;
      } );
    }
  };
  DECLARE_COMPONENT( TestMMThroughputAlg )
} // namespace Gsino::ML::Torch
#endif

#ifdef GSINO_USE_ONNXRUNTIME
namespace Gsino::ML::ONNX {
  using TestMMThroughputAlg = ML::TestMMThroughputAlg<ModelServer>;
  DECLARE_COMPONENT_WITH_ID( TestMMThroughputAlg, "Gsino__ML__ONNX__TestMMThroughputAlg" )
} // namespace Gsino::ML::ONNX
#endif

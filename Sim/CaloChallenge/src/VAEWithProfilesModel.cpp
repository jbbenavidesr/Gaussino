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
// CLHEP
#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGauss.h>

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SystemOfUnits.h"

// Gaussino
#include "GaussinoMLBase/IModelServerSvc.h"
#include "IMLModel.h"
#ifdef GSINO_USE_ONNXRUNTIME
#  include "GaussinoONNX/ONNXModelServer.h"
#endif

namespace Gsino::CaloChallenge {
  /**
   * @brief A tool that implements the IMLModel interface to fill showers using a custom VAE model with profiles.
   *
   * This tool uses a machine learning model server to generate shower energies based on latent vectors
   * and conditions such as particle energy and angle, while also providing energy profiles.
   *
   * @tparam TModelServer The type of the model server (e.g., Torch or ONNX).
   * @author Michał Mazurek
   * @date 2023
   */
  template <class TModelServer>
  class VAEWithProfilesModel : public extends<GaudiTool, IMLModel> {
    static_assert( std::is_base_of<ML::ModelServerBase, TModelServer>::value );

    Gaudi::Property<size_t> m_latentVectorSize{ this, "LatentVectorSize", 10 };
    Gaudi::Property<float>  m_maxEnergy{ this, "MaxEnergy", 1024000.0 * Gaudi::Units::MeV };
    Gaudi::Property<float>  m_maxTheta{ this, "MaxTheta", 90.0 * Gaudi::Units::degree };
    Gaudi::Property<float>  m_maxPhi{ this, "MaxPhi", 360.0 * Gaudi::Units::degree };
    Gaudi::Property<float>  m_eProfileLogEnergyMax{ this, "EProfileLogEnergyMax", 2.0 };
    Gaudi::Property<float>  m_eProfileLogEnergyMin{ this, "EProfileLogEnergyMin", -4.0 };
    Gaudi::Property<size_t> m_eProfileLogEnergyBinsNo{ this, "EProfileLogEnergyBinsNo", 80 };
    Gaudi::Property<float>  m_hitsNoOverflow{ this, "HitsNoOverflow", 1.1 };

    ServiceHandle<ML::IModelServerSvc<TModelServer>> m_serverSvc{ this, "ModelServerSvc", "" };

    std::vector<float> m_energyBins    = {};
    float              m_energyBinSize = 0.0;

  public:
    using extends::extends;

    virtual StatusCode initialize() override {
      return extends::initialize().andThen( [&]() {
        if ( m_eProfileLogEnergyBinsNo.value() < 0 ) {
          error() << "Number of energy bins must be positive" << endmsg;
          return StatusCode::FAILURE;
        }

        m_energyBinSize = ( m_eProfileLogEnergyMax - m_eProfileLogEnergyMin ) / (float)m_eProfileLogEnergyBinsNo;
        for ( size_t i = 0; i < m_eProfileLogEnergyBinsNo; i++ ) {
          m_energyBins.push_back( m_eProfileLogEnergyMin + i * m_energyBinSize );
        }

        return StatusCode::SUCCESS;
      } );
    }

    virtual void fillShower( std::vector<float>& energies, CLHEP::HepRandomEngine* engine, float particleEnergy,
                             int pid, float theta, float phi ) const override {
      CLHEP::RandFlat    flatGenerator{ *engine, 0, 1 };
      CLHEP::RandGauss   gaussGenerator{ *engine, 0, 1 };
      std::vector<float> latentVector( m_latentVectorSize.value() );
      std::generate( latentVector.begin(), latentVector.end(), [&]() { return gaussGenerator(); } );

      float              totalEnergy = 0;
      float              totalHitsNo = 0;
      std::vector<float> rhoProfile;
      std::vector<float> phiProfile;
      std::vector<float> zProfile;
      std::vector<float> eProfile;

#ifdef GSINO_USE_ONNXRUNTIME
      if constexpr ( std::is_same_v<TModelServer, ML::ONNX::ModelServer> ) {
        std::vector<ML::ONNX::Tensor> tensorVector;
        std::vector<float>            particleVector( 8, 0 );
        particleVector[0] = particleEnergy / m_maxEnergy.value();
        particleVector[1] = theta / m_maxTheta.value();
        particleVector[2] = phi / m_maxPhi.value();

        if ( pid == 11 || pid == -11 ) {
          particleVector[3] = 1;
        } else if ( pid == 22 ) {
          particleVector[4] = 1;
        } else {
          throw GaudiException( "Unsupported PIDs during inference!", this->name(), StatusCode::FAILURE );
        }
        particleVector[5] = 1;
        auto srv          = m_serverSvc->getServer();
        tensorVector      = srv->template evaluate<std::vector<ML::ONNX::Tensor>>( latentVector, particleVector );
        totalHitsNo       = tensorVector[0].GetTensorData<float>()[0];
        totalEnergy       = tensorVector[1].GetTensorData<float>()[0];
        zProfile          = srv->template makeContiguous<float>( tensorVector[2] );
        rhoProfile        = srv->template makeContiguous<float>( tensorVector[3] );
        phiProfile        = srv->template makeContiguous<float>( tensorVector[4] );
        eProfile          = srv->template makeContiguous<float>( tensorVector[5] );
      }
#endif
      float totalHitsNoParsed = totalHitsNo * m_meshNumber.x() * m_meshNumber.y() * m_meshNumber.z();
      float totalEnergyParsed = totalEnergy * particleEnergy;

      auto generateCumulativeProbabilities = []( const std::vector<float>& profile, std::vector<float>& cumProb ) {
        auto profileSum = std::accumulate( profile.begin(), profile.end(), 0.0 );
        std::transform( profile.begin(), profile.end(), std::back_inserter( cumProb ),
                        [&]( auto& val ) { return val / profileSum; } );
        std::partial_sum( cumProb.begin(), cumProb.end(), cumProb.begin() );
      };

      auto sample = [&]( const std::vector<float>& cumProb ) -> size_t {
        auto x     = flatGenerator();
        auto bound = std::lower_bound( cumProb.begin(), cumProb.end(), x );
        return std::distance( cumProb.begin(), bound );
      };

      std::transform( eProfile.begin(), eProfile.end(), eProfile.begin(), [&]( auto& val ) {
        auto parsed = std::round( val * totalHitsNoParsed * 1e6 ) / 1e6;
        return parsed >= 1e-2 ? parsed : 0.0;
      } );
      std::transform( zProfile.begin(), zProfile.end(), zProfile.begin(), [&]( auto& val ) {
        auto parsed = std::round( val * totalEnergyParsed * 1e6 ) / 1e6;
        return parsed >= 1e-4 ? parsed : 0.0;
      } );
      std::transform( rhoProfile.begin(), rhoProfile.end(), rhoProfile.begin(), [&]( auto& val ) {
        auto parsed = std::round( val * totalEnergyParsed * 1e6 ) / 1e6;
        return parsed >= 1e-4 ? parsed : 0.0;
      } );
      std::transform( phiProfile.begin(), phiProfile.end(), phiProfile.begin(), [&]( auto& val ) {
        auto parsed = std::round( val * totalEnergyParsed * 1e6 ) / 1e6;
        return parsed >= 1e-4 ? parsed : 0.0;
      } );

      std::vector<float> zCumProb;
      std::vector<float> rhoCumProb;
      std::vector<float> phiCumProb;
      std::vector<float> eCumProb;
      zCumProb.reserve( zProfile.size() );
      rhoCumProb.reserve( rhoProfile.size() );
      phiCumProb.reserve( phiProfile.size() );
      eCumProb.reserve( eProfile.size() );

      generateCumulativeProbabilities( zProfile, zCumProb );
      generateCumulativeProbabilities( rhoProfile, rhoCumProb );
      generateCumulativeProbabilities( phiProfile, phiCumProb );
      generateCumulativeProbabilities( eProfile, eCumProb );

      float energySum = 0.0;
      for ( size_t i = 0; i < size_t( totalHitsNoParsed * m_hitsNoOverflow ); i++ ) {
        int   zID     = sample( zCumProb );
        int   rhoID   = sample( rhoCumProb );
        int   phiID   = sample( phiCumProb );
        int   eID     = sample( eCumProb );
        float randBin = flatGenerator();
        auto  energy  = std::pow( 10.0, randBin * m_energyBinSize + m_energyBins[eID] );
        energies[rhoID * m_meshNumber.y() * m_meshNumber.z() + phiID * m_meshNumber.z() + zID] += energy;
        energySum += energy;
        if ( energySum > totalEnergyParsed ) break;
      }
    }
  };
} // namespace Gsino::CaloChallenge

#ifdef GSINO_USE_ONNXRUNTIME
namespace Gsino::CaloChallenge {
  using VAEWithProfilesONNXModel = VAEWithProfilesModel<ML::ONNX::ModelServer>;
  DECLARE_COMPONENT_WITH_ID( VAEWithProfilesONNXModel, "VAEWithProfilesONNXModel" )
} // namespace Gsino::CaloChallenge
#endif

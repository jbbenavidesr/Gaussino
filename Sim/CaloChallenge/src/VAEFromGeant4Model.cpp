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

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SystemOfUnits.h"

// Gaussino
#include "GaussinoMLBase/IModelServerSvc.h"
#include "IMLModel.h"
#ifdef GSINO_USE_TORCH
#  include "GaussinoTorch/TorchModelServer.h"
#endif
#ifdef GSINO_USE_ONNXRUNTIME
#  include "GaussinoONNX/ONNXModelServer.h"
#endif

namespace Gsino::CaloChallenge {
  /**
   * @brief A tool that implements the IMLModel interface to fill showers using a VAE model from Geant4.
   *
   * This tool uses a machine learning model server to generate shower energies based on latent vectors
   * and conditions such as particle energy and angle.
   *
   * @tparam TModelServer The type of the model server (e.g., Torch or ONNX).
   * @author Michał Mazurek
   * @date 2023
   */
  template <class TModelServer>
  class VAEFromGeant4Model : public extends<GaudiTool, IMLModel> {
    static_assert( std::is_base_of<ML::ModelServerBase, TModelServer>::value );

    Gaudi::Property<size_t> m_latentVectorSize{ this, "LatentVectorSize", 10 };
    Gaudi::Property<float>  m_maxEnergy{ this, "MaxEnergy", 1024000.0 * Gaudi::Units::MeV };
    Gaudi::Property<float>  m_maxAngle{ this, "MaxAngle", 90.0 * Gaudi::Units::degree };

  public:
    using extends::extends;

    ServiceHandle<ML::IModelServerSvc<TModelServer>> m_serverSvc{ this, "ModelServerSvc", "" };

    virtual void fillShower( std::vector<float>& energies, CLHEP::HepRandomEngine* engine, float particleEnergy,
                             int /* pid */, float theta, float /* phi */ ) const override {
      CLHEP::RandFlat    flatGenerator{ *engine, 0, 1 };
      std::vector<float> latentVector( m_latentVectorSize.value() );
      std::generate( latentVector.begin(), latentVector.end(), [&]() { return flatGenerator(); } );

      // Vector of condition (comments as in Geant4/Par04)
      // this is application specific it depdens on what the model was condition on
      // and it depends on how the condition values were encoded at the training time
      // in this example the energy of each particle is normlaized to the highest
      // energy in the considered range (1GeV-500GeV)
      // the angle is also is normlaized to the highest angle in the considered range
      // (0-90 in dergrees)
      // the model in this example was trained on two detector geometries PBW04
      // and SiW  a one hot encoding vector is used to represent the geometry with
      // [0,1] for PBW04 and [1,0] for SiW
      // INFERENCE
      // TODO: support batching!!! (this depends on the model though...)

      latentVector.push_back( particleEnergy / m_maxEnergy.value() );
      latentVector.push_back( theta / m_maxAngle.value() );
      latentVector.push_back( 0 );
      latentVector.push_back( 1 );
      energies = m_serverSvc->getServer()->template evaluate<std::vector<float>>( latentVector );
      return;
    }
  };
} // namespace Gsino::CaloChallenge

#ifdef GSINO_USE_TORCH
namespace Gsino::CaloChallenge {
  using VAEFromGeant4TorchModel = VAEFromGeant4Model<ML::Torch::ModelServer>;
  DECLARE_COMPONENT_WITH_ID( VAEFromGeant4TorchModel, "VAEFromGeant4TorchModel" )
} // namespace Gsino::CaloChallenge
#endif

#ifdef GSINO_USE_ONNXRUNTIME
namespace Gsino::CaloChallenge {
  using VAEFromGeant4ONNXModel = VAEFromGeant4Model<ML::ONNX::ModelServer>;
  DECLARE_COMPONENT_WITH_ID( VAEFromGeant4ONNXModel, "VAEFromGeant4ONNXModel" )
} // namespace Gsino::CaloChallenge
#endif

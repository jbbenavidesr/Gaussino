/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

#include "GiGaMTCoreTruth/TruthFlaggingTrackAction.h"
#include "GiGaMTCoreTruth/TruthStoringTrackAction.h"

#include "Geant4/G4UserTrackingAction.hh"

class TruthFlaggingTrackActionFAC : public extends<GiGaTool, GiGaFactoryBase<G4UserTrackingAction>> {
  using extends::extends;

  Gaudi::Property<bool>   m_storeAll{this, "StoreAll", false};
  Gaudi::Property<bool>   m_storePrimaries{this, "StorePrimaries", true};
  Gaudi::Property<bool>   m_storeDecayProducts{this, "StoreForcedDecays", true};
  Gaudi::Property<bool>   m_storeByOwnEnergy{this, "StoreByOwnEnergy", false};
  Gaudi::Property<double> m_ownEnergyThreshold{this, "OwnEnergyThreshold", 10 * CLHEP::TeV};
  Gaudi::Property<bool>   m_storeByOwnType{this, "StoreByOwnType", false};
  Gaudi::Property<bool>   m_storeByChildEnergy{this, "StoreByChildEnergy", false};
  Gaudi::Property<bool>   m_storeByChildType{this, "StoreByChildType", false};
  Gaudi::Property<double> m_childEnergyThreshold{this, "ChildEnergyThreshold", 10 * CLHEP::TeV};
  Gaudi::Property<TruthFlaggingTrackAction::TypeNames> m_ownStoredTypes{this, "StoredOwnTypes", {}};
  Gaudi::Property<TruthFlaggingTrackAction::TypeNames> m_childStoredTypesNames{this, "StoredChildTypes", {}};
  Gaudi::Property<bool>                                m_storeBySecondariesProcess{this, "StoreByChildProcess", false};
  Gaudi::Property<std::vector<std::string>>            m_childStoredProcess{this, "StoredChildProcesses", {}};
  Gaudi::Property<bool>                                m_storeByOwnProcess{this, "StoreByOwnProcess", false};
  Gaudi::Property<std::vector<std::string>>            m_ownStoredProcess{this, "StoredOwnProcesses", {}};
  Gaudi::Property<bool>                                m_storeUpToZmax{this, "StoreUpToZ", true};
  Gaudi::Property<double>                              m_zMaxToStore{this, "ZmaxForStoring", 10. * CLHEP::km};
  Gaudi::Property<double>                              m_zMaxTilt{this, "ZmaxForStoringTilt", 0. * CLHEP::degree};
  Gaudi::Property<double>                              m_zMaxYShift{this, "ZmaxForStoringYShift", 0. * CLHEP::mm};
  Gaudi::Property<bool>                                m_rejectRICHphe{this, "RejectRICHPhotoelectrons", true};
  Gaudi::Property<bool>                                m_rejectOptPhot{this, "RejectOpticalPhotons", true};

  virtual G4UserTrackingAction* construct() const override {
    auto action = new TruthFlaggingTrackAction{};
    action->SetMessageInterface( this->message_interface() );
    action->storeAll                  = m_storeAll.value();
    action->storePrimaries            = m_storePrimaries.value();
    action->storeDecayProducts        = m_storeDecayProducts.value();
    action->storeByOwnEnergy          = m_storeByOwnEnergy.value();
    action->ownEnergyThreshold        = m_ownEnergyThreshold;
    action->storeByOwnType            = m_storeByOwnType.value();
    action->storeByChildEnergy        = m_storeByChildEnergy.value();
    action->storeByChildType          = m_storeByChildType.value();
    action->childEnergyThreshold      = m_childEnergyThreshold;
    action->childStoredTypesNames     = m_childStoredTypesNames;
    action->storeBySecondariesProcess = m_storeBySecondariesProcess.value();
    action->childStoredProcess.insert( std::begin( m_childStoredProcess ), std::end( m_childStoredProcess ) );
    action->storeByOwnProcess = m_storeByOwnProcess.value();
    action->ownStoredProcess.insert( std::begin( m_ownStoredProcess ), std::end( m_ownStoredProcess ) );
    action->storeUpToZmax = m_storeUpToZmax.value();
    if ( m_storeUpToZmax.value() && m_zMaxToStore.value() == 10. * CLHEP::km ) {
      warning() << "StoreUpToZmax activated, but used with the default ZmaxForStoring = 10 km" << endmsg;
    }
    action->zMaxToStore   = m_zMaxToStore.value();
    action->zMaxTilt   = m_zMaxTilt.value();
    action->zMaxYShift   = m_zMaxYShift.value();
    action->rejectRICHphe = m_rejectRICHphe.value();
    action->rejectOptPhot = m_rejectOptPhot.value();
    return action;
  }
};

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

class TruthStoringTrackActionFAC : public extends<GiGaTool, GiGaFactoryBase<G4UserTrackingAction>> {
  Gaudi::Property<bool>                                m_endvertices{this, "AddEndVertices", true};
  using extends::extends;
  StatusCode initialize() override {
    auto sc = extends::initialize();
    // Manually retrieve here to avoid race condition as construct() can be called concurrently
    sc &= m_ppSvc.retrieve();
    return sc;
  }

  virtual G4UserTrackingAction* construct() const override {
    auto action = new TruthStoringTrackAction{};
    action->SetMessageInterface( this->message_interface() );
    action->m_fNameToID = [&]( const std::string& name ) -> std::optional<int> {
      auto pid = m_ppSvc->find( name );
      if ( pid ) { return pid->pdgID().pid(); }
      return std::nullopt;
    };
    action->addEndVertices = m_endvertices.value();
    return action;
  }

private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
};

DECLARE_COMPONENT_WITH_ID( TruthFlaggingTrackActionFAC, "TruthFlaggingTrackAction" )
DECLARE_COMPONENT_WITH_ID( TruthStoringTrackActionFAC, "TruthStoringTrackAction" )

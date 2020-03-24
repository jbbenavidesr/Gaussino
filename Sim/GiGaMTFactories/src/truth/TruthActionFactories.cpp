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
  Gaudi::Property<double>                              m_zMaxToStore{this, "ZmaxForStoring", 12280 * CLHEP::mm};
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
    action->zMaxToStore   = m_zMaxToStore;
    action->rejectRICHphe = m_rejectRICHphe.value();
    action->rejectOptPhot = m_rejectOptPhot.value();
    return action;
  }
};

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

class TruthStoringTrackActionFAC : public extends<GiGaTool, GiGaFactoryBase<G4UserTrackingAction>> {
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
    return action;
  }

private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
};

DECLARE_COMPONENT_WITH_ID( TruthFlaggingTrackActionFAC, "TruthFlaggingTrackAction" )
DECLARE_COMPONENT_WITH_ID( TruthStoringTrackActionFAC, "TruthStoringTrackAction" )

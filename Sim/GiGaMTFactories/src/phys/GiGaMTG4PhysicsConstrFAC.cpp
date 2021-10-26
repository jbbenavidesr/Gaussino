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
// Include files

#include "GiGaMTFactories/GiGaMTG4PhysicsConstrFAC.h"

// Geant4 physics lists
#include "Geant4/G4DecayPhysics.hh"

// EM physics
#include "Geant4/G4EmExtraPhysics.hh"
#include "Geant4/G4EmStandardPhysics.hh"
#include "Geant4/G4EmStandardPhysics_option1.hh"
#include "Geant4/G4EmStandardPhysics_option2.hh"
#include "Geant4/G4EmStandardPhysics_option3.hh"

// Ion and hadrons
#include "Geant4/G4HadronElasticPhysics.hh"
#include "Geant4/G4HadronElasticPhysicsHP.hh"
#include "Geant4/G4IonPhysics.hh"
#include "Geant4/G4NeutronTrackingCut.hh"
#include "Geant4/G4StoppingPhysics.hh"

#include "Geant4/G4HadronPhysicsQGSP_BERT.hh"
#include "Geant4/G4HadronPhysicsQGSP_BERT_HP.hh"
#include "Geant4/G4HadronPhysicsQGSP_FTFP_BERT.hh"

// FTFP hadrons
#include "Geant4/G4HadronPhysicsFTFP_BERT.hh"
#include "Geant4/G4HadronPhysicsFTFP_BERT_HP.hh"

typedef GiGaMTG4PhysicsConstrFAC<G4DecayPhysics> GiGaMT_G4DecayPhysics;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4DecayPhysics, "GiGaMT_G4DecayPhysics" )

typedef GiGaMTG4PhysicsConstrFAC<G4EmStandardPhysics_option1> GiGaMT_G4EmStandardPhysics_option1;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4EmStandardPhysics_option1, "GiGaMT_G4EmStandardPhysics_option1" )
typedef GiGaMTG4PhysicsConstrFAC<G4EmStandardPhysics_option2> GiGaMT_G4EmStandardPhysics_option2;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4EmStandardPhysics_option2, "GiGaMT_G4EmStandardPhysics_option2" )
typedef GiGaMTG4PhysicsConstrFAC<G4EmStandardPhysics_option3> GiGaMT_G4EmStandardPhysics_option3;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4EmStandardPhysics_option3, "GiGaMT_G4EmStandardPhysics_option3" )
typedef GiGaMTG4PhysicsConstrFAC<G4EmStandardPhysics> GiGaMT_G4EmStandardPhysics;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4EmStandardPhysics, "GiGaMT_G4EmStandardPhysics" )
typedef GiGaMTG4PhysicsConstrFAC<G4EmExtraPhysics> GiGaMT_G4EmExtraPhysics;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4EmExtraPhysics, "GiGaMT_G4EmExtraPhysics" )

typedef GiGaMTG4PhysicsConstrFAC<G4IonPhysics> GiGaMT_G4IonPhysics;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4IonPhysics, "GiGaMT_G4IonPhysics" )

template <typename PhysConstr>
class GiGaMTG4PhysicsConstrFAC<PhysConstr,
                               typename std::enable_if<std::is_same<PhysConstr, G4StoppingPhysics>::value,
                                                       PhysConstr>::type>
    : public extends<GiGaMTPhysConstr, GiGaFactoryBase<G4VPhysicsConstructor>>
{
  Gaudi::Property<bool> m_useMuonMinusCapturetool{this, "UseMuonMinusCapture", true,
                          "Parameter 'UseMuonMinusCapture' for the constructor of G4StoppingPhysics"};

public:
  using extends::extends;
  PhysConstr* construct() const override
  {
    auto tmp = new PhysConstr{name(), verbosity(), m_useMuonMinusCapturetool.value()};
    return tmp;
  }
};
typedef GiGaMTG4PhysicsConstrFAC<G4StoppingPhysics> GiGaMT_G4StoppingPhysics;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4StoppingPhysics, "GiGaMT_G4StoppingPhysics" )
typedef GiGaMTG4PhysicsConstrFAC<G4HadronElasticPhysics> GiGaMT_G4HadronElasticPhysics;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronElasticPhysics, "GiGaMT_G4HadronElasticPhysics" )
typedef GiGaMTG4PhysicsConstrFAC<G4HadronElasticPhysicsHP> GiGaMT_G4HadronElasticPhysicsHP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronElasticPhysicsHP, "GiGaMT_G4HadronElasticPhysicsHP" )
typedef GiGaMTG4PhysicsConstrFAC<G4NeutronTrackingCut> GiGaMT_G4NeutronTrackingCut;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4NeutronTrackingCut, "GiGaMT_G4NeutronTrackingCut" )

// Specialization for those HadronPhysics lists with quasi elastic option
template <typename PhysConstr>
class GiGaMTG4PhysicsConstrFAC<PhysConstr,
                               typename std::enable_if<std::is_same<PhysConstr, G4HadronPhysicsQGSP_BERT>::value ||
                                                       std::is_same<PhysConstr, G4HadronPhysicsQGSP_BERT_HP>::value ||
                                                       std::is_same<PhysConstr, G4HadronPhysicsQGSP_FTFP_BERT>::value ||
                                                       std::is_same<PhysConstr, G4HadronPhysicsFTFP_BERT>::value ||
                                                       std::is_same<PhysConstr, G4HadronPhysicsFTFP_BERT_HP>::value,
                                                       PhysConstr>::type>
    : public extends<GiGaMTPhysConstr, GiGaFactoryBase<G4VPhysicsConstructor>>
{
  Gaudi::Property<bool> m_quasiElastic{this, "QuasiElastic", true,
                                       "Parameter 'quasiElastic' for the constructor of HadronPhysicsQGSP_BERT"};

public:
  using extends::extends;
  PhysConstr* construct() const override
  {
    auto tmp = new PhysConstr{name(), m_quasiElastic.value()};
    tmp->SetVerboseLevel( verbosity() );
    return tmp;
  }
};

typedef GiGaMTG4PhysicsConstrFAC<G4HadronPhysicsQGSP_BERT> GiGaMT_G4HadronPhysicsQGSP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronPhysicsQGSP_BERT, "GiGaMT_G4HadronPhysicsQGSP_BERT" )
typedef GiGaMTG4PhysicsConstrFAC<G4HadronPhysicsQGSP_BERT_HP> GiGaMT_G4HadronPhysicsQGSP_BERT_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronPhysicsQGSP_BERT_HP, "GiGaMT_G4HadronPhysicsQGSP_BERT_HP" )
typedef GiGaMTG4PhysicsConstrFAC<G4HadronPhysicsQGSP_FTFP_BERT> GiGaMT_G4HadronPhysicsQGSP_FTFP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronPhysicsQGSP_FTFP_BERT, "GiGaMT_G4HadronPhysicsQGSP_FTFP_BERT" )
typedef GiGaMTG4PhysicsConstrFAC<G4HadronPhysicsFTFP_BERT> GiGaMT_G4HadronPhysicsFTFP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronPhysicsFTFP_BERT, "GiGaMT_G4HadronPhysicsFTFP_BERT" )
typedef GiGaMTG4PhysicsConstrFAC<G4HadronPhysicsFTFP_BERT_HP> GiGaMT_G4HadronPhysicsFTFP_BERT_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HadronPhysicsFTFP_BERT_HP, "GiGaMT_G4HadronPhysicsFTFP_BERT_HP" )

typedef GiGaMTG4PhysicsConstrFAC<G4NeutronTrackingCut> GiGaMT_G4NeutronTrackingCut;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4NeutronTrackingCut, "GiGaMT_G4NeutronTrackingCut" )

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//---------------------------------------------------------------------------
//
// ClassName:   G4EmStandardPhysics_option2_AdePT
//
// Author:      S.Diederichs 08.08.2025
//
//
//----------------------------------------------------------------------------
//

#include "G4EmStandardPhysics_option2_AdePT.hh"

#include "G4HepEmConfig.hh"
#include "G4HepEmParameters.hh"

#include <AdePT/core/AdePTConfiguration.hh>
#include <AdePT/integration/AdePTTrackingManager.hh>

#include "G4EmBuilder.hh"
#include "G4EmParameters.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4KleinNishinaModel.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"

#include "G4CoulombScattering.hh"
#include "G4UrbanMscModel.hh"
#include "G4WentzelVIModel.hh"
#include "G4eCoulombScatteringModel.hh"
#include "G4eMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4Generator2BS.hh"
#include "G4SeltzerBergerModel.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eBremsstrahlungRelModel.hh"
#include "G4eIonisation.hh"
#include "G4eplusAnnihilation.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4GenericIon.hh"
#include "G4ParticleTable.hh"
#include "G4Positron.hh"

#include "G4BuilderType.hh"
#include "G4EmModelActivator.hh"
#include "G4GammaGeneralProcess.hh"
#include "G4PhysicsListHelper.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY( G4EmStandardPhysics_option2_AdePT );

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics_option2_AdePT::G4EmStandardPhysics_option2_AdePT( G4int ver, const G4String& )
    : G4VPhysicsConstructor( "G4EmStandard_opt2_HepEm" ), verbose( ver ) {
  G4EmParameters* param = G4EmParameters::Instance();
  param->SetDefaults();
  param->SetVerbose( verbose );
  param->SetApplyCuts( true );
  param->SetStepFunction( 0.8, 1 * CLHEP::mm );
  param->SetMscRangeFactor( 0.2 );
  param->SetLateralDisplacement( false );
  param->SetMscStepLimitType( fMinimal );
  param->SetLowestElectronEnergy( 1. );
  SetPhysicsType( bElectromagnetic );

  fAdePTConfiguration = new AdePTConfiguration();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics_option2_AdePT::~G4EmStandardPhysics_option2_AdePT() { delete fAdePTConfiguration; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4EmStandardPhysics_option2_AdePT::ConstructParticle() {
  // minimal set of particles for EM physics
  G4EmBuilder::ConstructMinimalEmSet();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4EmStandardPhysics_option2_AdePT::ConstructProcess() {
  if ( verbose > 1 ) { G4cout << "### " << GetPhysicsName() << " Construct Processes " << G4endl; }
  G4EmBuilder::PrepareEMPhysics();

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // processes used by several particles
  G4hMultipleScattering* hmsc = new G4hMultipleScattering( "ionmsc" );
  G4NuclearStopping*     pnuc( nullptr );

  // high energy limit for e+- scattering models and bremsstrahlung
  G4double highEnergyLimit = G4EmParameters::Instance()->MscEnergyLimit();

  // Add gamma EM Processes
  G4ParticleDefinition* particle = G4Gamma::Gamma();

  G4PhotoElectricEffect* pee = new G4PhotoElectricEffect();
  pee->SetEmModel( new G4LivermorePhotoElectricModel() );

  if ( G4EmParameters::Instance()->GeneralProcessActive() ) {
    G4GammaGeneralProcess* sp = new G4GammaGeneralProcess();
    sp->AddEmProcess( pee );
    sp->AddEmProcess( new G4ComptonScattering() );
    sp->AddEmProcess( new G4GammaConversion() );
    G4LossTableManager::Instance()->SetGammaGeneralProcess( sp );
    ph->RegisterProcess( sp, particle );

  } else {
    ph->RegisterProcess( pee, particle );
    ph->RegisterProcess( new G4ComptonScattering(), particle );
    ph->RegisterProcess( new G4GammaConversion(), particle );
  }

  // e-
  particle = G4Electron::Electron();

  G4eIonisation* eioni = new G4eIonisation();

  G4eMultipleScattering* msc  = new G4eMultipleScattering;
  G4UrbanMscModel*       msc1 = new G4UrbanMscModel();
  G4WentzelVIModel*      msc2 = new G4WentzelVIModel();
  msc1->SetHighEnergyLimit( highEnergyLimit );
  msc2->SetLowEnergyLimit( highEnergyLimit );
  msc->SetEmModel( msc1 );
  msc->SetEmModel( msc2 );

  G4eCoulombScatteringModel* ssm = new G4eCoulombScatteringModel();
  G4CoulombScattering*       ss  = new G4CoulombScattering();
  ss->SetEmModel( ssm );
  ss->SetMinKinEnergy( highEnergyLimit );
  ssm->SetLowEnergyLimit( highEnergyLimit );
  ssm->SetActivationLowEnergyLimit( highEnergyLimit );

  ph->RegisterProcess( msc, particle );
  ph->RegisterProcess( eioni, particle );
  ph->RegisterProcess( new G4eBremsstrahlung(), particle );
  ph->RegisterProcess( ss, particle );

  // e+
  particle = G4Positron::Positron();
  eioni    = new G4eIonisation();

  msc  = new G4eMultipleScattering;
  msc1 = new G4UrbanMscModel();
  msc2 = new G4WentzelVIModel();
  msc1->SetHighEnergyLimit( highEnergyLimit );
  msc2->SetLowEnergyLimit( highEnergyLimit );
  msc->SetEmModel( msc1 );
  msc->SetEmModel( msc2 );

  ssm = new G4eCoulombScatteringModel();
  ss  = new G4CoulombScattering();
  ss->SetEmModel( ssm );
  ss->SetMinKinEnergy( highEnergyLimit );
  ssm->SetLowEnergyLimit( highEnergyLimit );
  ssm->SetActivationLowEnergyLimit( highEnergyLimit );

  ph->RegisterProcess( msc, particle );
  ph->RegisterProcess( eioni, particle );
  ph->RegisterProcess( new G4eBremsstrahlung(), particle );
  ph->RegisterProcess( new G4eplusAnnihilation(), particle );
  ph->RegisterProcess( ss, particle );

  std::cout << " ### AdePT is active: using the AdePTTrackingManager for e-/e+ and gamma." << std::endl;

  // Construct the AdePT tracking manager and configure the G4HepEm TrackingManager:
  fTrackingManager = new AdePTTrackingManager( fAdePTConfiguration, /*verboseLevel*/ 0 );

  auto g4hepemconfig = fTrackingManager->GetG4HepEmConfig();
  g4hepemconfig->SetMultipleStepsInMSCWithTransportation( false );
  //   // set the continuous step limit function parameters
  //   // NOTE: this can be done only explicitly (no getters in Geant4)
  g4hepemconfig->SetEnergyLossStepLimitFunctionParameters( 0.8, 1.0 * CLHEP::mm );

  // Attach the tracking manager to e-/e+ and gamma
  G4Electron::Definition()->SetTrackingManager( fTrackingManager );
  G4Positron::Definition()->SetTrackingManager( fTrackingManager );
  G4Gamma::Definition()->SetTrackingManager( fTrackingManager );

  // generic ion
  particle                 = G4GenericIon::GenericIon();
  G4ionIonisation* ionIoni = new G4ionIonisation();
  ph->RegisterProcess( hmsc, particle );
  ph->RegisterProcess( ionIoni, particle );

  // muons, hadrons ions
  G4EmBuilder::ConstructCharged( hmsc, pnuc );

  // extra configuration
  G4EmModelActivator mact( GetPhysicsName() );

  G4EmParameters::Instance()->Dump();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

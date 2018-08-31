#include "GiGaMTModularPhysListFAC.h"
#include "Geant4/G4VModularPhysicsList.hh"
#include "GiGaMTModularPhysListFAC.h"

DECLARE_COMPONENT( GiGaMTModularPhysListFAC )

// ============================================================================
StatusCode GiGaMTModularPhysListFAC::initialize()
{
  StatusCode sc = extends::initialize();
  if ( sc.isFailure() ) {
    return Error( "Could not initialize base class!", sc );
  }

  info() << "GiGaMTModularPhysListFAC initializing" << endmsg;

  if ( m_physconstr.empty() ) {
    return Error( "Invalid/Empty list of Physics constructors" );
  }
  for ( auto & constructor: m_physconstr ) {
    auto theconstr = tool<ConstructorFactory>( constructor, this );
    if ( !theconstr ) {
      return StatusCode::FAILURE;
    }
    m_constructors.push_back(theconstr);
  }

  return StatusCode::SUCCESS;
}

G4VUserPhysicsList* GiGaMTModularPhysListFAC::construct() const
{
  auto plist = new G4VModularPhysicsList{};
  for ( auto& ctool : m_constructors ) {
    plist->RegisterPhysics( ctool->construct() );
  }
  return plist;
}

//// ============================================================================
//void GiGaMTModularPhysListFAC::SetCuts()
//{
  //MsgStream log( msgSvc(), name() );

  //// set cut values for gamma
  //SetCutValue( cutForGamma(), "gamma" );
  //log << MSG::INFO << " The production cut for gamma is set to \t" << cutForGamma() / CLHEP::mm << " mm " << endmsg;

  //// set cut values for electron
  //SetCutValue( cutForElectron(), "e-" );
  //log << MSG::INFO << " The production cut for electron is set to \t" << cutForElectron() / CLHEP::mm << " mm "
      //<< endmsg;

  //// set cut values for positron
  //SetCutValue( cutForPositron(), "e+" );
  //log << MSG::INFO << " The production cut for positron is set to \t" << cutForPositron() / CLHEP::mm << " mm "
      //<< endmsg;

  //if ( m_dumpCutsTable ) {
    //DumpCutValuesTable();
  //}
//}

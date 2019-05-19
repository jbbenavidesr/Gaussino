#include "GiGaMTModularPhysListFAC.h"
#include "Geant4/G4VModularPhysicsList.hh"
#include "GiGaMTCore/IGiGaMessage.h"

DECLARE_COMPONENT( GiGaMTModularPhysListFAC )

class GiGaMTModularPhysList : public GiGaMessage, public G4VModularPhysicsList
{
  friend class GiGaMTModularPhysListFAC;
  void SetCuts() override;

private:
  GiGaMTModularPhysList() = default;
  double cutForGamma;
  double cutForElectron;
  double cutForPositron;
  bool dumpCutsTable;
};

StatusCode GiGaMTModularPhysListFAC::initialize(){
  auto sc = extends::initialize();
  for(auto & name:m_constructorNames){
    m_constructors.push_back(tool<ConstructorFactory>(name, this));
  }
  return sc;
}

StatusCode GiGaMTModularPhysListFAC::finalize(){
  for(auto & constr:m_constructors){
    constr->release();
  }

  return extends::finalize();
}

G4VUserPhysicsList* GiGaMTModularPhysListFAC::construct() const
{
  auto plist = new GiGaMTModularPhysList{};
  plist->SetMessageInterface( message_interface() );
  plist->cutForGamma    = m_cutForGamma;
  plist->cutForElectron = m_cutForElectron;
  plist->cutForPositron = m_cutForPositron;
  plist->dumpCutsTable  = m_dumpCutsTable;
  for ( auto& ctool : m_constructors ) {
    plist->RegisterPhysics( ctool->construct() );
  }
  return plist;
}

void GiGaMTModularPhysList::SetCuts()
{
  // set cut values for gamma
  SetCutValue( cutForGamma, "gamma" );
  std::stringstream message;
  message << "The production cut for gamma is set to \t" << cutForGamma / CLHEP::mm << " mm ";
  info( message.str() );

  // set cut values for electron
  SetCutValue( cutForElectron, "e-" );
  message.str("");
  message.clear();
  message << " The production cut for electron is set to \t" << cutForElectron / CLHEP::mm << " mm ";
  info( message.str() );

  // set cut values for positron
  SetCutValue( cutForPositron, "e+" );
  message.str("");
  message.clear();
  message << " The production cut for positron is set to \t" << cutForPositron / CLHEP::mm << " mm ";
  info( message.str() );

  if ( dumpCutsTable ) {
    DumpCutValuesTable();
  }
}

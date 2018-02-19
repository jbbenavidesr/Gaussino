// $Id: GiGaPhysListModular.cpp,v 1.18 2008-10-08 16:39:09 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"

// GiGa
//#include "GiGa/GiGaMACROs.h"
#include "GiGa/IGiGaPhysicsConstructor.h"

// G4 
#include "Geant4/G4ParticleTypes.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ParticleWithCuts.hh"
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4ParticleTypes.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4ios.hh"

// local
#include "GiGaPhysListModular.h"

/** @file 
 * 
 *  implementation of class GiGaPhysListModular
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysListModular )

// ============================================================================
/** standard constructor 
 *  @see GiGaPhysListBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaPhysListModular::GiGaPhysListModular
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : G4VModularPhysicsList(),
    GiGaPhysListBase( type , name , parent ),
    m_dumpCutsTable(false)
{
  declareProperty( "PhysicsConstructors"  , m_physconstr);
  declareProperty( "DumpCutsTable", m_dumpCutsTable);
}


// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysListModular::~GiGaPhysListModular()
{
  m_physconstr.clear();
}


// ============================================================================
StatusCode GiGaPhysListModular::initialize() 
{
  StatusCode sc=GiGaPhysListBase::initialize ();
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaPhysListBase class!", sc); }
  
  info() << "GiGaPhysListModular initializing" << endmsg;
  
  if( m_physconstr.empty() ) 
    { return Error ( "Invalid/Empty list of Physics constructors" ) ; }
  
  for ( std::vector<std::string>::iterator constructor = m_physconstr.begin() ; 
        m_physconstr.end() != constructor ; ++constructor )
  {
    IGiGaPhysicsConstructor* theconstr = 
      tool<IGiGaPhysicsConstructor>( *constructor , this ) ;
    if( 0 == theconstr ) { return StatusCode::FAILURE ; }
    
    if( 0 == theconstr -> physicsConstructor() ) 
    { return Error ( "G4PhysicsConstructor* points to NULL!" ) ; }
    
    m_constructors.push_back( theconstr );
    
    // register 
    RegisterPhysics( theconstr -> physicsConstructor() ) ;
    
    // Print name of physics constructors registered
    info() << "Registered " << theconstr->name() << endmsg;

  }
  
  return StatusCode::SUCCESS;
}

// ============================================================================
StatusCode GiGaPhysListModular::finalize () {
  // reset G4 vector of physics lists (they have been destoyed by the Gaudi
  // tool release methods
  // physicsVector->clear();

  // In G4v10+ physicsVector has been moved to a data-encapsulation class
  // G4VMPLData which should be accessed via a G4VMPLManager
  GetSubInstanceManager().offset[GetInstanceID()].physicsVector->clear();

  return GiGaPhysListBase::finalize();
}


// ============================================================================
void GiGaPhysListModular::SetCuts()
{
  MsgStream log( msgSvc() , name() );

  // set cut values for gamma
  SetCutValue ( cutForGamma       () , "gamma"        ) ;
  log << MSG::INFO
      << " The production cut for gamma is set to \t"
      << cutForGamma       () / CLHEP::mm << " mm " << endmsg ;

  // set cut values for electron
  SetCutValue ( cutForElectron    () , "e-"           ) ;
  log << MSG::INFO
      << " The production cut for electron is set to \t"
      << cutForElectron    () / CLHEP::mm << " mm " << endmsg ;

  // set cut values for positron
  SetCutValue ( cutForPositron    () , "e+"           ) ;
  log << MSG::INFO
      << " The production cut for positron is set to \t"
      << cutForPositron    () / CLHEP::mm << " mm " << endmsg ;

  if (m_dumpCutsTable) { DumpCutValuesTable(); }
}


// ============================================================================
// The END
// ============================================================================

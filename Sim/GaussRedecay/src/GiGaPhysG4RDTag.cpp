// $Id: GiGaPhysG4RDTag.cpp,v 1.6 2009-10-20 07:35:07 marcin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/PhysicalConstants.h"

// G4 
#include "G4UnknownParticle.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"

// local
#include "G4RDTag.h"

//
#include "GiGaPhysG4RDTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaPhysG4RDTag
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysG4RDTag )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysG4RDTag::GiGaPhysG4RDTag
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ) 
{
    declareProperty("G4Reserve", m_g4_reserve = 100,
                    "Number of tag particles registerred to Geant4.");
    declareProperty("InitialPlaceholder", m_initial_placeholder=424242,
                    "Initial PDG ID of the placeholder particle");
}

//=============================================================================
// Destructor
//=============================================================================
GiGaPhysG4RDTag::~GiGaPhysG4RDTag(){}

//=============================================================================
// ConstructParticle
//=============================================================================
void GiGaPhysG4RDTag::ConstructParticle()
{
    G4ParticleTable::GetParticleTable()->SetVerboseLevel(1);
    for (int i = 0; i < m_g4_reserve; i++) {
        int id = m_initial_placeholder + i;
        auto ret = G4RDTag::Definition(id);
        if (!ret) {
            error() << "Could not register tag particle in Geant4 with ID: "
                    << id << endmsg;
        }
        if ( msgLevel( MSG::DEBUG) ) {
            debug() << "Added placeholder " << id << endmsg;
        }
    }
}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysG4RDTag::ConstructProcess()
{
    G4ParticleTable::GetParticleTable()->SetVerboseLevel(1);
  theParticleIterator -> reset() ;
  while ( (*theParticleIterator)() ) {
    G4ParticleDefinition * particle = theParticleIterator -> value() ;
    if ( msgLevel( MSG::DEBUG) ) {
    debug() << "RDTagList ConstructProcess(): " << particle ->GetPDGEncoding() << ", " << particle->GetParticleName()  
            << endmsg;
    }
    G4ProcessManager     * pmanager = particle -> GetProcessManager() ;
    if ( particle -> GetParticleName() == "unknown" ) {
      pmanager -> AddProcess( &m_unknownDecay ) ;
      pmanager -> SetProcessOrdering( &m_unknownDecay , idxPostStep ) ;
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << "RDTagList ConstructProcess(): Unknown Decays for " << particle -> GetParticleName() 
              << endmsg;
        pmanager -> DumpInfo() ;
      }
    }
  }
}

//=============================================================================

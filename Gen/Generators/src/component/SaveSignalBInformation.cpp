#include "SaveSignalBInformation.h"

#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/VertexAttribute.h"
#include "HepMCUtils/HepMCUtils.h"
#include "HepMC3/Relatives.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SaveSignalBInformation
//
// 2013-06-26 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_COMPONENT( SaveSignalBInformation )

//=============================================================================
// Main execution
//=============================================================================
std::vector<HepMC3::GenEvent> SaveSignalBInformation::operator()( const std::vector<HepMC3::GenEvent>& hepmcevents ) const
{
  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

  std::vector<HepMC3::GenEvent> outputevents;
  for ( auto& evt : hepmcevents ) {
    // check if signal exists
    auto sig_proc_vtx =
        evt.attribute<HepMC3::VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex )->value();
    if ( sig_proc_vtx ) {
      auto ret = extractSignal( sig_proc_vtx );
      if ( ret ) {
        outputevents.push_back( std::move( *ret ) );
      }
    }
  }

  return outputevents;
}

//=============================================================================
// Extract B string and copy to a new location
//=============================================================================
HepMC3::GenEvent* SaveSignalBInformation::extractSignal( const HepMC3::ConstGenVertexPtr& theVertex ) const
{
  auto& HEPB0                   = *std::begin( theVertex->particles_in() );
  HepMC3::ConstGenParticlePtr Bstring = nullptr;

  // look for the string associated to the signal
  for ( auto& part : HepMC3::Relatives::ANCESTORS(HEPB0) ) {
    int genid = abs( part->pdg_id() );
    if ( ( 91 == genid ) || ( 92 == genid ) ) {
      Bstring = part;
      break;
    }
  }

  if ( Bstring ) {
    // copy the string in a new event
    auto hepmcevt = new HepMC3::GenEvent();
    hepmcevt->add_attribute( Gaussino::HepMC::Attributes::GeneratorName,
                             std::make_shared<HepMC3::StringAttribute>( "String" ) );
    // Little hack to make it thread-safe when reading later
    hepmcevt->attribute<HepMC3::StringAttribute>(Gaussino::HepMC::Attributes::GeneratorName);

    if ( 0 == Bstring->production_vertex() ) error() << "Bstring particle has no production vertex." << endmsg;

    // create a new vertex and a new HepMC Particle for the root particle
    // (a copy of which will be associated to the new HepMC event)

    HepMC3::GenVertexPtr newVertex{new HepMC3::GenVertex( Bstring->production_vertex()->position() )};

    hepmcevt->add_vertex( newVertex );

    HepMC3::GenParticlePtr theNewParticle{
        new HepMC3::GenParticle( Bstring->momentum(), Bstring->pdg_id(), Bstring->status() )};

    newVertex->add_particle_out( theNewParticle );

    // Associate the new particle to the HepMC event
    StatusCode sc = fillHepMCEvent( theNewParticle, Bstring );
    hepmcevt->add_attribute( Gaussino::HepMC::Attributes::SignalProcessVertex,
                             std::make_shared<HepMC3::VertexAttribute>( theNewParticle->production_vertex() ) );

    if ( !sc.isSuccess() ) error() << "Could not fill HepMC event for signal tree" << endmsg;

    return hepmcevt;
  }
  return nullptr;
}

//=============================================================================
// Fill HepMC event from a HepMC tree
//=============================================================================
StatusCode SaveSignalBInformation::fillHepMCEvent( HepMC3::GenParticlePtr & theNewParticle,
                                                   const HepMC3::ConstGenParticlePtr & theOldParticle ) const
{
  StatusCode sc = StatusCode::SUCCESS;
  //
  // Copy theOldParticle to theNewParticle in theEvent
  // theNewParticle already exist and is created outside this function
  auto oVertex = theOldParticle->end_vertex();
  if ( oVertex ) {
    // Create decay vertex and associate it to theNewParticle
    HepMC3::GenVertexPtr newVertex{new HepMC3::GenVertex( oVertex->position() )};
    newVertex->add_particle_in( theNewParticle );
    theNewParticle->parent_event()->add_vertex( newVertex );

    // loop over child particle of this vertex after sorting them
    std::list<HepMC3::ConstGenParticlePtr> outParticles;
    for ( auto & part : oVertex->particles_out() )
      outParticles.push_back( part );

    outParticles.sort( HepMCUtils::compareConstHepMCParticles );

    for ( auto & child : outParticles ) {

      // Create a new particle for each daughter of theOldParticle
      HepMC3::GenParticlePtr newPart{new HepMC3::GenParticle( child->momentum(), child->pdg_id(), child->status() )};
      newVertex->add_particle_out( newPart );

      // Recursive call : fill the event with the daughters
      sc = fillHepMCEvent( newPart, child);

      if ( !sc.isSuccess() ) return sc;
    }
  }
  return sc;
}

#include "SaveSignalBInformation.h"

#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/VertexAttribute.h"
#include "HepMCUtils/HepMCUtils.h"

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
std::vector<HepMC::GenEvent> SaveSignalBInformation::operator()( const std::vector<HepMC::GenEvent>& hepmcevents ) const
{
  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

  std::vector<HepMC::GenEvent> outputevents;
  for ( auto& evt : hepmcevents ) {
    // check if signal exists
    auto sig_proc_vtx =
        evt.attribute<HepMC::VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex )->value();
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
HepMC::GenEvent* SaveSignalBInformation::extractSignal( const HepMC::GenVertexPtr& theVertex ) const
{
  auto& HEPB0                   = *( theVertex->particles_in_const_begin() );
  HepMC::GenParticlePtr Bstring = nullptr;

  // look for the string associated to the signal
  for ( auto& part : HEPB0->production_vertex()->particles( HepMC::ancestors ) ) {
    int genid = abs( part->pdg_id() );
    if ( ( 91 == genid ) || ( 92 == genid ) ) {
      Bstring = part;
      break;
    }
  }

  if ( Bstring ) {
    // copy the string in a new event
    auto hepmcevt = new HepMC::GenEvent();
    hepmcevt->add_attribute( Gaussino::HepMC::Attributes::GeneratorName,
                             std::make_shared<HepMC::StringAttribute>( "String" ) );
    // Little hack to make it thread-safe when reading later
    hepmcevt->attribute<HepMC::StringAttribute>(Gaussino::HepMC::Attributes::GeneratorName);

    if ( 0 == Bstring->production_vertex() ) error() << "Bstring particle has no production vertex." << endmsg;

    // create a new vertex and a new HepMC Particle for the root particle
    // (a copy of which will be associated to the new HepMC event)

    HepMC::GenVertexPtr newVertex = new HepMC::GenVertex( Bstring->production_vertex()->position() );

    hepmcevt->add_vertex( newVertex );

    HepMC::GenParticlePtr theNewParticle =
        new HepMC::GenParticle( Bstring->momentum(), Bstring->pdg_id(), Bstring->status() );

    newVertex->add_particle_out( theNewParticle );

    // Associate the new particle to the HepMC event
    StatusCode sc = fillHepMCEvent( theNewParticle, Bstring );
    hepmcevt->add_attribute( Gaussino::HepMC::Attributes::SignalProcessVertex,
                             std::make_shared<HepMC::VertexAttribute>( theNewParticle->production_vertex() ) );

    if ( !sc.isSuccess() ) error() << "Could not fill HepMC event for signal tree" << endmsg;

    return hepmcevt;
  }
  return nullptr;
}

//=============================================================================
// Fill HepMC event from a HepMC tree
//=============================================================================
StatusCode SaveSignalBInformation::fillHepMCEvent( HepMC::GenParticlePtr & theNewParticle,
                                                   const HepMC::GenParticlePtr & theOldParticle ) const
{
  StatusCode sc = StatusCode::SUCCESS;
  //
  // Copy theOldParticle to theNewParticle in theEvent
  // theNewParticle already exist and is created outside this function
  auto & oVertex = theOldParticle->end_vertex();
  if ( oVertex ) {
    // Create decay vertex and associate it to theNewParticle
    HepMC::GenVertexPtr newVertex = new HepMC::GenVertex( oVertex->position() );
    newVertex->add_particle_in( theNewParticle );
    theNewParticle->parent_event()->add_vertex( newVertex );

    // loop over child particle of this vertex after sorting them
    std::list<HepMC::GenParticlePtr> outParticles;
    for ( auto & part : oVertex->particles_out() )
      outParticles.push_back( part );

    outParticles.sort( HepMCUtils::compareHepMCParticles );

    for ( auto & child : outParticles ) {

      // Create a new particle for each daughter of theOldParticle
      HepMC::GenParticlePtr newPart =
          new HepMC::GenParticle( child->momentum(), child->pdg_id(), child->status() );
      newVertex->add_particle_out( newPart );

      // Recursive call : fill the event with the daughters
      sc = fillHepMCEvent( newPart, child);

      if ( !sc.isSuccess() ) return sc;
    }
  }
  return sc;
}

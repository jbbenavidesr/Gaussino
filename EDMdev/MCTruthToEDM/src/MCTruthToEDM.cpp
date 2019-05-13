// Local.
#include "MCTruthToEDM.h"
#include "MCInterfaces/IFlagSignalChain.h"

// Gaudi.
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Kernel.
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include "range/v3/all.hpp"

#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/Status.h"
#include "HepMCUser/VertexAttribute.h"

DECLARE_COMPONENT( MCTruthToEDM )

LHCb::MCVertex::MCVertexType vertexType( int id )
{
  LHCb::MCVertex::MCVertexType vType;
  switch ( id ) {
  case LHCb::MCVertex::ppCollision:
    vType = LHCb::MCVertex::ppCollision;
    break;
  case LHCb::MCVertex::DecayVertex:
    vType = LHCb::MCVertex::DecayVertex;
    break;
  case LHCb::MCVertex::OscillatedAndDecay:
    vType = LHCb::MCVertex::OscillatedAndDecay;
    break;
  case LHCb::MCVertex::HadronicInteraction:
    vType = LHCb::MCVertex::HadronicInteraction;
    break;
  case LHCb::MCVertex::Bremsstrahlung:
    vType = LHCb::MCVertex::Bremsstrahlung;
    break;
  case LHCb::MCVertex::PairProduction:
    vType = LHCb::MCVertex::PairProduction;
    break;
  case LHCb::MCVertex::Compton:
    vType = LHCb::MCVertex::Compton;
    break;
  case LHCb::MCVertex::DeltaRay:
    vType = LHCb::MCVertex::DeltaRay;
    break;
  case LHCb::MCVertex::PhotoElectric:
    vType = LHCb::MCVertex::PhotoElectric;
    break;
  case LHCb::MCVertex::Annihilation:
    vType = LHCb::MCVertex::Annihilation;
    break;
  case LHCb::MCVertex::RICHPhotoElectric:
    vType = LHCb::MCVertex::RICHPhotoElectric;
    break;
  case LHCb::MCVertex::Cerenkov:
    vType = LHCb::MCVertex::Cerenkov;
    break;
  case LHCb::MCVertex::LHCHalo:
    vType = LHCb::MCVertex::LHCHalo;
    break;
  case LHCb::MCVertex::MuonBackground:
    vType = LHCb::MCVertex::MuonBackground;
    break;
  case LHCb::MCVertex::MuonBackgroundFlat:
    vType = LHCb::MCVertex::MuonBackgroundFlat;
    break;
  case LHCb::MCVertex::MuonBackgroundSpillover:
    vType = LHCb::MCVertex::MuonBackgroundSpillover;
    break;
  default:
    vType = LHCb::MCVertex::Unknown;
  }

  return vType;
}

std::tuple<LHCb::MCParticles, LHCb::MCVertices, LHCb::MCHeader, LinkedParticleMCParticleLinks> MCTruthToEDM::
operator()( const Gaussino::MCTruthPtrs& mctruths ) const
{
  // Create containers in TES for MCParticles and MCVertices.
  LHCb::MCParticles m_particleContainer;
  LHCb::MCVertices m_vertexContainer;
  LinkedParticleMCParticleLinks particle_links;
  size_t n_LinkedParticles{0};
  size_t n_LinkedVertices{0};
  for ( auto& mctruth : mctruths ) {
    n_LinkedParticles += mctruth->GetNParticles();
    n_LinkedVertices += mctruth->GetNVertices();
  }

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Reserving space for " << n_LinkedParticles << "(" << n_LinkedVertices << ") MCParticles(MCVertices)"
            << endmsg;
  }

  m_particleContainer.reserve( n_LinkedParticles );
  m_vertexContainer.reserve( n_LinkedVertices );
  Converter converter{m_particleContainer, m_vertexContainer, particle_links, msgStream()};

  // Create some MCHeader.
  LHCb::MCHeader mcHeader;
  // Create a set to store a pointer to createed primary vertices.
  // This will be searched for every root particle to assign them to the
  // same vertex if necessary
  VertexSet pvs;

  // Loop over the MCTruth objects
  for ( auto& mt : mctruths ) {

    for ( auto rp : mt->GetRootParticles() ) {
      LHCb::MCVertex* primary{nullptr};
      auto foundpv = FindPV( rp, pvs );
      // Attach to the found vertex, if not create a new one
      if ( foundpv ) {
        primary = foundpv;
      } else {
        primary = converter.createVertex( rp->GetOriginPosition() );
        primary->setType( LHCb::MCVertex::ppCollision );
        mcHeader.addToPrimaryVertices( primary );
        // Lastly, add the vertex to the set to be found later
        pvs.insert( std::make_pair( primary, rp->HepMC() ? rp->HepMC()->production_vertex().get() : nullptr ) );
      }
      // Now recursively convert everything
      converter.convert( rp, primary );
    }
  }

  return std::make_tuple( std::move( m_particleContainer ), std::move( m_vertexContainer ), mcHeader,
                          std::move( particle_links ) );
}

//=============================================================================
// Convert a decay tree into MCParticle or to G4PrimaryParticle.
//=============================================================================

LHCb::MCVertex* MCTruthToEDM::FindPV( LinkedParticle* lp, VertexSet& pvs ) const
{

  auto essentiallyEqual = []( float a, float b, float epsilon = 0.00001 ) {
    return fabs( a - b ) <= ( ( fabs( a ) > fabs( b ) ? fabs( b ) : fabs( a ) ) * epsilon );
  };

  // Vertex finder for the PV of a root particle
  auto finder_hepmc = [&]( VertexSet::key_type p ) {
    if ( lp->HepMC() && lp->HepMC()->production_vertex().get() == p.second ) {
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Found PV for " << *lp << " via HepMC production vertex" << endmsg;
      }
      return true;
    }
    return false;
  };
  auto finder_position = [&]( VertexSet::key_type p ) {
    if ( essentiallyEqual( p.first->position().x(), lp->GetOriginPosition().x() ) &&
         essentiallyEqual( p.first->position().y(), lp->GetOriginPosition().y() ) &&
         essentiallyEqual( p.first->position().z(), lp->GetOriginPosition().z() ) &&
         essentiallyEqual( p.first->time(), lp->GetOriginPosition().t() ) ) {
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Found PV for " << *lp << " via position comparison vertex" << endmsg;
      }
      return true;
    }
    return false;
  };
  auto found = std::find_if( std::begin( pvs ), std::end( pvs ), finder_hepmc );
  if ( found != std::end( pvs ) ) {
    return found->first;
  }
  found = std::find_if( std::begin( pvs ), std::end( pvs ), finder_position );
  if ( found != std::end( pvs ) ) {
    return found->first;
  }
  return nullptr;
}

LHCb::MCVertex* MCTruthToEDM::Converter::createVertex( const HepMC3::FourVector& fm )
{
  auto ret = new LHCb::MCVertex{};
  ret->setPosition( Gaudi::XYZPoint{fm.x(), fm.y(), fm.z()} );
  ret->setTime( fm.t() );
  m_vertices.insert( ret );

  return ret;
}

LHCb::MCVertex* MCTruthToEDM::Converter::createVertex( LinkedVertex* lv )
{
  auto fm  = lv->GetPosition();
  auto ret = createVertex( fm );
  already_converted.insert( lv );
  // Check if this vertex has an associated hepmc vertex, if it does it was
  // produced during the generation phase and we set sensible vertex types
  auto hepmc_endvertex = lv->hepmc_vtx;
  if ( hepmc_endvertex ) {
    // Check if a single particle went in and oscillated
    auto parent = *std::begin( lv->incoming_particle );

    if ( parent->HasOscillated() && lv->incoming_particle.size() == 1 ) {
      ret->setType( LHCb::MCVertex::OscillatedAndDecay );
    } else if ( ( 4 == std::abs( parent->GetPDG() ) ) || ( 5 == std::abs( parent->GetPDG() ) ) )
      ret->setType( LHCb::MCVertex::StringFragmentation );
    else
      ret->setType( LHCb::MCVertex::DecayVertex );
  } else if ( lv->outgoing_particles.size() > 0 ) {
    // If the vertex was not produced during the generation phase, get something from G4
    // Therefore, all children should have been handled by G4 and have the same creatorID assigned
    if ( std::all_of( std::begin( lv->outgoing_particles ), std::end( lv->outgoing_particles ),
                      []( LinkedParticle* p ) -> bool { return p->G4Truth(); } ) ) {
      auto first_proc = ( *std::begin( lv->outgoing_particles ) )->G4Truth()->GetCreatorID();
      if ( std::all_of( std::begin( lv->outgoing_particles ), std::end( lv->outgoing_particles ),
                        [&]( LinkedParticle* p ) -> bool { return p->G4Truth()->GetCreatorID() == first_proc; } ) ) {
        ret->setType( vertexType( first_proc ) );
      } else {
        msgStream << MSG::ERROR << "Failed to set LHCb::MCVertex type. Not all G4 outgoing have same creator ID."
                  << endmsg;
      }
    } else {
      msgStream << MSG::ERROR << "Failed to set LHCb::MCVertex type. No generation and not all children from G4."
                << endmsg;
    }
  } else {
    msgStream << MSG::ERROR << "Failed to set LHCb::MCVertex type. No outgoing particles." << endmsg;
  }
  return ret;
}

void MCTruthToEDM::Converter::convert( LinkedParticle* particle, LHCb::MCVertex* originVertex )
{
  auto mcp = makeMCParticle( particle );
  mcp->setOriginVertex( originVertex );
  originVertex->addToProducts( mcp );
  for ( auto& ev : particle->GetEndVtxs() ) {
    if ( already_converted.count( ev.get() ) > 0 ) {
      // The endvertex has already been converted. This happens if a LinkedVertex
      // has two ingoing particles, usually from the hard collision. In this case
      // we leave the second particle unconnected
      continue;
    }
    auto endVertex = createVertex( ev.get() );
    endVertex->setMother( mcp );
    mcp->addToEndVertices( endVertex );
    for ( auto& child : ev->outgoing_particles ) {
      convert( child, endVertex );
    }
  }
}

LHCb::MCParticle* MCTruthToEDM::Converter::makeMCParticle( LinkedParticle* particle )
{
  // Create and insert into TES.
  // LHCb::MCParticle* mcp = new LHCb::MCParticle();
  auto mcp = new LHCb::MCParticle{};
  m_particles.insert( mcp );
  m_links[particle] = mcp;

  // Set properties.
  Gaudi::LorentzVector mom( particle->GetMomentum() );
  LHCb::ParticleID pid( particle->GetPDG() );
  mcp->setMomentum( mom );
  mcp->setParticleID( pid );

  //  Set the fromSignal flag
  if ( particle->HepMC() && ( HepMC3::Status::SignalInLabFrame == ( particle->HepMC()->status() ) ) ) {
    mcp->setFromSignal( true );
  }

  return mcp;
}

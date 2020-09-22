#include "GiGaMTCoreTruth/LinkedParticle.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "GiGaMTCoreTruth/GaussinoPrimaryParticleInformation.h"
#include "GiGaMTCoreTruth/LinkedParticleHelpers.h"
#include "Helpers.h"
#include "Math/GenVector/Boost.h"
#include "Math/Vector4D.h"

LinkedParticle::~LinkedParticle()
{
  if ( m_tracking ) delete m_tracking;
  // Remove itself from the vertices
  if ( m_prodvtx ) {
    m_prodvtx->outgoing_particles.erase( this );
  }
  for ( auto& vtx : m_endvtxs ) {
    vtx->incoming_particle.erase( this );
  }
}

int LinkedParticle::GetPDG() const
{
  // FIXME: This information should be checked for consistency instead
  if ( m_hepmc ) {
    return m_hepmc->pdg_id();
  }
  if ( m_primary ) {
    return m_primary->GetPDGcode();
  }
  if ( m_tracking ) {
    return m_tracking->GetPdgID();
  }
  return 0;
}

LinkedParticle::PtrSet LinkedParticle::GetParents() const
{
  if ( m_prodvtx ) {
    return m_prodvtx->incoming_particle;
  } else {
    return LinkedParticle::PtrSet{};
  }
}

LinkedParticle::PtrSet LinkedParticle::GetChildren() const
{
  LinkedParticle::PtrSet children;
  for ( auto& vtx : m_endvtxs ) {
    children.insert( std::begin( vtx->outgoing_particles ), std::end( vtx->outgoing_particles ) );
  }
  return children;
}

HepMC3::FourVector LinkedParticle::GetMomentum() const
{
  // One special case:
  // If the particle has a Geant4 simulated parent but is itself MC only,
  // it's momentum as taken from HepMC might be wrong as it doesn't
  // account for any changes of the parent kinematics due to magnetic fields
  // or scattering. Hence compute it from the sum of child momenta.
  HepMC3::FourVector ret{};
  if ( m_tracking ) {
    ret = m_tracking->GetMomentum();
  } else if ( m_hepmc ) {
    ret = m_hepmc->momentum();
  } else {
    return ret;
  }
  // FIXME: This should not happen this way ... Should really reconsider and give
  // the entire decay tree to Geant4
  if ( auto g4parent = Gaussino::LPUtils::GetSimulatedG4Ancestor( this );
       g4parent && GetType() == Gaussino::ConversionType::MC ) {
    ROOT::Math::PxPyPzEVector rparent_start{g4parent->G4Truth()->GetFinalMomentum()};
    ROOT::Math::PxPyPzEVector rparent_final{g4parent->G4Truth()->GetMomentum()};
    HepMC3::FourVector after =
        ROOT::Math::Boost{-rparent_final.BoostToCM()}( ROOT::Math::Boost{rparent_start.BoostToCM()}( ret ) );
    return after;
  } else {
    return ret;
  }
}

int LinkedParticle::GetCreatorID() const
{
  if ( m_tracking ) {
    return m_tracking->GetCreatorID();
  }
  return -1;
}

HepMC3::FourVector LinkedParticle::GetOriginPosition() const
{
  // FIXME: This should not happen this way ... Should really reconsider and give
  // the entire decay tree to Geant4
  if ( auto g4parent = Gaussino::LPUtils::GetSimulatedG4Ancestor( this );
       g4parent && GetType() == Gaussino::ConversionType::MC ) {
    return g4parent->GetEndPosition();
  }
  if ( m_tracking ) {
    return m_tracking->GetOriginVertex();
  } else if ( m_hepmc && m_hepmc->production_vertex() ) {
    return m_hepmc->production_vertex()->position();
  } else {
    return HepMC3::FourVector{};
  }
}

HepMC3::FourVector LinkedParticle::GetEndPosition() const
{
  // FIXME: Need proper definition when more are present
  if ( m_tracking ) {
    return m_tracking->GetEndVertex();
  }
  if ( m_hepmc && m_hepmc->end_vertex() ) {
    return m_hepmc->end_vertex()->position();
  }
  return HepMC3::FourVector{};
}

double LinkedParticle::GetDecayTimeHepMC() const
{
  if ( !m_hepmc ) return -2;
  auto E = m_hepmc->end_vertex();
  auto P = m_hepmc->production_vertex();
  if ( !E ) return -1;
  ROOT::Math::PxPyPzEVector A( P->position() ), B( E->position() );
  ROOT::Math::PxPyPzEVector AB = B - A;

  // Switch to mm for time.
  AB.SetE( AB.T() * CLHEP::c_light );

  // Boost displacement 4-vector to rest frame of particle.
  ROOT::Math::PxPyPzEVector M( GetMomentum() );
  ROOT::Math::Boost theBoost( M.BoostToCM() );
  ROOT::Math::PxPyPzEVector ABStar = theBoost( AB );

  // Switch back to time.
  return ABStar.T() / CLHEP::c_light;
}

void LinkedParticle::AddParent( LinkedParticle* part )
{
  if ( m_hepmc && part->m_hepmc ) {
    if ( !Gaussino::LinkedParticleHelpers::CompareFourVector( m_hepmc->production_vertex()->position(),
                                                              part->m_hepmc->end_vertex()->position() ) ) {
      throw std::runtime_error( "Particle have HepMC but prod/end vertex are at different positions!" );
    }
  }
  std::shared_ptr<LinkedVertex> vertex{nullptr};
  for ( auto& vtx : part->GetEndVtxs() ) {
    if ( m_hepmc && part->m_hepmc ) {
      // Treatment different for particles with HepMC record where the
      // vertex information is used to identify the correct parent vertex
      if ( vtx->hepmc_vtx && vtx->hepmc_vtx->id() == m_hepmc->production_vertex()->id() ) {
        vertex = vtx;
        break;
      }
    } else {
      // Now just find by location. As the vertices are stored locally with the particle, multiple
      // vertices can still exist in the same location.
      // The only scenario where multiple end-vertices for the same particle could
      // exist is from tracking in Geant4. In which case assigning all particles to the same logical vertex
      // that originated in the same point in space is perfectly fine.
      if ( Gaussino::LinkedParticleHelpers::CompareFourVector( vtx->GetPosition(), this->GetOriginPosition() ) ) {
        // However, the vertex type must match, otherwise it is difficult to associate a specific process with a
        // given vertex later on
        if ( GetCreatorID() == vtx->GetProcessID() ) {
          vertex = vtx;
          break;
        }
      }
    }
  }
  if ( vertex ) {
    // Parent has matching vertex. Make sure that this particle has no conflicting vertex set
    if ( m_prodvtx && m_prodvtx.get() != vertex.get() ) {
      throw std::runtime_error( "Particle has production vertex that isn't same a HepMC-matched endvertex of parent." );
    }
    if ( !m_prodvtx ) {
      m_prodvtx = vertex;
    }
  } else {
    // we did not find a matching vertex in the parents decay vertex list.
    // Either add this vertex to parent or create a new one if no
    // production vertex has yet been set
    if ( !m_prodvtx ) {
      vertex = m_prodvtx = std::make_shared<LinkedVertex>( GetID() );
      if ( part->HepMC() && HepMC() ) {
        // If both are HepMC particles, assign the production vertex of the child
        // to the vertex. If particles are skipped this will lead to multiple end vertices
        // for the particle potentially
        m_prodvtx->hepmc_vtx = part->m_hepmc->end_vertex().get();
      }
    } else {
      // If a production vertex exists before but we haven't identified the vertex
      // before in the parent endvertices implies some form of n -> 1 where we currently trying to add
      // the second+ parent. Hence just assign the vertex
      vertex = m_prodvtx;
    }
    part->m_endvtxs.insert( m_prodvtx );
  }
  vertex->incoming_particle.insert( part );
  vertex->outgoing_particles.insert( this );
}

void LinkedParticle::AddEndVertex( const HepMC3::FourVector& position, int procid ) {
  for ( auto& vtx : GetEndVtxs() ) {
    if ( Gaussino::LinkedParticleHelpers::CompareFourVector( vtx->GetPosition(), position ) &&
         vtx->GetProcessID() == procid ) {
      return;
    }
  }
  std::shared_ptr<LinkedVertex> endvtx{new EndLinkedVertex{GetID(), position, procid}};
  m_endvtxs.insert( endvtx );
}

void LinkedParticle::AddChild( LinkedParticle* part ) { part->AddParent( this ); }

template <typename T>
T& operator<<( T& ostr, const HepMC3::FourVector& fv )
{
  ostr << "[" << fv.x() << ", " << fv.y() << ", " << fv.z() << ", " << fv.t() << "]";
  return ostr;
}

std::ostream& operator<<( std::ostream& out, const LinkedParticle& lp )
{
  out << " PDG: " << lp.GetPDG() << ", ID = " <<lp.GetID() << ", IDs = [" << ( lp.m_hepmc ? lp.m_hepmc->id() : -1 ) << ", "
      << ( lp.m_primary
               ? std::to_string( lp.m_primary->GetTrackID() ) + "(" +
                     std::to_string( GaussinoPrimaryParticleInformation::Get( lp.m_primary )->getLinkedID() ) + ")"
               : "-1" )
      << ", " << ( lp.m_tracking ? lp.m_tracking->GetTrackID() : -1 ) << "],"
      << " HepMC|G4Primary|G4Truth = " << (bool)lp.m_hepmc << "|" << (bool)lp.m_primary << "|" << (bool)lp.m_tracking
      << " CONV  = " << lp.m_conversion_type;
  return out;
}

int LinkedVertex::GetProcessID() const
  {
    if ( outgoing_particles.size() > 0 ) {
      return ( *std::begin( outgoing_particles ) )->GetCreatorID();
    }
    return -1;
  }

HepMC3::FourVector LinkedVertex::GetPosition() const
  {
    // FIXME: Prioritize the location G4 simulated particles
    if ( outgoing_particles.size() > 0 ) {
      return ( *std::begin( outgoing_particles ) )->GetOriginPosition();
    }
    if ( incoming_particle.size() > 0 ) {
      return ( *std::begin( incoming_particle ) )->GetEndPosition();
    }

    throw std::runtime_error( "Trying to access position of vertex without associated particles" );
  }

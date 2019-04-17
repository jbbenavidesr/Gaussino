#include "GiGaMTCore/Truth/LinkedParticle.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "Helpers.h"
#include "Math/GenVector/Boost.h"
#include "Math/Vector4D.h"

int LinkedParticle::GetPDG() const
{
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

std::set<LinkedParticle*> LinkedParticle::GetParents() { return m_prodvtx->incoming_particle; }

std::set<LinkedParticle*> LinkedParticle::GetChildren()
{
  std::set<LinkedParticle*> children;
  for ( auto& vtx : m_endvtxs ) {
    children.insert( std::begin( vtx->outgoing_particles ), std::end( vtx->outgoing_particles ) );
  }
  return children;
}

HepMC::FourVector LinkedParticle::GetMomentum() const
{
  if ( m_hepmc ) {
    return m_hepmc->momentum();
  }
  if ( m_primary ) {
    auto mom = m_primary->GetMomentum();
    HepMC::FourVector fourmomentum;
    fourmomentum.setPx( mom.getX() );
    fourmomentum.setPy( mom.getY() );
    fourmomentum.setPz( mom.getZ() );
    fourmomentum.setE( m_primary->GetTotalEnergy() );
    return fourmomentum;
  }
  if ( m_tracking ) {
    return m_tracking->GetMomentum();
  }
  return HepMC::FourVector{};
}

HepMC::FourVector LinkedParticle::GetOriginPosition() const
{
  // FIXME: Need proper definition when more are present
  if ( m_hepmc && m_hepmc->production_vertex() ) {
    return m_hepmc->production_vertex()->position();
  }
  return HepMC::FourVector{};
}

HepMC::FourVector LinkedParticle::GetEndPosition() const
{
  // FIXME: Need proper definition when more are present
  if ( m_hepmc && m_hepmc->end_vertex() ) {
    return m_hepmc->end_vertex()->position();
  }
  return HepMC::FourVector{};
}

double LinkedParticle::GetDecayTimeHepMC() const
{
  if ( !m_hepmc ) return -2;
  auto E = m_hepmc->end_vertex();
  auto P = m_hepmc->production_vertex();
  if ( !E ) return 0;
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
      if ( vtx->hepmc_vtx && vtx->hepmc_vtx->id() == part->m_hepmc->end_vertex()->id() ) {
        vertex = vtx;
        break;
      }
    } else {
      // Now just find by location. As the vertices are stored locally with the particle, multiple
      // vertices can still exist. The only scenario where multiple end-vertices for the same particle could
      // exist is from tracking in Geant4. In which case assigning all particles to the same logical vertex
      // that originated in the same point in space is perfectly fine.
      if ( Gaussino::LinkedParticleHelpers::CompareFourVector( vtx->GetPosition(), this->GetOriginPosition() ) ) {
        vertex = vtx;
        break;
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
      m_prodvtx = std::make_shared<LinkedVertex>();
      if ( part->HepMC() && HepMC() ) {
        m_prodvtx->hepmc_vtx = &( *part->HepMC()->end_vertex() ); // FIXME: ugly
      }
    }
    part->m_endvtxs.insert( m_prodvtx );
  }
  vertex->incoming_particle.insert( part );
  vertex->outgoing_particles.insert( this );
}

void LinkedParticle::AddChild( LinkedParticle* part ) { part->AddParent( this ); }

std::ostream& operator<<( std::ostream& out, const LinkedParticle& lp )
{
  out << " PDG: " << lp.GetPDG() << ", ID = [" << ( lp.m_hepmc ? lp.m_hepmc->id() : -1 ) << ", "
      << ( lp.m_primary ? lp.m_primary->GetTrackID() : -1 ) << ", "
      << ( lp.m_tracking ? lp.m_tracking->GetTrackID() : -1 ) << "], " << lp.GetMomentum().p3mod()
      << " HepMC|G4Primary|G4Truth = " << (bool)lp.m_hepmc << "|" << (bool)lp.m_primary << "|" << (bool)lp.m_tracking
      << " CONV  = " << lp.m_conversion_type;
  return out;
}

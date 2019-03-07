#include "GiGaMTCore/Truth/LinkedParticle.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "Math/Vector4D.h"
#include "Math/GenVector/Boost.h"

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
  if ( m_hepmc && m_hepmc->production_vertex()) {
    return m_hepmc->production_vertex()->position();
  }
  return HepMC::FourVector{};
}

HepMC::FourVector LinkedParticle::GetEndPosition() const
{
  // FIXME: Need proper definition when more are present
  if ( m_hepmc && m_hepmc->end_vertex()) {
    return m_hepmc->end_vertex()->position();
  }
  return HepMC::FourVector{};
}

double LinkedParticle::GetDecayTimeHepMC() const
{
  if(!m_hepmc) return -2;
  auto E = m_hepmc->end_vertex();
  auto P = m_hepmc->production_vertex();
  if ( !E ) return 0;
  ROOT::Math::PxPyPzEVector A( P->position() ), B( E->position() );
  ROOT::Math::PxPyPzEVector AB = B - A;

  // Switch to mm for time.
  AB.SetE( AB.T() * CLHEP::c_light );

  // Boost displacement 4-vector to rest frame of particle.
  ROOT::Math::PxPyPzEVector M( GetMomentum());
  ROOT::Math::Boost theBoost( M.BoostToCM() );
  ROOT::Math::PxPyPzEVector ABStar = theBoost( AB );

  // Switch back to time.
  return ABStar.T() / CLHEP::c_light;
}

std::ostream& operator<<( std::ostream& out, const LinkedParticle& lp )
{
  out << " PDG: " << lp.GetPDG() << ", ID = [" << (lp.m_hepmc ? lp.m_hepmc->id() : -1) << ", " << (lp.m_primary ? lp.m_primary->GetTrackID() : -1) << ", " << (lp.m_tracking ? lp.m_tracking->GetTrackID() : -1) << "], " << lp.GetMomentum().p3mod() << " HepMC|G4Primary|G4Truth = " << (bool)lp.m_hepmc
      << "|" << (bool)lp.m_primary << "|" << (bool)lp.m_tracking << " CONV  = " << lp.m_conversion_type;
  return out;
}

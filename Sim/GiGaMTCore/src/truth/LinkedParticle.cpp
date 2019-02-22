#include "GiGaMTCore/Truth/LinkedParticle.h"

int LinkedParticle::GetPDG() const
{
  if ( m_hepmc ) {
    return m_hepmc->pdg_id();
  }
  if ( m_primary ) {
    return m_primary->GetPDGcode();
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
  return HepMC::FourVector{};
}

std::ostream& operator<<( std::ostream& out, const LinkedParticle& lp )
{
  out << " PDG: " << lp.GetPDG() << ", ID: #" << (lp.m_hepmc ? lp.m_hepmc->id() : -1) << ", " << lp.GetMomentum().p3mod() << " HepMC|G4Primary|G4Truth = " << (bool)lp.m_hepmc
      << "|" << (bool)lp.m_primary << "|" << (bool)lp.m_tracking << " CONV  = " << lp.m_conversion_type;
  return out;
}

#include "Helpers.h"
#include "GiGaMTCore/Truth/LinkedParticle.h"

namespace Gaussino::LinkedParticleHelpers
{

  HepMC3::ConstGenParticlePtr hasOscillated( const HepMC3::GenParticle* P )
  {
    auto ev = P->end_vertex();
    if ( !ev ) return nullptr;
    if ( 1 != ev->particles_out().size() ) return nullptr;
    auto D = *std::begin( ev->particles_out() );
    if ( !D ) return nullptr;
    if ( -P->pdg_id() != D->pdg_id() ) return nullptr;
    return D;
  }

  bool VerifyLink( LinkedParticle* a, LinkedParticle* b )
  {
    return CompareFourVector(a->GetEndPosition(), b->GetOriginPosition());
  }

  bool CompareFourVector( const HepMC3::FourVector& a, const HepMC3::FourVector& b ){
    if ( !essentiallyEqual( a.x(), b.x() ) ) return false;
    if ( !essentiallyEqual( a.y(), b.y() ) ) return false;
    if ( !essentiallyEqual( a.z(), b.z() ) ) return false;
    if ( !essentiallyEqual( a.t(), b.t() ) ) return false;
    return true;
  }
}

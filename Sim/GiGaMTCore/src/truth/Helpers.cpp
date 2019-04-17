#include "Helpers.h"
#include "GiGaMTCore/Truth/LinkedParticle.h"

namespace Gaussino::LinkedParticleHelpers
{

  HepMC::GenParticlePtr hasOscillated( const HepMC::GenParticle* P )
  {
    auto& ev = P->end_vertex();
    if ( !ev ) return nullptr;
    if ( 1 != ev->particles_out_size() ) return nullptr;
    auto D = *( ev->particles_out_const_begin() );
    if ( !D ) return nullptr;
    if ( -P->pdg_id() != D->pdg_id() ) return nullptr;
    return D;
  }

  bool VerifyLink( LinkedParticle* a, LinkedParticle* b )
  {
    return CompareFourVector(a->GetEndPosition(), b->GetOriginPosition());
  }

  bool CompareFourVector( const HepMC::FourVector& a, const HepMC::FourVector& b ){
    if ( !essentiallyEqual( a.x(), b.x() ) ) return false;
    if ( !essentiallyEqual( a.y(), b.y() ) ) return false;
    if ( !essentiallyEqual( a.z(), b.z() ) ) return false;
    if ( !essentiallyEqual( a.t(), b.t() ) ) return false;
    return true;
  }
}

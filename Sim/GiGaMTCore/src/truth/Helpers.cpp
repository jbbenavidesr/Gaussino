#include "Helpers.h"
#include "GiGaMTCoreTruth/LinkedParticle.h"

namespace Gaussino::LinkedParticleHelpers
{

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

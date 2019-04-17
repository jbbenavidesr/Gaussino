#include "HepMC/FourVector.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
class LinkedParticle;

namespace Gaussino::LinkedParticleHelpers
{
  // Compare whether
  template <typename FP>
  bool essentiallyEqual( FP a, FP b, FP epsilon=0.00001 )
  {
    return fabs( a - b ) <= ( ( fabs( a ) > fabs( b ) ? fabs( b ) : fabs( a ) ) * epsilon );
  }
  // Determine if the particle has oscillated by checking if it has a
  // single child with opposite PDG ID. If so, return ptr to child,
  // nullptr otherwise
  HepMC::GenParticlePtr hasOscillated( const HepMC::GenParticle* P );

  // Helper function to verify that ones particles endvertex is at the same position as the others
  // origin in the original HepMC record, i.e. that particles that have been skipped in between didn't fly
  bool VerifyLink( LinkedParticle* a, LinkedParticle* b );
  bool CompareFourVector( const HepMC::FourVector& a, const HepMC::FourVector& b );
}

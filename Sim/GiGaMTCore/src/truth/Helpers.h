/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#include "HepMC3/FourVector.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
class LinkedParticle;

namespace Gaussino::LinkedParticleHelpers
{
  // Compare whether
  template <typename FP>
  bool essentiallyEqual( FP a, FP b, FP epsilon=0.0001 )
  {
    return fabs( a - b ) <= ( ( fabs( a ) > fabs( b ) ? fabs( b ) : fabs( a ) ) * epsilon );
  }

  // Helper function to verify that ones particles endvertex is at the same position as the others
  // origin in the original HepMC record, i.e. that particles that have been skipped in between didn't fly
  bool VerifyLink( LinkedParticle* a, LinkedParticle* b );
  bool CompareFourVector( const HepMC3::FourVector& a, const HepMC3::FourVector& b );
}

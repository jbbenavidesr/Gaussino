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

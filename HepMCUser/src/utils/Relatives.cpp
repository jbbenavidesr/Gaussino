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
#include "HepMCUtils/Relatives.h"
namespace HepMCUtils {
  const WrapperRelatives::AncestorsWrapper   WrapperRelatives::ANCESTORS{};
  const WrapperRelatives::DescendantsWrapper WrapperRelatives::DESCENDANTS{};

  const HepMC3::Relatives& RelativesFromID( int idx ) {

    if ( idx == 0 ) {
      return HepMC3::Relatives::PARENTS;
    } else if ( idx == 1 ) {
      return HepMC3::Relatives::CHILDREN;
    } else if ( idx == 3 ) {
      return HepMCUtils::WrapperRelatives::ANCESTORS;
    } else {
      return HepMCUtils::WrapperRelatives::DESCENDANTS;
    }
  }
} // namespace HepMCUtils

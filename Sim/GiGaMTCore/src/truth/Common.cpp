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
#include "GiGaMTCoreTruth/Common.h"

std::ostream& operator<<( std::ostream& out, const Gaussino::ConversionType& type )
{
  switch ( type ) {
  case Gaussino::ConversionType::G4:
    return out << "G4";
  case Gaussino::ConversionType::MC:
    return out << "MC";
  case Gaussino::ConversionType::FROMG4:
    return out << "FROMG4";
  case Gaussino::ConversionType::REDECAY:
    return out << "REDECAY";
  default:
    return out;
  }
}

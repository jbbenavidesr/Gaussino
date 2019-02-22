#include "GiGaMTCore/Truth/Common.h"

std::ostream& operator<<( std::ostream& out, const Gaussino::ConversionType& type )
{
  switch ( type ) {
  case Gaussino::ConversionType::G4:
    return out << "G4";
  case Gaussino::ConversionType::MC:
    return out << "MC";
  case Gaussino::ConversionType::NONE:
    return out << "NONE";
  default:
    return out;
  }
}

#pragma once

#include <ostream>

namespace Gaussino
{
  enum class ConversionType { MC, G4, FROMG4, REDECAY };
}

std::ostream& operator<<( std::ostream& out, const Gaussino::ConversionType& type );

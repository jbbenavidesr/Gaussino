#pragma once

#include <ostream>

namespace Gaussino
{
  enum class ConversionType { MC, G4, NONE };
}

std::ostream& operator<<( std::ostream& out, const Gaussino::ConversionType& type );

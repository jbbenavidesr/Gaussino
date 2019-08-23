#include "GiGaMTCoreTruth/GaussinoPrimaryParticleInformation.h"

std::ostream& operator<<( std::ostream& out, const GaussinoPrimaryParticleInformation& lp )
{
  out << " PrimaryInfo: LinkedID=" << lp.m_linkedID;
  return out;
}

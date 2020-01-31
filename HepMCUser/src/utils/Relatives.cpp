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

#include "GiGaMTCoreTruth/LinkedParticleHelpers.h"

namespace Gaussino::LPUtils {
    bool ShouldHaveButWasNotSimulated( const LinkedParticle* lp ) {
      return lp->G4Primary() && !lp->G4Truth();
    }

    LinkedParticle* GetSimulatedG4Parent( const LinkedParticle* lp ) {
      for ( auto parent : lp->GetParents() ) {
        if ( parent->G4Primary() && parent->G4Truth() ) {
          return parent;
        }
      }
      return nullptr;
    }

    bool HasSimulatedG4Parent( const LinkedParticle* lp ) {
      return (bool) GetSimulatedG4Parent(lp);
    }

    LinkedParticle* GetSimulatedG4Ancestor( const LinkedParticle* lp ) {
      for ( auto parent : lp->GetParents() ) {
        if ( parent->G4Primary() && parent->G4Truth() ) {
          return parent;
        } else {
          return GetSimulatedG4Parent(parent);
        }
      }
      return nullptr;
    }

    bool HasSimulatedG4Ancestor( const LinkedParticle* lp ) {
      return (bool) GetSimulatedG4Ancestor(lp);
    }

    bool HasG4ChildWithoutG4Truth( const LinkedParticle* lp ) {
      bool found = false;
      for ( auto child : lp->GetChildren() ) {
        if ( ShouldHaveButWasNotSimulated( child ) ) {
          found = true;
        } else if ( child->GetType() == ConversionType::MC ) {
          // Recursively call on child if child itself was not given
          // to G4
          found = found || HasG4ChildWithoutG4Truth( child );
        }
      }
      return found;
    }
}

#pragma once

#include "GiGaMTCoreTruth/LinkedParticle.h"

namespace Gaussino::LPUtils {
    bool ShouldHaveButWasNotSimulated( const LinkedParticle* lp );

    LinkedParticle* GetSimulatedG4Parent( const LinkedParticle* lp );

    bool HasSimulatedG4Parent( const LinkedParticle* lp );

    LinkedParticle* GetSimulatedG4Ancestor( const LinkedParticle* lp );

    bool HasSimulatedG4Ancestor( const LinkedParticle* lp );

    bool HasG4ChildWithoutG4Truth( const LinkedParticle* lp );
}

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
#pragma once

#include "GiGaMTCoreRun/LinkedParticle.h"

namespace Gaussino::LPUtils {
    bool ShouldHaveButWasNotSimulated( const LinkedParticle* lp );

    LinkedParticle* GetSimulatedG4Parent( const LinkedParticle* lp );

    bool HasSimulatedG4Parent( const LinkedParticle* lp );

    LinkedParticle* GetSimulatedG4Ancestor( const LinkedParticle* lp );

    bool HasSimulatedG4Ancestor( const LinkedParticle* lp );

    bool HasG4ChildWithoutG4Truth( const LinkedParticle* lp );
}

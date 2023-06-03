/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

// Gaussino
#include "CollectorG4Hit.h"
#include "GiGaMTCoreRun/GaussinoEventInformation.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

namespace Gaussino::G4Par04 {
  struct EventInformation : public GaussinoEventInformation {
    EventInformation( GaussinoEventInformation&& info )
        : GaussinoEventInformation( std::forward<GaussinoEventInformation>( info ) ) {}

    inline void SetCollectorHitsMap( std::shared_ptr<CollectorG4Hit::Map> hitsMap ) { m_hitsMap = hitsMap; };

    CollectorG4Hit* GetCollectorHit( const int trackID ) const {
      if ( auto result = m_hitsMap->find( trackID ); result != m_hitsMap->end() ) {
        return result->second;
      } else {
        throw GaudiException(
            "No collector hit available for the selected trackID! Something is off with the configuration...",
            "Gaussino::G4Par04::EventInformation", StatusCode::FAILURE );
      }
    };

  private:
    // trackID <-> collector hits
    std::shared_ptr<CollectorG4Hit::Map> m_hitsMap;
  };
} // namespace Gaussino::G4Par04

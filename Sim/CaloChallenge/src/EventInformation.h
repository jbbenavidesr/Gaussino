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

namespace Gsino::CaloChallenge {
  /**
   * @brief Represents event-level information in the calorimeter simulation.
   *
   *  This includes mappings between track IDs and collector hits.
   *  It is needed in the context of the CaloChallenge to map exactly which
   *  collector hit corresponds to which track ID in the simulation.
   *
   * @author Michał Mazurek
   * @date 2023
   */
  struct EventInformation : public GaussinoEventInformation {
    EventInformation( GaussinoEventInformation&& info )
        : GaussinoEventInformation( std::forward<GaussinoEventInformation>( info ) ) {}

    inline void SetCollectorHitsMap( std::shared_ptr<CollectorG4Hit::Map> hitsMap ) { m_hitsMap = hitsMap; };

    CollectorG4Hit* GetCollectorHit( const int trackID, bool ignore = false ) const {
      if ( auto result = m_hitsMap->find( trackID ); result != m_hitsMap->end() ) {
        return result->second;
      } else {
        if ( ignore ) return nullptr;
        throw GaudiException(
            "No collector hit available for the selected trackID! Something is off with the configuration...",
            "Gsino::CaloChallenge::EventInformation", StatusCode::FAILURE );
      }
    };

  private:
    // trackID <-> collector hits
    std::shared_ptr<CollectorG4Hit::Map> m_hitsMap;
  };
} // namespace Gsino::CaloChallenge

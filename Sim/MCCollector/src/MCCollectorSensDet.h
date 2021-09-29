/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include "GaussTools/GaussTrackInformation.h"
#include "GiGa/GiGaSensDetBase.h"
#include "MCCollectorHit.h"

// Forward declarations
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace MCCollector {
  class SensDet : virtual public GiGaSensDetBase {

  public:
    SensDet( const std::string& type, const std::string& name, const IInterface* parent )
        : G4VSensitiveDetector( name ), GiGaSensDetBase( type, name, parent ) {
      collectionName.insert( "Hits" );
    }

    void Initialize( G4HCofThisEvent* HCE ) override;

    bool ProcessHits( G4Step* step, G4TouchableHistory* history ) override;

  protected:
    HitsCollection* m_col;

    // Watch out: default dE/dx=0 true by default
    Gaudi::Property<bool> m_requireEDep{this, "RequireEDep", false};

    // Only forward particles
    Gaudi::Property<bool> m_onlyForward{this, "OnlyForward", true};

    // Only hits at the boundary
    Gaudi::Property<bool> m_onlyAtBoundary{this, "OnlyAtBoundary", false};
  };
} // namespace MCCollector

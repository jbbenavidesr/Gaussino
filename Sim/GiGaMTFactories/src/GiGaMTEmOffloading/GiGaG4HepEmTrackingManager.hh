/*****************************************************************************\
* (c) Copyright 2025 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// SPDX-FileCopyrightText: 2024 CERN
// SPDX-License-Identifier: Apache-2.0

///   GiGaG4HepEmTrackingManager class:
///   The derived class from G4HepEmTrackingManager must implement HandOverOneTrack and CheckEarlyTrackingExit to allow
///   for an early exit of the tracking loop in the G4HepEmTrackingManager

#ifndef GiGaG4HepEmTrackingManager_hh
#define GiGaG4HepEmTrackingManager_hh

#include "G4HepEmTrackingManager.hh"

#ifndef G4HepEm_EARLY_TRACKING_EXIT
#  error "Build error: G4HepEm must be build with -DG4HepEm_EARLY_TRACKING_EXIT=ON"
#endif

class GiGaG4HepEmTrackingManager : public G4HepEmTrackingManager {
public:
  GiGaG4HepEmTrackingManager( G4int verbose );
  ~GiGaG4HepEmTrackingManager();

  // Implement the early tracking exit function
  bool CheckEarlyTrackingExit( G4Track* track, G4EventManager* evtMgr, G4UserTrackingAction* userTrackingAction,
                               G4TrackVector& secondaries ) const override;

  void HandOverOneTrack( G4Track* aTrack ) override;

private:
  G4int fCurrentEventId = -1;

  // min position and kinetic energy for e-/e+/gamma. Using the same default values as TrCutsRunAction.cpp
  // real values are read from TrCutsRunAction in the constructor
  double fMinX{ -10000. }, fMinY{ -10000. }, fMinZ{ -5000. }, fMaxX{ 10000. }, fMaxY{ 10000. }, fMaxZ{ 25000. };
  double fElectronTrackingCut{ 1. * CLHEP::MeV };
  double fGammaTrackingCut{ 1. * CLHEP::MeV }; // Note: A gamma tracking cut of 1 MeV is extremely high as it removes
                                               // all gammas from annihilation at rest! This changes physics results
};

#endif // GiGaG4HepEmTrackingManager_hh

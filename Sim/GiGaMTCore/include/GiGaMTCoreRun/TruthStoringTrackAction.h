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

// Geant4
#include "G4UserTrackingAction.hh"
#include "G4VProcess.hh"

// Gaussino
#include "GiGaMTCoreMessage/IMessage.h"

// HepMC3
#include "HepMC3/FourVector.h"

// STL
#include <functional>
#include <optional>
#include <string>
#include <unordered_set>

// namespace LHCb {
// class IParticlePropertySvc;
//}

/** @class TruthStoringTrackAction TruthStoringTrackAction.h
 *
 *  Accesses the track information to check whether a track needs to be stored
 *  in the MCTruthTracker. If so, registers the track with the MCTruthTracker
 *  associated to the current G4Event and provides all the needed information
 *  to construct the relationship to other particles.
 *
 *  @author Dominik Muller
 *  @date   28/02/2019
 */

class TruthStoringTrackAction : virtual public G4UserTrackingAction, virtual public Gsino::Message {
  /// friend factory for instantiation
  // friend class GiGaFactory<TruthStoringTrackAction>;
  ///
public:
  virtual ~TruthStoringTrackAction()                        = default;
  TruthStoringTrackAction()                                 = default;
  TruthStoringTrackAction( const TruthStoringTrackAction& ) = delete;

  // We only know whether a track is to be stored at the end
  // of its tracking. However, G4Track only represents the current state.
  // Therefore, this function copies information on the initial state
  // of the track which is later used if the track is to be stored.
  void PreUserTrackingAction( const G4Track* track ) override;

  void PostUserTrackingAction( const G4Track* track ) override;

  std::function<std::optional<int>( const std::string& )> m_fNameToID = []( const std::string& ) {
    return std::nullopt;
  };

  bool addEndVertices{ false };

protected:
  // Function to return an integer ID for the process. Currently
  // implements the default LHCb behavior and codes but is declared
  // virtual so users can inherit and implement their own version.
  virtual int processID( const G4VProcess* creator );

  HepMC3::FourVector fourmomentum;

  static std::unordered_set<std::string> m_hadronicProcesses;
};

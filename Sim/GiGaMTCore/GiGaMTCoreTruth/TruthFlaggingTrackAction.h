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
// STL
#include <mutex>
#include <string>
#include <vector>
// GiGa
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4UserTrackingAction.hh"
#include "GiGaMTCoreMessage/IGiGaMessage.h"
#include "GiGaMTCoreTruth/ZMaxPlane.h"

#include <unordered_set>
// forward declarations
// template <class TYPE> class GiGaFactory;
class G4Track;
class G4ParticleDefinition;

/** @class TruthFlaggingTrackAction TruthFlaggingTrackAction.h
 *
 *  Track action that flags tracks that should be kept and stores
 *  the relevant information in the respectiv user information
 *  object associated to the track itself. The actual storing
 *  must be handled by a separate action.
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @author  Dominik Muller dominik.muller@cern.ch
 *
 *  @date    23/01/2001
 */

class TruthFlaggingTrackAction : virtual public G4UserTrackingAction, virtual public GiGaMessage {
  /// friend factory for instantiation
  //  friend class GiGaFactory<TruthFlaggingTrackAction>;

public:
  /// useful typedefs
  typedef std::vector<std::string>                        TypeNames;
  typedef std::unordered_set<const G4ParticleDefinition*> PartDefs;

  /// destructor (virtual and protected)
  virtual ~TruthFlaggingTrackAction() = default;
  TruthFlaggingTrackAction()          = default;

public:
  /** If store by own energy is true and energy above ownEnergyThreshold, track is marked
   *  to be stored (unless storeUpToZmax is true and beyond zMaxToStore). This will also
   *  likely be the first time GaussinoTrackInformation is required and thus created.
   */
  void PreUserTrackingAction( const G4Track* ) override;

  /** perform action
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject
   */
  void PostUserTrackingAction( const G4Track* ) override;

private:
  void           Setup();
  std::once_flag run_setup_flag;
  /// Utility function to fill GaussTrack informations
  // FIXME: Used only for trajectories to removing this for now
  // void fillGaussTrackInformation( const G4Track* track ) const;

  /// Utility function to fill process information of a track
  // FIXME: Used only for trajectories to removing this for now
  // void setProcess( const G4Track* track ) const;

public:
  //  Flags:
  ///  all tracks to be stored
  bool storeAll{false};
  ///  all primaries are stored
  bool storePrimaries{true};
  /// all decays products are stored
  bool storeDecayProducts{true};
  /// all track with kinetic energy above some threshold are stored
  bool storeByOwnEnergy{false};
  ///  all tracks  with given type are stored
  bool storeByOwnType{false};
  /** all tracks which has a daughter
   *  with kinetic energy over threshold are stored
   */
  bool storeByChildEnergy{false};
  ///  all tracks which has a daughter of given type are stored
  bool storeByChildType{false};
  /// threshold for own kinetic energy
  double ownEnergyThreshold{10 * CLHEP::TeV};
  /// threshold for child kinetic energy
  double childEnergyThreshold{10 * CLHEP::TeV};

  /// container of names of own   types
  TypeNames ownStoredTypesNames{};
  /// container of names of child types
  TypeNames childStoredTypesNames{};
  ///

  /// store particles undorgoing specific process
  bool storeBySecondariesProcess{false};
  /// container for the process names
  std::unordered_set<std::string> childStoredProcess{};
  /// store particles produced in specific process
  bool storeByOwnProcess{false};
  /// container for own process names
  std::unordered_set<std::string> ownStoredProcess{};

  /// for flags above beside primary, forced decays and marked store up
  /// to a certain z
  bool storeUpToZmax{true};
  /// value of z max for storage
  double zMaxToStore{10. * CLHEP::km};
  /// tilt of zMax plane in degrees
  double zMaxTilt{0. * CLHEP::degree};
  /// y shift of zMax plane
  double zMaxYShift{0. * CLHEP::mm};
  /// bool to keep or not RICHPhotoelectrons
  bool rejectRICHphe{true};
  /// bool to keep or not optical photons
  bool rejectOptPhot{true};

  /// container of definitions of own   types
  PartDefs ownStoredTypes{};
  /// container of definitions  of child types
  PartDefs childStoredTypes{};
  ///
private:
  /// ZMax plane (transformed z = 0 plane)
  ZMaxPlane zMaxPlane;
};

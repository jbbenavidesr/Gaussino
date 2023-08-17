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

// Geant4
#include "G4Allocator.hh"
#include "G4RotationMatrix.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"

// Gaussino
#include "GiGaMTCoreDet/GaussHitBase.h"

class G4LogicalVolume;
class G4AttDef;

namespace Gaussino::G4Par04 {
  struct CaloG4Hit : public HitBase {
    void* operator new( size_t );
    void operator delete( void* aHit );
    /// Visualise hits. If pointer to the logical volume was set, cell shape is
    /// drawn taking into account proper radial position (taken from fRhoId)
    virtual void Draw() final;
    /// Retrieve atributes' names in order to allow filtering
    virtual const std::map<G4String, G4AttDef>* GetAttDefs() const final;
    /// Create attributes for the visualisation.
    virtual std::vector<G4AttValue>* CreateAttValues() const final;
    /// Print hit properties.
    virtual void Print() final;
    /// Set position
    inline void SetPos( G4ThreeVector aXYZ ) { fPos = aXYZ; }
    /// Get position
    inline G4ThreeVector GetPos() const { return fPos; }
    /// Set rotation
    inline void SetRot( G4RotationMatrix aXYZ ) { fRot = aXYZ; }
    /// Get rotation
    inline G4RotationMatrix GetRot() const { return fRot; }
    /// Set energy
    inline void SetEdep( G4double aEdep ) { fEdep = aEdep; }
    /// Add energy to previous value
    inline void AddEdep( G4double aEdep ) { fEdep += aEdep; }
    /// Get energy
    inline G4double GetEdep() const { return fEdep; }
    /// Set Z id of the cell in the readout segmentation
    inline void SetZId( G4int aZ ) { fZId = aZ; }
    /// Get Z id of the cell in the readout segmentation
    inline G4int GetZId() const { return fZId; }
    /// Set Rho id of the cell in the readout segmentation
    inline void SetRhoId( G4int aRho ) { fRhoId = aRho; }
    /// Get rho id of the cell in the readout segmentation
    inline G4int GetRhoId() const { return fRhoId; }
    /// Set phi id of the cell in the readout segmentation
    inline void SetPhiId( G4int aPhi ) { fPhiId = aPhi; }
    /// Get phi id of the cell in the readout segmentation
    inline G4int GetPhiId() const { return fPhiId; }
    /// Set time
    inline void SetTime( G4double aTime ) { fTime = aTime; }
    /// Get time
    inline G4double GetTime() const { return fTime; }
    /// Set type (0 = full sim, 1 = fast sim)
    inline void SetType( G4int aType ) { fType = aType; }
    /// Get type (0 = full sim, 1 = fast sim)
    inline G4int GetType() const { return fType; }
    // Set pointer to cell logical volume
    inline void SetLogV( G4LogicalVolume* aLogVol ) { fLogVol = aLogVol; }
    // Get pointer to cell logical volume
    inline const G4LogicalVolume* GetLogVol() { return fLogVol; }

  private:
    /// Energy deposit
    G4double fEdep = 0;
    /// Z ID of readout cell
    G4int fZId = -1;
    /// Rho ID of readout cell
    G4int fRhoId = -1;
    /// Phi ID of readout cell
    G4int fPhiId = -1;
    /// Position
    G4ThreeVector fPos = { -1, -1, -1 };
    /// Rotation
    G4RotationMatrix fRot;
    /// Time
    G4double fTime = -1;
    /// Type: 0 = full sim, 1 = fast sim
    G4int fType = -1;
    /// Pointer to logical volume for visualisation
    G4LogicalVolume* fLogVol = nullptr;
  };

  using CaloHitsCollection = G4THitsCollection<CaloG4Hit>;

  extern G4ThreadLocal G4Allocator<CaloG4Hit>* CaloHitAllocator;

} // namespace Gaussino::G4Par04

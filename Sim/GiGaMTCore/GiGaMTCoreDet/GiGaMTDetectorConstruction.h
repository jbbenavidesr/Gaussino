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

// from Gaudi
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include <functional>

//@class GiGaMTDetectorConstruction
//@brief Simple wrapper around G4VUserDetectorConstruction
//
// This wraps around G4VUserDetectorConstruction and acts as a proxy that
// calls externally provided functions to construct the world as well as the
// senstive detectors and fields
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTDetectorConstruction final : public G4VUserDetectorConstruction
{
  typedef std::function<G4VPhysicalVolume*()> worldConstructor;
  typedef std::function<void()> sdConstructor;

public:
  using G4VUserDetectorConstruction::G4VUserDetectorConstruction;

  ~GiGaMTDetectorConstruction(){};

  virtual G4VPhysicalVolume* Construct() override final { return m_world_constructor(); }
  virtual void ConstructSDandField() override final { m_sd_constructor(); }

  void SetWorldConstructor(worldConstructor constr){m_world_constructor=constr;}
  void SetSDConstructor(sdConstructor constr){m_sd_constructor=constr;}

private:
  worldConstructor m_world_constructor;
  sdConstructor m_sd_constructor;

};

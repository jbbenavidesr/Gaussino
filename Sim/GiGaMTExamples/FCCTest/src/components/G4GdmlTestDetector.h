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

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

/** @class G4GdmlTestDetector SimG4Components/src/G4GdmlTestDetector.h G4GdmlTestDetector.h
 *
 *  Detector construction tool using the GDML file.
 *  GDML file name needs to be specified in job options file (\b'gdml').
 *
 *  @author Anna Zaborowska
 *
 *  Modification for MT Gaussino
 *
 *  @author Dominik Muller
 */

class G4GdmlTestDetector : public extends<GaudiTool, GiGaFactoryBase<G4VUserDetectorConstruction>>
{
private:
  Gaudi::Property<std::string> m_gdmlFile{this, "GDML", ""};

public:
  using extends::extends;
  G4VUserDetectorConstruction* construct() const override;
};

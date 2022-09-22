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
#include "G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

/** @class GDMLConstructionFactory SimG4Components/src/GDMLConstructionFactory.h GDMLConstructionFactory.h
 *
 *  Simple tool to create a detector construction which instantiates the geometry from a GDML file.
 *  No support for sensitive detectors or fields
 *
 *  @author Dominik Muller
 */

class GDMLConstructionFactory : public extends<GaudiTool, GiGaFactoryBase<G4VUserDetectorConstruction>> {
private:
  Gaudi::Property<std::string> m_gdmlFile{this, "GDML", ""};

public:
  using extends::extends;
  G4VUserDetectorConstruction* construct() const override;
};

/*****************************************************************************\
* (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include "GaudiKernel/IAlgTool.h"

class G4VPhysicalVolume;

/** @class IGDMLReader IGDMLReader.h
 *  Interface class for importing GDML files
 *
 */

class IGDMLReader : virtual public IAlgTool {
public:
  DeclareInterfaceID( IGDMLReader, 1, 0 );

  virtual StatusCode import( G4VPhysicalVolume* world ) const = 0;
};

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

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// G4
#include "Geant4/G4VUserParallelWorld.hh"

class IParallelWorldFactory : virtual public IAlgTool {

public:
  DeclareInterfaceID( IParallelWorldFactory, 1, 0 );

  virtual G4VUserParallelWorld* construct() const = 0;
};

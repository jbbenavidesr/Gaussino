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

#include "GaudiKernel/IAlgTool.h"

class G4VPhysicalVolume;

class IExternalDetectorEmbedder : virtual public IAlgTool {

public:
  DeclareInterfaceID( IExternalDetectorEmbedder, 1, 0 );

  virtual StatusCode embed( G4VPhysicalVolume* world ) const = 0;
};

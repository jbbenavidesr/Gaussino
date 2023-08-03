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

#include "GaudiKernel/IAlgTool.h"
#include "HepMC3/GenParticle_fwd.h"
#include "HepMCUser/typedefs.h"

class IReDecaySorter : public extend_interfaces<IAlgTool> {
public:
  DeclareInterfaceID( IReDecaySorter, 1, 0 );
  virtual bool FlagAndRemoveReDecays( std::vector<HepMC3::GenEventPtr>& ) const = 0;
};

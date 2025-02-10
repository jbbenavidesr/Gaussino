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

// Gaudi
#include <GaudiKernel/IService.h>

// Gaussino
#include "GaussinoMLBase/ModelServerBase.h"

namespace Gsino::ML {

  template <typename TServer>
  struct IModelServerSvc : extend_interfaces<IService> {

    static_assert( std::is_base_of<ModelServerBase, TServer>::value );

    DeclareInterfaceID( IModelServerSvc, 1, 1 );

    virtual TServer* getServer() const = 0;
  };
} // namespace Gsino::ML

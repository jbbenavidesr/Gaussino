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
#include "GaudiKernel/IAlgTool.h"
#include<type_traits>

/** GiGaFactoryBase
 *
 *  Base class for all factories of G4 objects which defines
 *  construct interface
 *
 *  @author Dominik Muller
 *  @date   2018-06-04
 */

template <typename T, typename... Args>
class GiGaFactoryBase : virtual public extend_interfaces<IAlgTool>
{
public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return iid::interfaceID(); }
  using iid      = Gaudi::InterfaceId<GiGaFactoryBase<T, Args...>, 3, 0>;
  using ext_iids = typename iid::iids;

  //using extend_interfaces::extend_interfaces;
  //
  virtual ~GiGaFactoryBase() = default;

  virtual T* construct(std::conditional_t<std::is_pointer_v<Args>,Args,const Args&>...) const = 0;
};

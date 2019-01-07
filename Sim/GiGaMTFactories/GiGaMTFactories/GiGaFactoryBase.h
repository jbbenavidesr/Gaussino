#pragma once

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** GiGaFactoryBase
 *
 *  Base class for all factories of G4 objects which defines
 *  construct interface
 *
 *  @author Dominik Muller
 *  @date   2018-06-04
 */

template <typename T>
class GiGaFactoryBase : public extend_interfaces<IAlgTool>
{
public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return iid::interfaceID(); }
  using iid      = Gaudi::InterfaceId<GiGaFactoryBase<T>, 1, 0>;
  using ext_iids = typename iid::iids;

  using extend_interfaces::extend_interfaces;
  virtual ~GiGaFactoryBase(){};

  virtual T* construct() const = 0;
};

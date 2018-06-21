#pragma once

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GiGaMTCore/IGiGaMessage.h"

/** GiGaFactoryBase
 *
 *  Base class for all factories of G4 objects which implements
 *  the IGiGaMessage interface to allow the use of the Gaudi
 *  messaging facilities from the factory within the G4 objects.
 *  TODO: That might actually be stupid but whatever ...
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

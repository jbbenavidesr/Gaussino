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

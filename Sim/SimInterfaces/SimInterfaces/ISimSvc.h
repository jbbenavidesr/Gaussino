#pragma once

#include "GaudiKernel/IService.h"
#include <vector>

// Forward declaration
namespace HepMC {
  class GenEvent;
}

namespace CLHEP {
  class HepRandomEngine;
}

/** @class ISimSvc ISimSvc.h "GenInterfaces/ISimSvc.h"
 *  
 *  Basic interface for simulation services
 * 
 *  @author Dominik Muller
 *  @date   2018-05-24
 */

static const InterfaceID IID_ISimSvc( "ISimSvc" , 2 , 
                                                  0 ) ;

class ISimSvc : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_ISimSvc ; }
  
  /// Simulate the HepMC events
  virtual StatusCode simulate( const std::vector<HepMC3::GenEvent> & theEvents , CLHEP::HepRandomEngine & engine ) = 0 ;
};

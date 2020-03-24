#ifndef MCINTERFACES_IFULLGENEVENTCUTTOOL_H 
#define MCINTERFACES_IFULLGENEVENTCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"
#include "Event/GenCollision.h"

#include <vector>

/** @class IFullGenEventCutTool IFullGenEventCutTool.h "MCInterfaces/IFullGenEventCutTool.h"
 *  
 *  Abstract interface to generator level cut on full event. This type of
 *  cut is applied on a fully generated event: it includes pile-up interactions
 *  and all particles have already been decayed.
 * 
 *  @author Patrick Robbe
 *  @date   2005-11-17
 */

class IFullGenEventCutTool : public extend_interfaces<IAlgTool> {
  public:

  DeclareInterfaceID( IFullGenEventCutTool , 3 , 0 ) ;

  /** Apply the cut on a event.
   *  @param[in] theEvents      Container of all interactions in the event.
   *  @param[in] theCollisions  Container of hard process informations of each 
   *                            pile-up interactions of the event.
   *  @return    true  if the full event passes the cut.
   */  
  virtual bool studyFullEvent( const HepMC3::GenEventPtrs & theEvents ,
                               const LHCb::GenCollisions & theCollisions ) 
    const = 0 ;
};
#endif // MCINTERFACES_ICUTTOOL_H

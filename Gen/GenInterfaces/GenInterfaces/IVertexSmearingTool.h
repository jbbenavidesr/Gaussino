// $Id: IVertexSmearingTool.h,v 1.3 2005-12-31 17:30:37 robbep Exp $
#ifndef GENERATORS_IVERTEXSMEARINGTOOL_H 
#define GENERATORS_IVERTEXSMEARINGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "NewRnd/RndCommon.h"
#include "HepMCUser/typedefs.h"

namespace CLHEP {
  class HepRandomEngine;
}

/** @class IVertexSmearingTool IVertexSmearingTool.h "GenInterfaces/IVertexSmearingTool.h"
 *  
 *  Abstract interface to vertex smearing tools. Concrete implementations 
 *  apply vertex smearing algorithms to each generated pile-up interactions.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IVertexSmearingTool( "IVertexSmearingTool" , 2 , 
                                                  0 ) ;

class IVertexSmearingTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IVertexSmearingTool ; }
  
  /// Smear the vertex of the interaction (independantly of the others)
  virtual StatusCode smearVertex( HepMC3::GenEventPtr theEvent , HepRandomEnginePtr & engine ) = 0 ;
};
#endif // GENERATORS_ISMEARINGTOOL_H

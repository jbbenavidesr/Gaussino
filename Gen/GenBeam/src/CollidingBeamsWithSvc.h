#pragma once

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "GenInterfaces/IBeamTool.h"

// Forward declarations
class IRndmGenSvc ;
class IBeamInfoSvc;

/** @class CollidingBeamsWithSvc CollidingBeamsWithSvc.h "CollidingBeamsWithSvc.h"
 *  
 *  Tool to compute colliding beams values. Concrete implementation
 *  of a beam tool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class CollidingBeamsWithSvc : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  CollidingBeamsWithSvc( const std::string& type, const std::string& name,
                  const IInterface* parent ) ;
  
  virtual ~CollidingBeamsWithSvc( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  
  
  /** Implements IBeamTool::getMeanBeams
   */
  virtual void getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                             Gaudi::XYZVector & pBeam2 ) const ;
  
  /** Implements IBeamTool::getBeams
   *  Compute beam 3-momentum taking into account the horizontal and vertical
   *  beam angles (given by job options). These angles are Gaussian-smeared
   *  with an angular smearing equal to (emittance/beta*)^1/2.
   */
  virtual void getBeams( Gaudi::XYZVector & pBeam1 , 
                         Gaudi::XYZVector & pBeam2 ) ;

 private:
  Rndm::Numbers m_gaussianDist ; ///< Gaussian random number generator
  IBeamInfoSvc *m_beaminfosvc;
};

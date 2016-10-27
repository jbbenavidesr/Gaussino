#ifndef GENERATORS_ASYMMETRICCOLLIDINGBEAMS_H 
#define GENERATORS_ASYMMETRICCOLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

// Forward declarations
class IRndmGenSvc ;

/** @class AsymmetricCollidingBeams AsymmetricCollidingBeams.h "AsymmetricCollidingBeams.h"
 *  
 *  Tool to compute colliding beams values, with asymmetric beams. 
 *  Concrete implementation
 *  of a beam tool.
 * 
 *  @author Patrick Robbe
 *  @date   2016-10-27
 */
class AsymmetricCollidingBeams : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  AsymmetricCollidingBeams( const std::string& type, const std::string& name,
                            const IInterface* parent ) ;
  
  virtual ~AsymmetricCollidingBeams( ); ///< Destructor
  
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
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)
  double m_beam2_zMomentum ; ///< Energy of the second beam (beam 2)
 
  Rndm::Numbers m_gaussianDist ; ///< Gaussian random number generator
};
#endif // GENERATORS_ASYMMETRICCOLLIDINGBEAMS_H

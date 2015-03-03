// $Id: FlatSmearVertex.h,v 1.1 2008-05-19 10:11:24 robbep Exp $
#ifndef PARTICLEGUNS_FLATSMEARVERTEX_H 
#define PARTICLEGUNS_FLATSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/IVertexSmearingTool.h"

/** @class FlatSmearVertex FlatSmearVertex.h "FlatSmearVertex.h"
 *  
 *  Tool to smear vertex with flat smearing along the x- y- and z-axis.
 *  Concrete implementation of a IVertexSmearingTool.
 * 
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */
class FlatSmearVertex : public GaudiTool, virtual public IVertexSmearingTool {
 public:
  /// Standard constructor
  FlatSmearVertex( const std::string& type , const std::string& name,
                    const IInterface* parent ) ;
  
  virtual ~FlatSmearVertex( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Implements IVertexSmearingTool::smearVertex.
   */
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent ) ;
  
 private:
  /// Minimum value for the x coordinate of the vertex (set by options)
  double m_xmin   ;

  /// Minimum value for the y coordinate of the vertex (set by options)
  double m_ymin   ;
 
  /// Minimum value for the z coordinate of the vertex (set by options)
  double m_zmin   ;
  
  /// Maximum value for the x coordinate of the vertex (set by options)
  double m_xmax   ;

  /// Maximum value for the y coordinate of the vertex (set by options)
  double m_ymax   ;

  /// Maximum value for the z coordinate of the vertex (set by options)
  double m_zmax   ;

  Rndm::Numbers m_flatDist ; ///< Flat random number generator
};
#endif // PARTICLEGUNS_FLATSMEARVERTEX_H

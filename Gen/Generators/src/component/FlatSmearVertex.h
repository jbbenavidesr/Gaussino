// $Id: FlatSmearVertex.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_FLATSMEARVERTEX_H 
#define PARTICLEGUNS_FLATSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IVertexSmearingTool.h"

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
  virtual StatusCode initialize( ) override;
  
  /** Implements IVertexSmearingTool::smearVertex.
   */
  virtual StatusCode smearVertex( HepMC3::GenEventPtr theEvent , HepRandomEnginePtr & engine ) override;
  
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

  /// Direction of the beam to take into account TOF vs nominal IP8, can have
  /// only values -1 or 1, or 0 to switch off the TOF and set time of 
  /// interaction to zero (default = 1, as for beam 1)
  int m_zDir;

  bool m_tilt;
  double m_tiltAngle;
};
#endif // PARTICLEGUNS_FLATSMEARVERTEX_H

#pragma once

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IVertexSmearingTool.h"

class IBeamInfoSvc;

/** @class BeamSpotSmearVertexWithSvc BeamSpotSmearVertexWithSvc.h "BeamSpotSmearVertexWithSvc.h"
 *  
 *  VertexSmearingTool to smear vertex according to beam spot parameters.
 *  Concrete implementation of IVertexSmearingTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class BeamSpotSmearVertexWithSvc : public GaudiTool, 
                            virtual public IVertexSmearingTool {
public:
  /// Standard constructor
  BeamSpotSmearVertexWithSvc( const std::string& type, const std::string& name,
                       const IInterface* parent);
  
  virtual ~BeamSpotSmearVertexWithSvc( ); ///< Destructor

  /// Initialize function
  virtual StatusCode initialize( ) override;

  /** Implementation of IVertexSmearingTool::smearVertex.
   *  Gaussian smearing of spatial position of primary event truncated
   *  at a given number of sigma. 
   */
  virtual StatusCode smearVertex( HepMC3::GenEventPtr theEvent , HepRandomEnginePtr & engine ) override;
  
 private:
  /// Number of sigma above which to cut for x-axis smearing (set by options)
  double m_xcut   ;

  /// Number of sigma above which to cut for y-axis smearing (set by options)
  double m_ycut   ;

  /// Number of sigma above which to cut for z-axis smearing (set by options)
  double m_zcut   ;

  /// Sign of time of interaction as given from position with respect to 
  /// origin
  int  m_timeSignVsT0 ;

  IBeamInfoSvc *m_beaminfosvc;
};

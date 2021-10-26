/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// $Id: BeamSpotSmearVertex.h,v 1.7 2010-05-09 17:05:43 gcorti Exp $
#ifndef GENERATORS_BEAMSPOTSMEARVERTEX_H 
#define GENERATORS_BEAMSPOTSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IVertexSmearingTool.h"

/** @class BeamSpotSmearVertex BeamSpotSmearVertex.h "BeamSpotSmearVertex.h"
 *  
 *  VertexSmearingTool to smear vertex according to beam spot parameters.
 *  Concrete implementation of IVertexSmearingTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class BeamSpotSmearVertex : public GaudiTool, 
                            virtual public IVertexSmearingTool {
public:
  /// Standard constructor
  BeamSpotSmearVertex( const std::string& type, const std::string& name,
                       const IInterface* parent);
  
  virtual ~BeamSpotSmearVertex( ); ///< Destructor

  /// Initialize function
  virtual StatusCode initialize( ) ;

  /** Implementation of IVertexSmearingTool::smearVertex.
   *  Gaussian smearing of spatial position of primary event truncated
   *  at a given number of sigma. 
   */
  virtual StatusCode smearVertex( HepMC3::GenEventPtr theEvent , HepRandomEnginePtr & engine ) ;
  
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

  std::string m_beamParameters ; ///< Location of beam parameters (set by options)
  
};
#endif // GENERATORS_BEAMSPOTSMEARVERTEX_H

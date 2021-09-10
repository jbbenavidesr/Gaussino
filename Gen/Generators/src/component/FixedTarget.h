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
// $Id: FixedTarget.h,v 1.3 2005-12-31 17:32:01 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IBeamTool.h"

/** @class FixedTarget FixedTarget.h "FixedTarget.h"
 *  
 *  Tool to compute beam values with only one beam colliding to a fixed
 *  target. Concrete implementation of a IBeamTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class FixedTarget : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  FixedTarget( const std::string& type , const std::string& name,
               const IInterface* parent ) ;
  
  virtual ~FixedTarget( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  
  
  /// Implements IBeamTool::getMeanBeams. See CollidingBeams::getMeanBeams
  virtual void getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                             Gaudi::XYZVector & pBeam2 ) const ;
  
  /// Implements IBeamTool::getBeams. See CollidingBeams::getBeams
  virtual void getBeams( Gaudi::XYZVector & pBeam1 , 
                         Gaudi::XYZVector & pBeam2 ) ;
  
 private:
  std::string m_beamParameters ; ///< Location of beam parameters (set by options)
};
#endif // GENERATORS_FIXEDTARGET_H

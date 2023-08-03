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
#ifndef GENERATORS_ASYMMETRICCOLLIDINGBEAMS_H
#define GENERATORS_ASYMMETRICCOLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "GenInterfaces/IBeamTool.h"

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
  AsymmetricCollidingBeams( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~AsymmetricCollidingBeams(); ///< Destructor

  /** Implements IBeamTool::getMeanBeams
   */
  virtual void getMeanBeams( Gaudi::XYZVector& pBeam1, Gaudi::XYZVector& pBeam2 ) const override;

  /** Implements IBeamTool::getBeams
   *  Compute beam 3-momentum taking into account the horizontal and vertical
   *  beam angles (given by job options). These angles are Gaussian-smeared
   *  with an angular smearing equal to (emittance/beta*)^1/2.
   */
  virtual void getBeams( Gaudi::XYZVector& pBeam1, Gaudi::XYZVector& pBeam2 ) override;

private:
  std::string m_beamParameters;  ///< Location of beam parameters (set by options)
  double      m_beam2_zMomentum; ///< Energy of the second beam (beam 2)
};
#endif // GENERATORS_ASYMMETRICCOLLIDINGBEAMS_H

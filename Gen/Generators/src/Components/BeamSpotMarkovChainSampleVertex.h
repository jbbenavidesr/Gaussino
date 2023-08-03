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
#ifndef GENERATORS_BeamSpotMarkovChainSampleVertex_H
#define GENERATORS_BeamSpotMarkovChainSampleVertex_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// from Event
#include "Event/BeamParameters.h"

#include "GenInterfaces/IVertexSmearingTool.h"

namespace HepMC3 {
  class FourVector;
}

/** @class BeamSpotMarkovChainSampleVertex BeamSpotMarkovChainSampleVertex.h
 *
 *  VertexSmearingTool to sample the (x,y,z,t) parameters from a 4D PDF
 *  that describes the intersection of two bunches.
 *
 *  @author Floris Keizer
 *  @author Chris Jones
 *  @date   2016-10-10
 */
class BeamSpotMarkovChainSampleVertex : public GaudiTool, virtual public IVertexSmearingTool {

public:
  /// Standard constructor
  BeamSpotMarkovChainSampleVertex( const std::string& type, const std::string& name, const IInterface* parent );

  /** Implementation of IVertexSmearingTool::smearVertex.
   *  Gaussian smearing of spatial position of primary event truncated
   *  at a given number of sigma.
   */
  virtual StatusCode smearVertex( HepMC3::GenEventPtr theEvent, HepRandomEnginePtr& engine ) override;

private:
  //=============================================================================
  // Test function to try Markov chain
  //=============================================================================
  // inline double tophat( const HepMC3::FourVector& x ) const noexcept
  // {
  //   return ( (fabs(x.x())<2.0) &&
  //            (fabs(x.y())<2.0) &&
  //            (fabs(x.z())<2.0) &&
  //            (fabs(x.t())<2.0) ? 2.0 : 0.000001 );
  // }

  //=============================================================================
  // Test function to try Markov chain
  //=============================================================================
  // double triangle( const HepMC3::FourVector& x ) const noexcept
  // {
  //   return ( x.t() < 0.0 ? 0.0 :
  //            x.t() < 2.0 ? 2.0 - x.t() :
  //            0.0 );
  // }

  /// Probability distribution in 4D
  double gauss4D( LHCb::BeamParameters* beamp, const HepMC3::FourVector& vec ) const;

private:
  std::string m_beamParameters; ///< Location of beam parameters (set by options)

  /// Number of Markov Chain sampling iterations
  unsigned int m_nMCSamples;

  /// Number of sigma above which to cut for x-axis smearing (set by options)
  double m_xcut;
  /// Number of sigma above which to cut for y-axis smearing (set by options)
  double m_ycut;
  /// Number of sigma above which to cut for z-axis smearing (set by options)
  double m_zcut;
};

#endif // GENERATORS_BeamSpotMarkovChainSampleVertex_H

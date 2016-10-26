// $Id: BeamSpotMarkovChainSampleVertex.h,v 1.7 2010-05-09 17:05:43 gcorti Exp $
#ifndef GENERATORS_BEAMSPOTSMEARVERTEX_H 
#define GENERATORS_BEAMSPOTSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/PhysicalConstants.h" 

// from Event
#include "Event/HepMCEvent.h"
#include "Event/BeamParameters.h"

#include "Generators/IVertexSmearingTool.h"

/** @class BeamSpotMarkovChainSampleVertex BeamSpotMarkovChainSampleVertex.h 
 *  
 *  VertexSmearingTool to sample the (x,y,z,t) parameters from a 4D PDF
 *  that describes the intersection of two bunches.
 * 
 *  @author Floris Keizer
 *  @author Chris Jones
 *  @date   2016-10-10
 */
class BeamSpotMarkovChainSampleVertex final : public GaudiTool, 
                                              virtual public IVertexSmearingTool
{

public:

  /// Standard constructor
  BeamSpotMarkovChainSampleVertex( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );
 

  /// Initialize function
  virtual StatusCode initialize( ) ;

  /** Implementation of IVertexSmearingTool::smearVertex.
   *  Gaussian smearing of spatial position of primary event truncated
   *  at a given number of sigma. 
   */
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent ) override;

 private:

  //=============================================================================
  // Test function to try Markov chain
  //=============================================================================
  // inline double tophat( const HepMC::FourVector& x ) const noexcept
  // {
  //   return ( (fabs(x.x())<2.0) && 
  //            (fabs(x.y())<2.0) && 
  //            (fabs(x.z())<2.0) && 
  //            (fabs(x.t())<2.0) ? 2.0 : 0.000001 );
  // }
  
  //=============================================================================
  // Test function to try Markov chain 
  //=============================================================================
  // double triangle( const HepMC::FourVector& x ) const noexcept
  // {
  //   return ( x.t() < 0.0 ? 0.0 : 
  //            x.t() < 2.0 ? 2.0 - x.t() :
  //            0.0 );
  // }

  /// Probability distribution in 4D
  double gauss4D( LHCb::BeamParameters * beamp ,
                  const HepMC::FourVector & vec ) const;
  
 private:

  std::string m_beamParameters ; ///< Location of beam parameters (set by options)

  //  Rndm::Numbers m_gaussDist ; ///< Gaussian random number generator
  Rndm::Numbers m_gaussDistX ; ///< Gaussian random number generator for Markov chain pertubation in x
  Rndm::Numbers m_gaussDistY ; ///< Gaussian random number generator for Markov chain pertubation in y
  Rndm::Numbers m_gaussDistZ ; ///< Gaussian random number generator for Markov chain pertubation in z
  Rndm::Numbers m_gaussDistT ; ///< Gaussian random number generator for Markov chain pertubation in t
  Rndm::Numbers m_flatDist ; ///< Random number generator (between 0 and 1)

};
#endif // GENERATORS_BEAMSPOTSMEARVERTEX_H

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

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "GenBeam/IBeamInfoSvc.h"
#include "GenInterfaces/IVertexSmearingTool.h"

#include "HepMC3/FourVector.h"
#include "HepMC3/GenEvent.h"

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandomEngine.h"
#include "Math/CholeskyDecomp.h"
#include "Math/SMatrix.h"

/** @class BeamSpot4DWithSvc BeamSpot4DWithSvc.h
 *
 *  VertexSmearingTool to sample the (x,y,z,t) parameters from a 4D PDF
 *  that describes the intersection of two bunches.
 *
 *  @author Tim Evans
 *  @date   2019-09-06
 */

//=============================================================================
// Generator of N-dimensional Gaussian distributions from covariance matrix and mean
//=============================================================================
template <std::size_t N, typename real_t = double>
struct GaussGenerator {
  GaussGenerator( const ROOT::Math::SMatrix<real_t, N, N>& covMatrix,
                  const ROOT::Math::SVector<real_t, N>&    mean = ROOT::Math::SVector<real_t, N>() )
      : m_mean( mean ) {
    ROOT::Math::CholeskyDecomp<real_t, N> recomp( covMatrix );
    recomp.getL( m_decomp );
  }
  std::array<real_t, N> operator()( CLHEP::RandGauss& rnd ) const {
    std::array<real_t, N> p{ 0 };
    std::array<real_t, N> rt{ 0 };
    for ( unsigned i = 0; i != N * N; ++i ) {
      if ( i % N == 0 ) {
        p[i / N] = rnd();
        rt[i / N] += m_mean[i / N];
      }
      rt[i % N] += m_decomp( i % N, i / N ) * p[i / N];
    }
    return rt;
  }
  ROOT::Math::SMatrix<real_t, N, N> m_decomp;
  ROOT::Math::SVector<real_t, N>    m_mean;
};

struct BeamSpot4DWithSvc final : public extends<GaudiTool, IVertexSmearingTool> {
  using extends::extends;
  virtual StatusCode smearVertex( HepMC3::GenEventPtr theEvent, HepRandomEnginePtr& engine ) override;
  GaussGenerator<4>  calculateInteractionDistribution() const;

private:
  ServiceHandle<IBeamInfoSvc> m_beaminfoSvc{ this, "BeamInfoSvc", "BeamInfoSvc" };
};

// Declaration of the Tool Factory
DECLARE_COMPONENT( BeamSpot4DWithSvc )

ROOT::Math::SMatrix<double, 3, 3> rotX( const double angle ) {
  ROOT::Math::SMatrix<double, 3, 3> Rx;
  Rx( 0, 0 ) = 1.0;
  Rx( 1, 1 ) = Rx( 2, 2 ) = cos( angle );
  Rx( 1, 2 )              = -sin( angle );
  Rx( 2, 1 )              = +sin( angle );
  return Rx;
}

ROOT::Math::SMatrix<double, 3, 3> rotY( const double angle ) {
  ROOT::Math::SMatrix<double, 3, 3> Ry;
  Ry( 0, 0 ) = Ry( 2, 2 ) = cos( angle );
  Ry( 1, 1 )              = 1;
  Ry( 2, 0 )              = -sin( angle );
  Ry( 0, 2 )              = +sin( angle );
  return Ry;
}

//=============================================================================
// Calculate covariance matrix of 4D gaussian distribution for interactions
//=============================================================================

GaussGenerator<4> BeamSpot4DWithSvc::calculateInteractionDistribution() const {
  const auto emittance = m_beaminfoSvc->emittance();
  const auto betastarx = m_beaminfoSvc->betaStarX();
  const auto betastary = m_beaminfoSvc->betaStarY();
  const auto ha        = m_beaminfoSvc->horizontalCrossingAngle();
  const auto va        = m_beaminfoSvc->verticalCrossingAngle();
  const auto ha_bl     = m_beaminfoSvc->horizontalBeamlineAngle();
  const auto va_bl     = m_beaminfoSvc->verticalBeamlineAngle();
  const auto sb        = m_beaminfoSvc->sigmaS();
  const auto c         = Gaudi::Units::c_light;
  // const auto st        = std::sqrt( emittance * betastar );

  const auto sx = std::sqrt( emittance * betastarx );
  const auto sy = std::sqrt( emittance * betastary );
  debug() << "Transverse beam size = " << sx << " mm x << " << sy << "mm" << endmsg;
  debug() << "Bunch RMS = " << sb << "mm" << endmsg;
  debug() << "Emittance = " << emittance << endmsg;
  debug() << "BetastarX  = " << betastarx << "mm" << endmsg;
  debug() << "BetastarY  = " << betastary << "mm" << endmsg;
  debug() << "Half Crossing angles: " << ha << " " << va << endmsg;
  debug() << "Beam line angles: " << ha_bl << " " << va_bl << endmsg;
  // this is the (inverse) covariance matrix of each beam
  ROOT::Math::SMatrix<double, 3, 3> c1;
  c1( 0, 0 ) = 1. / ( sx * sx );
  c1( 1, 1 ) = 1. / ( sy * sy );
  c1( 2, 2 ) = 1. / ( sb * sb );
  auto c2    = c1;

  // Crossing in the horizontal plane corresponds to a rotation about the y axis,
  // and crossing in the vertical plane corresponds to a rotation about the x axis
  // By convention, a positive angle means that beam1 is
  // moving from - to + in the transverse plane, which corresponds to a rotation of +(-) the
  // horizontal (vertical) crossing angle for beam 1
  // Both beams are also rotated by the overall angle of the beam axis with respect to LHCb
  // health warning: auto + ROOT::Math::SMatrix produces strange results, so types are used explicitly here

  ROOT::Math::SMatrix<double, 3, 3> r1 = rotY( ha + ha_bl ) * rotX( -va - va_bl );
  ROOT::Math::SMatrix<double, 3, 3> r2 = rotY( -ha + ha_bl ) * rotX( va - va_bl );

  ROOT::Math::SMatrix<double, 3, 3> r1_T = ROOT::Math::Transpose( r1 );
  ROOT::Math::SMatrix<double, 3, 3> r2_T = ROOT::Math::Transpose( r2 );

  // both (inverse) covariance matrices are rotated by the crossing angles
  ROOT::Math::SMatrix<double, 3, 3> s1 = r1 * c1 * r1_T;
  ROOT::Math::SMatrix<double, 3, 3> s2 = r2 * c2 * r2_T;
  // From the (inverse) covariance matrix of the beams, we can determine the (inverse) covariance matrix of the
  // interactions (its just their sum) + the components in time
  ROOT::Math::SMatrix<double, 4, 4> D;
  for ( unsigned i = 0; i < 3; ++i ) {
    for ( unsigned j = 0; j < 3; ++j ) {
      D( i, j ) = s1( i, j ) + s2( i, j );
      // these terms are much simpler really as c1/c2 are diagonal, but left in for completeness,
      // or in case we want to generalise to elliptical beams
      D( 3, i ) += ( r1( i, j ) * c1( j, 2 ) - r2( i, j ) * c2( j, 2 ) ) * c;
      D( i, 3 ) += ( r1( i, j ) * c1( j, 2 ) - r2( i, j ) * c2( j, 2 ) ) * c;
    }
  }
  D( 3, 3 ) = c * c * ( c1( 2, 2 ) + c2( 2, 2 ) );

  D.Invert();
  debug() << "Luminous Region : [" << sqrt( D( 0, 0 ) ) << "mm, " << sqrt( D( 1, 1 ) ) << "mm, " << sqrt( D( 2, 2 ) )
          << "mm, " << sqrt( D( 3, 3 ) ) << "ns ]" << endmsg;
  ROOT::Math::SVector<double, 4> mu( m_beaminfoSvc->beamSpot().x(), m_beaminfoSvc->beamSpot().y(),
                                     m_beaminfoSvc->beamSpot().z(), 0 );
  return GaussGenerator<4>( D, mu );
}

//=============================================================================
// 'Smear' vertices by PV position
//=============================================================================
StatusCode BeamSpot4DWithSvc::smearVertex( HepMC3::GenEventPtr theEvent, HepRandomEnginePtr& engine ) {
  auto               genPV = calculateInteractionDistribution();
  CLHEP::RandGauss   gaussDist{ engine.getref(), 0, 1 };
  auto               d = genPV( gaussDist );
  HepMC3::FourVector dpos( d[0], d[1], d[2], d[3] );
  theEvent->shift_position_by( dpos );
  return StatusCode::SUCCESS;
}

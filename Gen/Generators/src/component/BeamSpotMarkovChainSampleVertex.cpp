
// local
#include "BeamSpotMarkovChainSampleVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2016-10-10 : Floris Keizer
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamSpotMarkovChainSampleVertex::
BeamSpotMarkovChainSampleVertex( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface< IVertexSmearingTool >( this ) ;
  declareProperty( "Xcut" , m_xcut = 4. ) ; // times SigmaX 
  declareProperty( "Ycut" , m_ycut = 4. ) ; // times SigmaY
  declareProperty( "Zcut" , m_zcut = 4. ) ; // times SigmaZ
  declareProperty( "BeamParameters" , 
                   m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
  declareProperty( "NMarkovChainSamples", m_nMCSamples = 1000 );
}

//=============================================================================
// Initialize 
//=============================================================================
StatusCode BeamSpotMarkovChainSampleVertex::initialize( ) 
{
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;

  // Gaussians used to perturb the four-vector in the Markov chain
  // The standard deviation of the pertubation should reflect (about 10%) 
  // the width of the function that is sampled from.
  // However, it is not sensitive to the precise value... changing for example 
  // the sigma in m_gaussDistZ from 5. to 7. will not make a difference.

  sc = ( sc 
         && m_gaussDistX.initialize( randSvc , Rndm::Gauss( 0. , 0.025  ) )
         && m_gaussDistY.initialize( randSvc , Rndm::Gauss( 0. , 0.025  ) ) 
         && m_gaussDistZ.initialize( randSvc , Rndm::Gauss( 0. , 5.     ) ) 
         && m_gaussDistT.initialize( randSvc , Rndm::Gauss( 0. , 1.     ) ) 
         && m_flatDist.initialize  ( randSvc , Rndm::Flat ( 0. , 1.     ) ) );
  if ( sc.isFailure() ) 
  { return Error( "Could not initialize random number generators" ); }

  release( randSvc ) ;

  return sc ; 
}

//=============================================================================
// Function representing the product of two 4D Gaussian PDFs (Floris)
//=============================================================================
double BeamSpotMarkovChainSampleVertex::gauss4D( LHCb::BeamParameters * beamp ,
                                                 const HepMC::FourVector & vec ) const
{
  const auto emittance = beamp -> emittance();
  const auto betastar  = beamp -> betaStar();
  const auto aX        = beamp -> horizontalCrossingAngle();
  const auto aY        = beamp -> verticalCrossingAngle();

  const auto c  = Gaudi::Units::c_light ;
  const auto Pi = Gaudi::Units::pi;

  const auto sx = std::sqrt( emittance*betastar );
  const auto sy = sx;
  const auto sz = beamp -> sigmaS();// RMS bunch length in mm

  const auto x = vec.x();
  const auto y = vec.y();
  const auto z = vec.z();
  const auto t = vec.t();

  const auto sX2 = std::pow( sx ,2 );
  const auto sY2 = std::pow( sy ,2 );
  const auto sZ2 = std::pow( sz ,2 );
  const auto x2 = std::pow( x , 2 );
  const auto y2 = std::pow( y , 2 );
  const auto z2 = std::pow( z , 2 );
  const auto t2 = std::pow( t , 2 );
  const auto c2 = std::pow(c,2);

  const auto c2m3 = std::cos(2*aX - 3*aY);
  const auto c4m2 = std::cos(4*aX - 2*aY);
  const auto c04 = std::cos(4*aY);
  const auto c20 = std::cos(2*aX); 
  const auto c40 = std::cos(4*aX);
  const auto c2m2 = std::cos(2*(aX - aY));
  const auto c2m1 = std::cos(2*aX - aY);
  const auto c4m1 = std::cos(4*aX - aY);
  const auto c01 = std::cos(aY);
  const auto c02 = std::cos(2*aY);
  const auto c03 = std::cos(3*aY);
  const auto c2p2 = std::cos(2*(aX + aY));
  const auto c4p4 = std::cos(4*(aX + aY));
  const auto c4p2 = std::cos(2*(2*aX + aY)); 
  const auto c2p4 = std::cos(2*(aX + 2*aY));
  const auto c2p3 = std::cos(2*aX + 3*aY);
  const auto c4p3 = std::cos(4*aX + 3*aY);
  const auto c2p1 = std::cos(2*aX + aY);
  const auto s10 = std::sin(aX);
  const auto s30 = std::sin(3*aX);
  const auto s1m3 = std::sin(aX - 3*aY);
  const auto s1m2 = std::sin(aX - 2*aY);
  const auto s3m2 = std::sin(3*aX - 2*aY);
  const auto s1m1 = std::sin(aX - aY);
  const auto s3m1 = std::sin(3*aX - aY);
  const auto s1p1 = std::sin(aX + aY);
  const auto s3p3 = std::sin(3*(aX + aY));
  const auto s3p1 = std::sin(3*aX + aY);
  const auto s1p2 = std::sin(aX + 2*aY);
  const auto s3p2 = std::sin(3*aX + 2*aY);
  const auto s1p3 = std::sin(aX + 3*aY);

  // finally, return the function...
  // This is computed from the Mathematica notebook primaryVertexPDF.nb
  return std::exp((-700 - (800*sZ2)/sX2 - (900*sZ2)/sY2 + (140*c*t)/sz + (160*c*sz*t)/sX2 + (180*c*sz*t)/sY2 - (8*c2*t2)/sX2 - (9*c2*t2)/sY2 - (7*c2*t2)/sZ2 - (16*x2)/sX2 - (8*x2)/sY2 - (8*x2)/sZ2 - (16*y2)/sY2 - (16*y2)/sZ2 - (16*z2)/sX2 - (8*z2)/sY2 - (8*z2)/sZ2 - (2*(-2*sY2*sZ2*std::pow(-10*sz + c*t,2) + sX2*(sZ2*(300*sZ2 - 60*c*sz*t + 3*c2*t2 - 4*(x2 - 2*y2 + z2)) + sY2*(300*sZ2 - 60*c*sz*t + 3*c2*t2 + 4*(x2 - 2*y2 + z2))))*c20)/(sX2*sY2*sZ2) + ((sY2 - sZ2)*std::pow(-10*sz + c*t,2)*c40)/(sY2*sZ2) + 200*c2m3 - (200*sZ2*c2m3)/sY2 - (40*c*t*c2m3)/sz + (40*c*sz*t*c2m3)/sY2 - (2*c2*t2*c2m3)/sY2 + (2*c2*t2*c2m3)/sZ2 - 100*c4m2 + (100*sZ2*c4m2)/sY2 + (20*c*t*c4m2)/sz - (20*c*sz*t*c4m2)/sY2 + (c2*t2*c4m2)/sY2 - (c2*t2*c4m2)/sZ2 - 200*c2m2 + (600*sZ2*c2m2)/sY2 + (40*c*t*c2m2)/sz - (120*c*sz*t*c2m2)/sY2 + (6*c2*t2*c2m2)/sY2 - (2*c2*t2*c2m2)/sZ2 - (4*x2*c2m2)/sY2 + (4*x2*c2m2)/sZ2 + (4*z2*c2m2)/sY2 - (4*z2*c2m2)/sZ2 + 200*c2m1 - (200*sZ2*c2m1)/sY2 - (40*c*t*c2m1)/sz + (40*c*sz*t*c2m1)/sY2 - (2*c2*t2*c2m1)/sY2 + (2*c2*t2*c2m1)/sZ2 + (16*x*y*c2m1)/sY2 - (16*x*y*c2m1)/sZ2 + 200*c4m1 - (200*sZ2*c4m1)/sY2 - (40*c*t*c4m1)/sz + (40*c*sz*t*c4m1)/sY2 - (2*c2*t2*c4m1)/sY2 + (2*c2*t2*c4m1)/sZ2 - 200*c01 + (200*sZ2*c01)/sY2 + (40*c*t*c01)/sz - (40*c*sz*t*c01)/sY2 + (2*c2*t2*c01)/sY2 - (2*c2*t2*c01)/sZ2 - 400*c02 - (800*sZ2*c02)/sX2 + (400*sZ2*c02)/sY2 + (80*c*t*c02)/sz + (160*c*sz*t*c02)/sX2 - (80*c*sz*t*c02)/sY2 - (8*c2*t2*c02)/sX2 + (4*c2*t2*c02)/sY2 - (4*c2*t2*c02)/sZ2 - (16*x2*c02)/sX2 + (8*x2*c02)/sY2 + (8*x2*c02)/sZ2 + (16*z2*c02)/sX2 - (8*z2*c02)/sY2 - (8*z2*c02)/sZ2 + 200*c03 - (200*sZ2*c03)/sY2 - (40*c*t*c03)/sz + (40*c*sz*t*c03)/sY2 - (2*c2*t2*c03)/sY2 + (2*c2*t2*c03)/sZ2 - 100*c04 + (100*sZ2*c04)/sY2 + (20*c*t*c04)/sz - (20*c*sz*t*c04)/sY2 + (c2*t2*c04)/sY2 - (c2*t2*c04)/sZ2 - 600*c2p2 + (800*sZ2*c2p2)/sX2 + (200*sZ2*c2p2)/sY2 + (120*c*t*c2p2)/sz - (160*c*sz*t*c2p2)/sX2 - (40*c*sz*t*c2p2)/sY2 + (8*c2*t2*c2p2)/sX2 + (2*c2*t2*c2p2)/sY2 - (6*c2*t2*c2p2)/sZ2 - (4*x2*c2p2)/sY2 + (4*x2*c2p2)/sZ2 + (4*z2*c2p2)/sY2 - (4*z2*c2p2)/sZ2 - 100*c4p4 + (100*sZ2*c4p4)/sY2 + (20*c*t*c4p4)/sz - (20*c*sz*t*c4p4)/sY2 + (c2*t2*c4p4)/sY2 - (c2*t2*c4p4)/sZ2 - 200*c2p1 + (200*sZ2*c2p1)/sY2 + (40*c*t*c2p1)/sz - (40*c*sz*t*c2p1)/sY2 + (2*c2*t2*c2p1)/sY2 - (2*c2*t2*c2p1)/sZ2 - (16*x*y*c2p1)/sY2 + (16*x*y*c2p1)/sZ2 - 300*c4p2 + (300*sZ2*c4p2)/sY2 + (60*c*t*c4p2)/sz - (60*c*sz*t*c4p2)/sY2 + (3*c2*t2*c4p2)/sY2 - (3*c2*t2*c4p2)/sZ2 - 200*c2p4 + (400*sZ2*c2p4)/sX2 - (200*sZ2*c2p4)/sY2 + (40*c*t*c2p4)/sz - (80*c*sz*t*c2p4)/sX2 + (40*c*sz*t*c2p4)/sY2 +  (4*c2*t2*c2p4)/sX2 - (2*c2*t2*c2p4)/sY2 - (2*c2*t2*c2p4)/sZ2 - 200*c2p3 + (200*sZ2*c2p3)/sY2 + (40*c*t*c2p3)/sz - (40*c*sz*t*c2p3)/sY2 + (2*c2*t2*c2p3)/sY2 - (2*c2*t2*c2p3)/sZ2 - 200*c4p3 +  (200*sZ2*c4p3)/sY2 + (40*c*t*c4p3)/sz - (40*c*sz*t*c4p3)/sY2 + (2*c2*t2*c4p3)/sY2 - (2*c2*t2*c4p3)/sZ2 + (80*x*s10)/sz - (80*sz*x*s10)/sY2 + (8*c*t*x*s10)/sY2 - (8*c*t*x*s10)/sZ2 - (80*y*s10)/sz + (80*sz*y*s10)/sY2 - (8*c*t*y*s10)/sY2 + (8*c*t*y*s10)/sZ2 + (80*x*s30)/sz - (80*sz*x*s30)/sY2 + (8*c*t*x*s30)/sY2 - (8*c*t*x*s30)/sZ2 - (80*y*s30)/sz + (80*sz*y*s30)/sY2 - (8*c*t*y*s30)/sY2 + (8*c*t*y*s30)/sZ2 + (40*x*s1m3)/sz - (40*sz*x*s1m3)/sY2 + (4*c*t*x*s1m3)/sY2 - (4*c*t*x*s1m3)/sZ2 - (40*x*s1m2)/sz + (40*sz*x*s1m2)/sY2 - (4*c*t*x*s1m2)/sY2 + (4*c*t*x*s1m2)/sZ2 - (80*y*s1m2)/sz +  (80*sz*y*s1m2)/sY2 - (8*c*t*y*s1m2)/sY2 + (8*c*t*y*s1m2)/sZ2 - (40*x*s3m2)/sz + (40*sz*x*s3m2)/sY2 - (4*c*t*x*s3m2)/sY2 + (4*c*t*x*s3m2)/sZ2 + (80*x*s1m1)/sz + (160*sz*x*s1m1)/sX2 + (80*sz*x*s1m1)/sY2 - (16*c*t*x*s1m1)/sX2 - (8*c*t*x*s1m1)/sY2 - (8*c*t*x*s1m1)/sZ2 - (80*y*s1m1)/sz - (240*sz*y*s1m1)/sY2 + (24*c*t*y*s1m1)/sY2 + (8*c*t*y*s1m1)/sZ2 + (40*x*s3m1)/sz - (40*sz*x*s3m1)/sY2 + (4*c*t*x*s3m1)/sY2 - (4*c*t*x*s3m1)/sZ2 + (80*y*s3m1)/sz - (80*sz*y*s3m1)/sY2 + (8*c*t*y*s3m1)/sY2 - (8*c*t*y*s3m1)/sZ2 - (40*x*s1p1)/sz + (320*sz*x*s1p1)/sX2 + (40*sz*x*s1p1)/sY2 - (32*c*t*x*s1p1)/sX2 - (4*c*t*x*s1p1)/sY2 + (4*c*t*x*s1p1)/sZ2 + (80*y*s1p1)/sz + (240*sz*y*s1p1)/sY2 - (24*c*t*y*s1p1)/sY2 - (8*c*t*y*s1p1)/sZ2 - (40*x*s3p3)/sz + (40*sz*x*s3p3)/sY2 - (4*c*t*x*s3p3)/sY2 + (4*c*t*x*s3p3)/sZ2 - (80*y*s3p1)/sz + (80*sz*y*s3p1)/sY2 - (8*c*t*y*s3p1)/sY2 + (8*c*t*y*s3p1)/sZ2 - (40*x*s1p2)/sz + (40*sz*x*s1p2)/sY2 - (4*c*t*x*s1p2)/sY2 + (4*c*t*x*s1p2)/sZ2 - (40*x*s3p2)/sz + (40*sz*x*s3p2)/sY2 - (4*c*t*x*s3p2)/sY2 + (4*c*t*x*s3p2)/sZ2 - (80*y*s3p2)/sz + (80*sz*y*s3p2)/sY2 - (8*c*t*y*s3p2)/sY2 + (8*c*t*y*s3p2)/sZ2 - (80*x*s1p3)/sz + (160*sz*x*s1p3)/sX2 - (80*sz*x*s1p3)/sY2 - (16*c*t*x*s1p3)/sX2 + (8*c*t*x*s1p3)/sY2 + (8*c*t*x*s1p3)/sZ2)/32.)/(8.*std::pow(Pi,3)*sX2*sY2*sZ2);
}

//=============================================================================
// Markov chain sampler
//=============================================================================
StatusCode BeamSpotMarkovChainSampleVertex::smearVertex( LHCb::HepMCEvent * theEvent )
{
  
  LHCb::BeamParameters * beamp = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( ! beamp ) Exception( "No beam parameters registered" ) ;

  // The sampled point. Always start at origin for reproducibility.
  HepMC::FourVector x( 0 , 0 , 0 , 0 );

  // Repeat until we get a sampled point within the defined (x,,y,z) limits
  unsigned int iLoop = 0; // sanity check to prevent infinite loops...
  bool OK = false;
  while ( iLoop++ < m_nMCSamples && !OK )
  {
    for ( unsigned int repeat = 0; repeat < m_nMCSamples; ++repeat ) 
    {
      // Copute the PDF value for this point
      const auto f = gauss4D( beamp , x );
      
      // smear the point. random walk.
      const HepMC::FourVector y( x.x() + m_gaussDistX(),
                                 x.y() + m_gaussDistY(),
                                 x.z() + m_gaussDistZ(),
                                 x.t() + m_gaussDistT() );    
      
      // compute the prob for the new point
      const auto g = gauss4D( beamp , y );    
      
      if ( f < g ) 
      {
        // new point is better, so always keep
        x = y;
      }
      else 
      {
        // randomly keep worse point a fraction depending on the prob values.
        const auto ratio = ( fabs(f)>0 ? g/f : 0.0 );
        const auto r = m_flatDist( );
        if ( r < ratio ) { x = y; }	
      }
    }

    // Check final if the spatial part of x is within the defined limits.
    OK = ( ( fabs(x.x()) < ( m_xcut * beamp->sigmaX() ) ) &&
           ( fabs(x.y()) < ( m_ycut * beamp->sigmaY() ) ) &&
           ( fabs(x.z()) < ( m_zcut * beamp->sigmaZ() ) ) );

    // reset and repeat
    if ( !OK ) { x = HepMC::FourVector(0,0,0,0); }

  }
  if ( UNLIKELY(!OK) )
  {
    Warning( "Markov Chain sampling for PV (x,y,z,t) failed" ).ignore();
  }
  else
  {

    // A shift to put the mean of the temporal distribution at t=0.// Factor 10 because the bunches 
    // start 10*sigmaZ away from the interaction point.
    const auto timeDelay = 10*(beamp -> sigmaS())/Gaudi::Units::c_light; 
    
    // Shift the sampled point to average beam spot position and time offset
    x.setT( x.t() - timeDelay ); 
    x.setX( x.x() + beamp -> beamSpot().x() ); // Offset the centre of the beamspot
    x.setY( x.y() + beamp -> beamSpot().y() );
    x.setZ( x.z() + beamp -> beamSpot().z() );
    
    // update the values for all vertices
    auto * pEvt = theEvent -> pGenEvt() ;
    if ( pEvt )
    {
      for ( auto vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; ++vit )
      {
        const auto pos = (*vit) -> position() ;
        (*vit) -> set_position( HepMC::FourVector( pos.x() + x.x() , 
                                                   pos.y() + x.y() , 
                                                   pos.z() + x.z() , 
                                                   pos.t() + x.t() ) ) ;
      }
    }
    
  }
  
  return StatusCode::SUCCESS;      
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BeamSpotMarkovChainSampleVertex )

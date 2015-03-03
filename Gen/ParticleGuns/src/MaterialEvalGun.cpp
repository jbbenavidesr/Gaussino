// $Id: MaterialEvalGun.cpp,v 1.5 2007-08-24 17:28:10 gcorti Exp $
// Include files
 
// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"

// From Event
#include "HepMC/GenEvent.h"
#include "Event/HepMCEvent.h"
 
// local
#include "MaterialEvalGun.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MaterialEvalGun
// Description:
//   Allows the user to "shoot" Monte Carlo particles and store the result
//   in the Transient Store.
//
// 2007-08-21 : Gloria Corti
//-----------------------------------------------------------------------------
 
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MaterialEvalGun );

//============================================================================
// Constructor
//============================================================================
MaterialEvalGun::MaterialEvalGun(const std::string& name, 
                                 ISvcLocator* pSvcLocator) 
  : ParticleGunBaseAlg(name, pSvcLocator)
  , m_mass( 0. )
  , m_dXstep( 0. )
  , m_dYstep( 0. )
  , m_dEtastep( 0. )
  , m_dPhistep( 0. )
  , m_counterX( 0 ) 
  , m_counterY( 0 ) 
  , m_counterEta( 0 ) 
  , m_counterPhi( 0 ) {

  // position of the "production point"
  declareProperty( "Xorig" , m_xVtx = 0. * Gaudi::Units::mm );
  declareProperty( "Yorig" , m_yVtx = 0. * Gaudi::Units::mm );
  declareProperty( "Zorig" , m_zVtx = 0. * Gaudi::Units::mm );
    
  // position and size of the "target rectangle"
  declareProperty( "ZPlane" , m_zplane =  10. * Gaudi::Units::m );
  declareProperty( "Xmin"   , m_xmin   = -3.2 * Gaudi::Units::m );
  declareProperty( "Ymin"   , m_ymin   = -2.6 * Gaudi::Units::m );
  declareProperty( "Xmax"   , m_xmax   =  3.2 * Gaudi::Units::m );
  declareProperty( "Ymax"   , m_ymax   =  2.6 * Gaudi::Units::m );
  declareProperty( "ModP"   , m_ptotal = 500. * Gaudi::Units::GeV );
  
  // Boundaries in Eta and Phi
  declareProperty( "MinEta", m_minEta = 2.1 ); 
  declareProperty( "MaxEta", m_maxEta = 4.9 ); 
  declareProperty( "MinPhi", m_minPhi = 0.0 * rad ); 
  declareProperty( "MaxPhi", m_maxPhi = 6.28 * rad ); 

  // Chose if using a grid or randomly flat distribution
  declareProperty( "UseGrid", m_useGrid = false );
  declareProperty( "EtaPhi" , m_etaPhi = false );

  // Number of steps when using a grid
  declareProperty( "NStepsInX"  , m_nXstep = 320);
  declareProperty( "NStepsInY"  , m_nYstep = 260);
  declareProperty( "NStepsInEta", m_nEtastep = 280);
  declareProperty( "NStepsInPhi", m_nPhistep = 314);
  
  //number of particles to be generated and their pdg
  declareProperty( "PartNumber" , m_partNumber = 10 );
  declareProperty( "PdgCode"    , m_pdgCode    = 12 );

}

//=============================================================================
// Destructor
//=============================================================================
MaterialEvalGun::~MaterialEvalGun() { }

//=============================================================================
// Initialize Method
//=============================================================================
StatusCode MaterialEvalGun::initialize() {  

  StatusCode sc = ParticleGunBaseAlg::initialize( );
  if ( sc.isFailure() ) return sc;  
  
  sc = m_flatGenerator.initialize( randSvc() , Rndm::Flat( 0. , 1. ) );
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize random number generator" );
  
  IParticlePropertySvc* ppSvc =  
    svc< IParticlePropertySvc >( "ParticlePropertySvc" , true );

  ParticleProperty* particle = ppSvc -> findByStdHepID( m_pdgCode );
  if ( !particle ) {
    error() << "Information not found for pdgID " << m_pdgCode << endmsg;
    return StatusCode::FAILURE;
  }
  m_mass = particle -> mass();

  release( ppSvc );

  if( m_useGrid ) {
    info() << "Generating in a grid of regular steps in ";
    if( m_etaPhi ) {
      m_dEtastep = (m_maxEta-m_minEta)/m_nEtastep;
      m_dPhistep = (m_maxPhi-m_minPhi)/m_nPhistep;
      info() << "eta-phi plane" << endmsg;
      info() << "  with " << m_nEtastep << " steps in "
             << m_minEta << " <= eta <= " << m_maxEta 
             << " and " << m_nPhistep << " steps in "
             << m_minPhi << " <= phi <= " << m_maxPhi << endmsg;
      info() << "  ==> DEta step = " << m_dEtastep << ", Dphi Step = "  
             << m_dPhistep << endmsg; 
    }
    else { 
      m_dXstep = (m_xmax - m_xmin)/m_nXstep;
      m_dYstep = (m_ymax - m_ymin)/m_nYstep;
      info() << "x-y plane at z = " << m_zplane/Gaudi::Units::m << " m " 
             << endmsg;
      info() << "  with " << m_nXstep << " steps in "
             << m_xmin/Gaudi::Units::cm << " cm <= x <= " 
             << m_xmax/Gaudi::Units::cm << " cm and " 
             << m_nYstep << " steps in "
             << m_ymin/Gaudi::Units::cm << " cm <= y <= " 
             << m_ymax/Gaudi::Units::cm << " cm " << endmsg;
      info() << "  ==> Dx step = " << m_dXstep/Gaudi::Units::cm 
             << " cm , Dy Step = " << m_dYstep/Gaudi::Units::cm << " cm " 
             << endmsg; 
    }
  }
  else {
    info() << "Generating a uniform distribution in ";
    if( m_etaPhi ) {
      info() << "eta-phi plane" << endmsg;
      info() << "  with " 
             << m_minEta << " <= eta <= " << m_maxEta 
             << " and " 
             << m_minPhi << " <= phi <= " << m_maxPhi << endmsg;
    }
    else {
      info() << "x-y plane at z = " << m_zplane /Gaudi::Units::m << " m "
             << endmsg;
      info() << "  with "
             << m_xmin/Gaudi::Units::cm << " cm <= x <= " 
             << m_xmax/Gaudi::Units::cm << " cm and "
             << m_ymin/Gaudi::Units::cm << " cm <= y <= " 
             << m_ymax/Gaudi::Units::cm << " cm " << endmsg;
    }
  }

  return StatusCode::SUCCESS;
  
}

//=============================================================================
// callParticleGun method
//=============================================================================
StatusCode MaterialEvalGun::callParticleGun( HepMC::GenEvent* evt ) {

  HepMC::GenVertex* v1 = 
    new HepMC::GenVertex( HepLorentzVector( m_xVtx , m_yVtx , m_zVtx , 
                                            CLHEP::Tcomponent( 0. ) ) );
  
  evt->add_vertex( v1 );

  int i;
  StatusCode sc;
  double px, py, pz;
  for ( i = 1; i <= m_partNumber; ++i ) {

    px = 0;
    py = 0;
    pz = 0;

    if( m_useGrid ){
      if( m_etaPhi ) {
        sc = generateGridEtaPhi(px,py,pz);
        if( sc.isFailure() ) return Error("Error in generating eta-phi grid");
      }
      else{
        sc = generateGridXY(px,py,pz);
        if( sc.isFailure() ) return Error("Error in generting x-y grid");
      }
    }
    else{     
      if( m_etaPhi ){
        generateUniformEtaPhi(px,py,pz);
      }
      else{
        generateUniformXY(px,py,pz);
      }
    }

    HepLorentzVector fourmomentum;
    fourmomentum.setVectM( Hep3Vector(px,py,pz) , m_mass );
    
    v1->add_particle_out( new HepMC::GenParticle(fourmomentum, m_pdgCode,
                          LHCb::HepMCEvent::StableInProdGen) );    
  }

  evt->set_signal_process_id( m_partNumber );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Generation of a uniformly flat distribution in x-y plane
//=============================================================================
void MaterialEvalGun::generateUniformXY( double& px, double& py, double& pz){
 
  double x = m_flatGenerator() * ( m_xmax - m_xmin ) + m_xmin - m_xVtx;
  double y = m_flatGenerator() * ( m_ymax - m_ymin ) + m_ymin - m_yVtx;

  double z = m_zplane - m_zVtx;   
  double r = sqrt(x*x+y*y+z*z);

  px = m_ptotal*x/r;    
  py = m_ptotal*y/r;    
  pz = m_ptotal*z/r;

  if( msgLevel( MSG::VERBOSE ) ) {
    verbose() << "====> Generate Uniform XY  "<< endmsg;
    verbose() << " X = " << x << " mm, Y = " << y << " mm " << endmsg;
  }
  
  return;
  
}

//=============================================================================
// Generate 3-momentum for a regular grid in x-y plane
//=============================================================================
StatusCode MaterialEvalGun::generateGridXY( double& px, double& py, 
                                            double& pz) {
  
  double x, y, z, r;

  if( m_counterY < m_nYstep ){
    if( m_counterX < m_nXstep ){
      
      x = (m_xmin + (m_dXstep/2)) + (m_counterX*m_dXstep) - m_xVtx;
      y = (m_ymin + (m_dYstep/2)) + (m_counterY*m_dYstep) - m_yVtx;
      z = m_zplane - m_zVtx;
      r = sqrt(x*x+y*y+z*z);
      
      px = m_ptotal*x/r;    
      py = m_ptotal*y/r;
      pz = m_ptotal*z/r;  
      
      m_counterX++;
    } 
    if( m_counterX == m_nXstep ){
      m_counterX = 0; 
      m_counterY++;
    }
  }
  else if( m_counterY >= m_nYstep){
    return StatusCode::FAILURE;
  }
  
  if( msgLevel( MSG::VERBOSE ) ) {
    verbose() << "====> Generate Grig XY  "<< endmsg;
    verbose() << " X = " << x << " mm,  Y = " << y << " mm" << endmsg;
  }
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
// Generate 3-momentum for a uniformly flat distribution in eta-phi plane
//=============================================================================
void MaterialEvalGun::generateUniformEtaPhi(double& px, double& py, 
                                            double& pz) {
  
  double eta, phi, theta;
  
  eta = m_flatGenerator() * ( m_maxEta - m_minEta ) + m_minEta;
  phi = m_flatGenerator() * ( m_maxPhi - m_minPhi ) + m_minPhi;
  theta = 2.*atan(exp(-eta));

  px = m_ptotal*sin(theta)*cos(phi);
  py = m_ptotal*sin(theta)*sin(phi);
  pz = m_ptotal*cos(theta);

  if( msgLevel( MSG::VERBOSE ) ) {
    verbose() << "====> GenerateUniform EtaPhi " << endmsg;
    verbose() << " Eta = " << eta << " Phi = " << phi << " rad " << endmsg;
  }
  
  return;

}


//=============================================================================
// Generate 3-momentum for a regular grid in eta-phi plane
//=============================================================================
StatusCode MaterialEvalGun::generateGridEtaPhi(double& px, double& py, 
                                               double& pz) {

  double eta, phi, theta;
  phi = 0.0;
  if( m_counterEta < m_nEtastep ){
    if( m_counterPhi < m_nPhistep ){
      
      phi = (m_minPhi+ (m_dPhistep/ 2)) + (m_counterPhi*m_dPhistep);
      eta = (m_minEta+ (m_dEtastep/ 2)) + (m_counterEta*m_dEtastep);
      theta = 2.*atan(exp(- eta));
      
      px = m_ptotal*sin(theta)*cos(phi);
      py = m_ptotal*sin(theta)*sin(phi);
      pz = m_ptotal*cos(theta);

      m_counterPhi++;
    }
    if( m_counterPhi == m_nPhistep ){
      m_counterPhi = 0; 
      m_counterEta++;
    }
  }
  else if( m_counterEta >= m_nEtastep){
    return StatusCode::FAILURE;
  }
  
  if( msgLevel( MSG::VERBOSE ) ) {
    verbose()<<"======>Generate Grid Eta Phi"<<endmsg;
    verbose() << " Eta = " << eta << " Phi = " << phi << " rad " << endmsg;
  }
  
  return StatusCode::SUCCESS;

}

//=============================================================================


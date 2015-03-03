// $Id: MaterialEvalGun.cpp,v 1.4 2007-01-12 15:22:03 ranjard Exp $
// -------------------------------------------------------------
// Description:
//   Allows the user to "shoot" Monte Carlo particles and store the result
//   in the Transient Store.
//
//   W. Pokorski, May 2002

// This class
#include "MaterialEvalGun.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// From Event
#include "Event/HepMCEvent.h"

// From HepMC
#include "HepMC/GenEvent.h"

DECLARE_ALGORITHM_FACTORY( MaterialEvalGun );

//============================================================================
// Constructor
//============================================================================
MaterialEvalGun::MaterialEvalGun(const std::string& name, 
                                 ISvcLocator* pSvcLocator): 
  ParticleGunBaseAlg(name, pSvcLocator), m_mass( 0. ) {
    // position of the "production point"
    declareProperty("Xorig" , m_xVtx = 0. * mm ) ;
    declareProperty("Yorig" , m_yVtx = 0. * mm ) ;
    declareProperty("Zorig" , m_zVtx = 0. * mm ) ;
    
    // position and size of the "target rectangle"
    declareProperty("ZPlane" , m_zplane = 1000. * mm ) ;
    declareProperty("Xmin"   , m_xmin   = -10.  * mm ) ;
    declareProperty("Ymin"   , m_ymin   = -10.  * mm ) ;
    declareProperty("Xmax"   , m_xmax   =  10.  * mm ) ;
    declareProperty("Ymax"   , m_ymax   =  10.  * mm ) ;
    declareProperty("ModP"   , m_ptotal = 500000. * MeV ) ;  
    
    //number of particles to be generated and their pdg
    declareProperty("PartNumber" , m_partNumber = 10 ) ;
    declareProperty("PdgCode"    , m_pdgCode    = 12 ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
MaterialEvalGun::~MaterialEvalGun() { }

//=============================================================================
// Initialize Method
//=============================================================================
StatusCode MaterialEvalGun::initialize() {  
  StatusCode sc = ParticleGunBaseAlg::initialize( ) ;
  if ( sc.isFailure() ) return sc ;  
  
  sc = m_flatGenerator.initialize( randSvc() , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize random number generator" ) ;

  IParticlePropertySvc * ppSvc =  
    svc< IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;
  
  ParticleProperty * particle = ppSvc -> findByStdHepID( m_pdgCode ) ; 
  m_mass = particle -> mass();

  release( ppSvc ) ;
  
  return sc ;
}

//=============================================================================
// Execute Method
//=============================================================================
StatusCode MaterialEvalGun::callParticleGun( HepMC::GenEvent * evt ) {

  HepMC::GenVertex* v1 = 
    new HepMC::GenVertex( HepLorentzVector( m_xVtx , m_yVtx , m_zVtx , 
                                            CLHEP::Tcomponent( 0. ) ) ) ;
  
  evt -> add_vertex( v1 );

  int i ;
  for ( i = 1 ; i<=m_partNumber ; ++i ) {
    double x = m_flatGenerator() * ( m_xmax - m_xmin ) + m_xmin - m_xVtx ;
    double y = m_flatGenerator() * ( m_ymax - m_ymin ) + m_ymin - m_yVtx ;
    double z = m_zplane - m_zVtx ;   

    double r = sqrt(x*x+y*y+z*z) ;

    double px = m_ptotal*x/r;    
    double py = m_ptotal*y/r;    
    double pz = m_ptotal*z/r;

    HepLorentzVector fourmomentum;
    fourmomentum.setVectM( Hep3Vector(px,py,pz) , m_mass ) ;    

    v1 -> add_particle_out( new HepMC::GenParticle(fourmomentum,m_pdgCode,
                  LHCb::HepMCEvent::StableInProdGen) ) ;    
  }

  evt -> set_signal_process_id( m_partNumber ) ;
  
  return StatusCode::SUCCESS;
}







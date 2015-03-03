// $Id: SingleParticleGun.cpp,v 1.3 2007-01-12 15:22:04 ranjard Exp $
// -------------------------------------------------------------
// File:  ParticleGuns/SingleParticleGun.cpp
// Description:
//   Allows the user to "shoot" Monte Carlo particles and store the result
//   in the Transient Store.
//
// AuthorList:
//         M. Shapiro:  Initial Code March 2000
//         W. Pokorski: LHCb modifications: all the time...

// This class
#include "SingleParticleGun.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// From HepMC
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// From Event
#include "Event/HepMCEvent.h"

DECLARE_ALGORITHM_FACTORY( SingleParticleGun );

//==========================================================================
// Constructor
//==========================================================================
SingleParticleGun::SingleParticleGun(const std::string& name, 
                                     ISvcLocator* pSvcLocator): 
  ParticleGunBaseAlg(name,pSvcLocator), m_mass( 0. ) {
    declareProperty("Pt",m_requestedPt = 5.0 * GeV ) ; 
    declareProperty("Eta",m_requestedEta = 0.0 ) ; 
    declareProperty("Phi",m_requestedPhi = 0.0 * rad ) ; 
    
    declareProperty("MinPt",m_minPt = 1. * GeV ) ; 
    declareProperty("MinEta",m_minEta = -4.0 ) ; 
    declareProperty("MinPhi",m_minPhi = 0. * rad ) ; 
    
    declareProperty("MaxPt",m_maxPt = 100. * GeV ) ; 
    declareProperty("MaxEta",m_maxEta= 4.0 ) ; 
    declareProperty("MaxPhi",m_maxPhi = twopi * rad ) ; 
    
    declareProperty("SigmaPt",m_sigmaPt = 0.1 * GeV ) ; 
    declareProperty("SigmaEta",m_sigmaEta= 0.1 ) ; 
    declareProperty("SigmaPhi",m_sigmaPhi = 0.1 * rad ) ; 
    
    declareProperty( "ModePt" , m_PtGenMode = SPGGenMode::FixedMode ) ; 
    declareProperty( "ModeEta" , m_EtaGenMode = SPGGenMode::GaussMode ) ; 
    declareProperty( "ModePhi" , m_PhiGenMode = SPGGenMode::FlatMode  ) ; 
    
    declareProperty("PdgCode",m_pdgCode=211);
  }

//==========================================================================
// Destructor
//==========================================================================
SingleParticleGun::~SingleParticleGun() { }

//==========================================================================
// Initialize Generator
//==========================================================================
StatusCode SingleParticleGun::initialize( ) {
  StatusCode sc = ParticleGunBaseAlg::initialize() ;
  if ( ! sc.isSuccess() ) return sc;
  
  // Create the flat and gaussian generators
  m_flatGenerator.initialize( randSvc() , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize flat generator" ) ;
  
  m_gaussGenerator.initialize( randSvc() , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize Gaussian generator" ) ;
  
  // Get the mass of the particle to be generated
  IParticlePropertySvc * ppSvc = 
    svc< IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;
 
  ParticleProperty* particle = ppSvc->findByStdHepID(m_pdgCode); 
  m_mass = particle->mass();
 
  // 
  // Make sure the parameters are in a sensible range...
  //
  if ( SPGGenMode::FixedMode != m_PtGenMode && 
       ( m_minPt > m_requestedPt || m_maxPt < m_requestedPt ) ||
       m_maxPt < m_minPt ) {
    error() << " Pt min and max out of range.  \n" 
            << "     Will set Pt mode to Fixed!!!" << endmsg ;
    m_PtGenMode = SPGGenMode::FixedMode;
  }

  if( SPGGenMode::FixedMode != m_EtaGenMode && 
      (m_minEta > m_requestedEta || m_maxPt < m_requestedEta) 
      || m_maxEta < m_minEta ) {
    error() << " Eta min and max out of range. \n" 
            << " Will set Eta mode to Fixed!!!" << endmsg ;
    m_EtaGenMode = SPGGenMode::FixedMode;
  }
  if( SPGGenMode::FixedMode != m_PhiGenMode && 
      (m_minPhi > m_requestedPhi || m_maxPhi < m_requestedPhi) 
      || m_maxPhi < m_minPhi ) {
    error() << " Phi min and max out of range.  \n" 
            << " Will set Phi mode to Fixed!!!" << endmsg ;
    m_PhiGenMode = SPGGenMode::FixedMode;
  }

  release( ppSvc ) ;
  
  return sc ;
}

//===========================================================================
// Generate Particle
//===========================================================================
StatusCode SingleParticleGun::callParticleGun( HepMC::GenEvent * evt ) {
  // Generate values for pt, eta and phi
  //
  double pt = generateValue(m_PtGenMode,m_requestedPt, m_sigmaPt, 
                            m_minPt, m_maxPt);
  double eta = generateValue(m_EtaGenMode,m_requestedEta, m_sigmaEta, 
                             m_minEta, m_maxEta);
  double phi = generateValue(m_PhiGenMode,m_requestedPhi, m_sigmaPhi, 
                             m_minPhi, m_maxPhi);
  
  // Transform to x,y,z coordinates
  //
  double theta = 2.*atan(exp(-eta));
  double px = pt*cos(phi);
  double py = pt*sin(phi);
  double pz = pt/tan(theta);  
  
  HepLorentzVector fourMom ;
  fourMom.setVectM( Hep3Vector( px , py , pz ) , m_mass ) ;

  HepMC::GenVertex * v1 = new HepMC::GenVertex() ;
  
  evt->add_vertex( v1 );
  v1->add_particle_out
    ( new HepMC::GenParticle( fourMom, m_pdgCode , 
                              LHCb::HepMCEvent::StableInProdGen ) ) ;

  evt -> set_signal_process_id( 1 ) ;
  evt -> set_signal_process_vertex( v1 ) ;

  return StatusCode::SUCCESS ;
}

//============================================================================
// Generate value
//============================================================================
double SingleParticleGun::generateValue( const int mode, const double val, 
                                         const double sigma, const double min, 
                                         const double max) {
  double tmp ;
  int i = 0 ;
  const int maxtries = 100 ; 
  
  switch (mode) {
  case SPGGenMode::FixedMode :
    return val ;
  case SPGGenMode::GaussMode :
    tmp = max + 1.0 ;
    i = 0 ;
    do {
      tmp = m_gaussGenerator() * sigma + val ; 
      i++;
    } while ( (tmp<min) || (tmp > max) && (i < maxtries));
    if(i>maxtries) {
      error() << "Cant generate value in range (min, max) "
              << val << "\t" << min << "\t" << max << endreq;
    }
    return tmp;
  case SPGGenMode::FlatMode :
    tmp = m_flatGenerator() * ( max - min ) + min ;
    return tmp;
  default:
    error() << "Unknown Generation Mode" << endreq;
    return 0.;
  }
}

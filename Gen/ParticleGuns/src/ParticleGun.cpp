// $Id: ParticleGun.cpp,v 1.5 2007-08-24 17:27:16 gcorti Exp $

// This class
#include "ParticleGun.h"

// From STL
#include <cmath>

// FromGaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// From HepMC
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// From Event
#include "Event/HepMCEvent.h"

DECLARE_ALGORITHM_FACTORY( ParticleGun );

//===========================================================================
// Constructor
//===========================================================================
ParticleGun::ParticleGun ( const std::string& name, ISvcLocator* pSvcLocator )
  : ParticleGunBaseAlg ( name, pSvcLocator ) {  
    declareProperty( "MomentumMin" , m_minMom = 100.0 * GeV ) ;
    declareProperty( "ThetMin"     , m_minTheta = 0.1 * rad ) ;
    declareProperty( "PhiMin"      , m_minPhi = 0. * rad ) ;
    
    declareProperty( "MomentumMax" , m_maxMom   = 100.0 * GeV ) ;
    declareProperty( "ThetMax"     , m_maxTheta = 0.4 * rad ) ;
    declareProperty( "PhiMax"      , m_maxPhi   = twopi * rad ) ;

    m_pdgCodes.push_back( -211 ); // default pi-
    declareProperty("PdgCodes",m_pdgCodes);

    declareProperty("xVertexMin", m_minxvtx = 0.0 * mm ) ;
    declareProperty("yVertexMin", m_minyvtx = 0.0 * mm ) ;
    declareProperty("zVertexMin", m_minzvtx = 0.0 * mm ) ;
    declareProperty("xVertexMax", m_maxxvtx = 0.0 * mm ) ;
    declareProperty("yVertexMax", m_maxyvtx = 0.0 * mm ) ;
    declareProperty("zVertexMax", m_maxzvtx = 0.0 * mm ) ;

    declareProperty("px", m_px = 1.0 * GeV ) ;
    declareProperty("py", m_py = 1.0 * GeV ) ;
    declareProperty("pz", m_pz = 1.0 * GeV ) ;

    declareProperty("GunMode", m_gmode = 1 ) ;

    declareProperty("MinNumParticles", m_minParts = 1 ) ;
    declareProperty("MaxNumParticles", m_maxParts = 1 ) ;  
}

//===========================================================================
// Destructor
//===========================================================================
ParticleGun::~ParticleGun() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode ParticleGun::initialize() {
  StatusCode sc = ParticleGunBaseAlg::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;

  sc = m_flatGenerator.initialize( randSvc() , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize flat generator" ) ;
  
  // Get the mass of the particle to be generated
  //
  IParticlePropertySvc* ppSvc = 
    svc< IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  // check momentum and angles
  if ( ( 0 != m_gmode ) && 
       ( ( m_minMom   > m_maxMom   ) || 
         ( m_minTheta > m_maxTheta ) || 
         ( m_minPhi   > m_maxPhi   ) ) )
    return Error( "Incorrect values for momentum, theta or phi!" ) ;
  
  // sanity checks on particle numbers
  if ( m_minParts > m_maxParts ) 
    return Error( "Max number of particles < Min number of particles !" ) ;
  else if ( 0 == m_maxParts ) 
    return Error( "Number of particles to generate set to zero !" ) ;

  // This is odd, so issue a warning
  if ( ( 0 == m_gmode ) && ( m_maxParts > 1 ) ) 
    warning() << "Generating multiple particles with the identical momenta !" 
              << endmsg ;

  info() << "Number of particles per event chosen randomly between "
         << m_minParts << " and " << m_maxParts << endmsg ;

  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode ;
  for ( icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode ) {
    ParticleProperty * particle = ppSvc->findByStdHepID( *icode );
    m_masses.push_back( ( particle->mass() ) ) ;
    m_names.push_back( particle->particle() ) ;
    info() << " " << particle->particle() ;
  }
  
  info() << endmsg ;

  // printout vertex information
  info() << "Origin vertex at : ( " 
         << m_minxvtx / mm << " mm < x < " << m_maxxvtx / mm << " mm"
         << ", " << m_minyvtx / mm << " mm < y < " << m_maxyvtx / mm << " mm"
         << ", " << m_minzvtx / mm << " mm < z < " << m_maxzvtx / mm << " mm"
         << " )" << endmsg ;

  if ( m_gmode ) {
    info() << "Momentum range: " << m_minMom / GeV << " GeV <-> " 
           << m_maxMom / GeV << " GeV" << endmsg ;
    info() << "Theta range: " << m_minTheta / rad << " rad <-> " 
           << m_maxTheta / rad << " rad" << endmsg ;
    info() << "Phi range: " << m_minPhi / rad << " rad <-> " 
           << m_maxPhi / rad << " rad" << endmsg ;
  } else {
    info() << "Momentum: (" << m_px / GeV << " GeV, " 
           << m_py / GeV << " GeV, " << m_pz / GeV << " GeV)" 
           << endmsg ;
  }

  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
StatusCode ParticleGun::callParticleGun( HepMC::GenEvent * evt ) {  
  // randomly choose number of particles
  unsigned currentparts = ( m_minParts + 
                            (unsigned int)( m_flatGenerator() 
                                            * (1+m_maxParts-m_minParts) ) ) ;

  if ( currentparts > m_maxParts ) currentparts = m_maxParts;

  unsigned int iPart ;
  for ( iPart = 0; iPart < currentparts; ++iPart ) {

    double px(0.), py(0.), pz(0.) ;
    
    if ( m_gmode ) {
      
      // Generate values for energy, theta and phi
      const double momentum = m_minMom   + m_flatGenerator() * 
        (m_maxMom-m_minMom);
      const double theta    = m_minTheta + 
        m_flatGenerator() * ( m_maxTheta - m_minTheta ) ;
      const double phi      = m_minPhi   + m_flatGenerator() * 
        (m_maxPhi-m_minPhi);

      // Transform to x,y,z coordinates
      const double pt = momentum*sin(theta);
      px              = pt*cos(phi);
      py              = pt*sin(phi);
      pz              = momentum*cos(theta);
    } else {
      px = m_px ;
      py = m_py ;
      pz = m_pz ;
    }

    // randomly choose a particle type
    unsigned int currentType = 
      (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
    // protect against funnies
    if ( currentType >= m_pdgCodes.size() ) currentType = 0; 

    HepLorentzVector fourMom;
    fourMom.setVectM( Hep3Vector(px,py,pz), m_masses[currentType] );

    // new vertex at the position specified by jobOptions
    const double x = m_minxvtx + m_flatGenerator()*(m_maxxvtx - m_minxvtx);
    const double y = m_minyvtx + m_flatGenerator()*(m_maxyvtx - m_minyvtx);
    const double z = m_minzvtx + m_flatGenerator()*(m_maxzvtx - m_minzvtx);

    // new vertex
    const HepLorentzVector vtx(x,y,z,CLHEP::Tcomponent(0.));
    HepMC::GenVertex * v1 = new HepMC::GenVertex( vtx );
    evt->add_vertex( v1 );
    v1 -> 
      add_particle_out( new HepMC::GenParticle
                        ( fourMom , m_pdgCodes[ currentType ] ,
                          LHCb::HepMCEvent::StableInProdGen ) ) ;
    debug() << " -> " << m_names[ currentType ] << endmsg 
            << "   P   = " << fourMom << endmsg 
            << "   Vtx = " << vtx << endmsg ;
  } // end loop over particles

  evt -> set_signal_process_id( currentparts ) ;

  return StatusCode::SUCCESS ;
}


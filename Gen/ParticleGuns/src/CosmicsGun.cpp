// $Id: CosmicsGun.cpp,v 1.1 2008-01-24 20:48:16 manca Exp $

// This class
#include "CosmicsGun.h"

// From STL
#include <cmath>
#include <TF2.h>

// FromGaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// From HepMC
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// From Event
#include "Event/HepMCEvent.h"

DECLARE_ALGORITHM_FACTORY( CosmicsGun );

//===========================================================================
// Constructor
//===========================================================================
CosmicsGun::CosmicsGun ( const std::string& name, ISvcLocator* pSvcLocator )
  : ParticleGunBaseAlg ( name, pSvcLocator ) {  
    //momentum of the mu at detection
    declareProperty( "MomentumMin" , m_minMom =   1000.0 * MeV ) ;
    //zenith angle of the mu with the vertical
    declareProperty( "ThetaMin"     , m_minTheta = 0.1 * rad ) ;
    //azimuthal angle 
    declareProperty( "PhiMin"      , m_minPhi =   0. * rad ) ;
    
    declareProperty( "MomentumMax" , m_maxMom   = 100000.0 * MeV ) ;
    declareProperty( "ThetaMax"     , m_maxTheta = 1.5708 * rad ) ;//90
    declareProperty( "PhiMax"      , m_maxPhi   = twopi* rad ) ;//360

    m_pdgCodes.push_back( 13 ); // default mu-
    m_pdgCodes.push_back( -13 ); // add mu+
    declareProperty("PdgCodes",m_pdgCodes);

    //xyz define the region of the detector I want to 
    //uniformly populate. The vertex is then extrapolated to
    //a plane above the detector at y=+6m.
    //default is the all detector
    declareProperty("xVertexMin", m_minxvtx = -5000.0 * mm ) ;
    declareProperty("yVertexMin", m_minyvtx = -5000.0 * mm ) ;
    declareProperty("zVertexMin", m_minzvtx = 0.0 * mm ) ;
    declareProperty("xVertexMax", m_maxxvtx = 5000.0 * mm ) ;
    declareProperty("yVertexMax", m_maxyvtx = 5000.0 * mm ) ;
    declareProperty("zVertexMax", m_maxzvtx = 19000.0 * mm ) ;
    
    //possible to choose between 1=nucl-ex/0601019 and 0=hep-ph/0604145;
    //default is 0; model 1 is strongly discouraged
    declareProperty("TheoryModel", m_model = 0 ) ;
    //Time at which I want the mu to hit the detector;
    declareProperty("TimeOfInteraction", m_time = 100.0 ) ;
    declareProperty("MinNumParticles", m_minParts = 1 ) ;
    declareProperty("MaxNumParticles", m_maxParts = 1 ) ;  
}

//===========================================================================
// Destructor
//===========================================================================
CosmicsGun::~CosmicsGun() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode CosmicsGun::initialize() {
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
  if ( 
       ( ( m_minMom   > m_maxMom   ) || 
         ( m_minTheta > m_maxTheta ) || 
         ( m_minPhi   > m_maxPhi   ) ) )
    return Error( "Incorrect values for momentum, theta or phi!" ) ;
  
  // sanity checks on particle numbers
  if ( m_minParts > m_maxParts ) 
    return Error( "Max number of particles < Min number of particles !" ) ;
  else if ( 0 == m_maxParts ) 
    return Error( "Number of particles to generate set to zero !" ) ;

  // // This is odd, so issue a warning
//   if ( ( m_maxParts > 1 ) ) 
//     warning() << "Generating multiple particles with the identical momenta !" 
//               << endmsg ;

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
  info() << "Interaction region : ( " 
         << m_minxvtx / mm << " mm < x < " << m_maxxvtx / mm << " mm"
         << ", " << m_minyvtx / mm << " mm < y < " << m_maxyvtx / mm << " mm"
         << ", " << m_minzvtx / mm << " mm < z < " << m_maxzvtx / mm << " mm"
         << " )" << endmsg ;


    info() << "Momentum range: " << m_minMom / GeV << " GeV <-> " 
           << m_maxMom / GeV << " GeV" << endmsg ;
    info() << "Zenith angle range: " << m_minTheta / rad << " rad <-> " 
           << m_maxTheta / rad << " rad" << endmsg ;
    info() << "Phi range: " << m_minPhi / rad << " rad <-> " 
           << m_maxPhi / rad << " rad" << endmsg ;

  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
StatusCode CosmicsGun::callParticleGun( HepMC::GenEvent * evt ) {  
  // randomly choose number of particles
  unsigned currentparts = ( m_minParts + 
                            (unsigned int)( m_flatGenerator() 
                                            * (1+m_maxParts-m_minParts) ) ) ;

  if ( currentparts > m_maxParts ) currentparts = m_maxParts;

  unsigned int iPart ;
  for ( iPart = 0; iPart < currentparts; ++iPart ) {
    
    double px(0.), py(0.), pz(0.) ;
    double verx(0.), very(0.), verz(0.) ;

    //GM
    //get the momentum according to the cosmic spectrum from
    //two different models
    //as a function of P=x (GeV) and theta'=y (in rads)

    double t = 0;
    double p = 0;
    double flux=0;
    double fluxMax=0.0;
    if(m_model==1) fluxMax=0.002;
    else if(m_model==0) fluxMax = 0.000002;
    
    for(int i=0; i<1000000; i++) {
      
      //theta uniform in radians
      t = ( m_minTheta + m_flatGenerator()*(m_maxTheta-m_minTheta) )  ;
      //translate momentum in GeV
      p = ( m_minMom/1000.0 + m_flatGenerator()*(m_maxMom-m_minMom)/1000.0 )  ;
      //
      //This is the function I need to use for the generation of the events; 
      //* m_model==1 => flux from nucl-ex/0601019, eq (1). take out the 
      //             p^3 factor and add the sin(theta)
      //* m_model==0 => flux from D.Reyna (hep-ph 0604145), adding the sin(theta)
      //             factor; flux calculated through "sphere" of cross-section 1cm^2
      if ( m_model==1 ) {
        warning()<<" You chose to use model 1; the use of this model is discouraged. You do it at your own risk "<< endmsg;        
        flux = sin(t)*(18.0*(p+5.0)*pow(p+2.7/cos(t),-2.7)) / ((p*cos(t)+145.0)*(p+5.0/cos(t))) ;
      } else if(m_model==0) {
        //depth in Meters Water Equivalent, 
        //80 meters * density of the standard rock w.r.t density of water = 1 = 2.65 g/cm^3
        double depth = 80.0*2.65;
        double angle = 0.0;   //flat overburden     
        flux = muonSpec(p, t, depth , angle);        
      }
      
      if(flux>fluxMax ) warning() <<"Cosmic flux =" <<flux<<" > Max = "<<fluxMax<<endmsg;
      
      double temp = m_flatGenerator()*fluxMax ;
      //std::cout << " temp ="<<temp<<std::endl;
      if(temp < flux)    break;        
    }
    
    //translate back into MeV 
    p*=1000;
    //Momentum does not need to be changed.
    const double momentum = p;
    //Phi random between +pi and -pi;
    double phiprime      = m_minPhi   + m_flatGenerator() * 
      (m_maxPhi-m_minPhi);     
    //
    //the angle (t) is the zenith, angle of the particle with the vertical (theta');
    //I want to translate it into theta = angle with the beam in the LHCb system.
    //
    //Now have p, theta' and phi';
    //need ^x,^y,^z=versors in cartesian system;
    //
    verx=sin(t)*cos(phiprime);
    very=-1*cos(t);
    verz=sin(t)*sin(phiprime);
    /*
      expressing ^x,^y,^z as a function of the 
      LHCb coordinates phi, theta (assuming 
      theta positive from -y axis => vertical track->theta = zero,y=-1,
      and phi positive from x axis)
      verx=sin(theta)*cos(phi)
      very=sin(theta)*sin(phi)
      verz=cos(theta),
      from this I get theta and phi as a function of the versors
    */
    const double theta = acos(verz);
    const double phi   = atan2(very,verx);
    //
    debug() <<" CosmicsGun : "<<endmsg;
    debug() <<" p = "<<momentum/GeV<<" GeV; "<< " Phi = "<< phi<< endmsg;
    debug() <<" Theta = "<<theta*180.0/3.14159265<<" degrees, "<<theta
            <<" rad"<<" ( from t = "<<t*180.0/3.14159265<<" "<<t <<")"<< endmsg;
    // Transform to x,y,z coordinates of the momentum
    const double pt = momentum*sin(theta);
    px              = pt*cos(phi);
    py              = pt*sin(phi);
    pz              = momentum*cos(theta);    
    // randomly choose a particle type
    unsigned int currentType = 
      (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
    // protect against funnies
    if ( currentType >= m_pdgCodes.size() ) currentType = 0; 

    HepLorentzVector fourMom;
    fourMom.setVectM( Hep3Vector(px,py,pz), m_masses[currentType] );

    //
    //Generate random space point anywhere inside the detector 
    //(as specified by job options )
    //0<z<19m, -5<x<5m, -5<y<5m 
    //
    double xprime = m_minxvtx + m_flatGenerator()*(m_maxxvtx - m_minxvtx);
    double yprime = m_minyvtx + m_flatGenerator()*(m_maxyvtx - m_minyvtx);
    double zprime = m_minzvtx + m_flatGenerator()*(m_maxzvtx - m_minzvtx);
    //GM: Add the time component; this is the time I want the cosmics
    //to be at the point in the detector.
    double tprime = m_time;    
    //
    //now extrapolate at y=6meters (above the detector), assuming 
    //cosmics always going down
    //
    const double y = m_maxyvtx;
    const double x = xprime + (m_maxyvtx-yprime)*(verx/very);
    const double z = zprime + (m_maxyvtx-yprime)*(verz/very);
    double tofLight = sqrt( (xprime-x)*(xprime-x) + 
                            (yprime-y)*(yprime-y) + (zprime-z)*(zprime-z) )/300.0;
    //get the time at the origin
    const double time = tprime - tofLight;
    
    //
    // new vertex
    //
    const HepLorentzVector vtx(x,y,z,CLHEP::Tcomponent(time));
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

double CosmicsGun::muonSpec(double muMomentum, double Theta, double vDepth, 
		double overAngle)
{

/*************************************************************************
 This is a function which is based on the surface muon spectrum
 that was developed in hep-ph/0604145.  It will return the rate of muon
 events at a given depth as a function of muon momentum and the 
 zenith angle.  Energy loss is calculated based on the geometric 
 path length to the surface and an energy loss of 217 MeV / mwe
 which is consistent with "standard rock".  The input and output 
 are defined as follows:
  Inputs:
      muMomentum  --  momentum of the detected muon in GeV
      cosTheta    --  cos of the zenith angle (no units)
      vDepth      --  The vertical depth directly over the detector in
                      meters of water equivalent (mwe)
      overAngle   --  Angle from flat (in degrees) of the overburden.
                      this yeilds a generic cone in which the peak of the 
                      overburden is assumed to be directly over the 
                      detector with height vDepth and base at detector 
                      level
                             0 = a flat overburden
  Output:
      muonSpec    -- muon rate in Hz/cm2/sr/GeV

 D. Reyna  14 November 2006
**************************************************************************/
  double zeta, Eloss, Alpha, ctAlpha;
  double delta = 3.38;
  double result = 0;
  double cosTheta = cos(Theta);

  if (muMomentum < 0) {
    error() << "Error: unphysical muon momentum: "<<muMomentum<<endmsg;
    return(0);
  }

  /* Calculate energy loss for generic overburden */
  Eloss = vDepth;                                //hemisphere
  if ((overAngle >= 0) && (overAngle < 90)) {    //generic cone (includes flat)
    Alpha = (90 - overAngle)*3.14159265/180.;
    ctAlpha = cos(Alpha)/sin(Alpha);
    Eloss /= cosTheta + ctAlpha*sqrt(1-pow(cosTheta,2));
  }
  /* assume 217 MeV loss per meter of water equivalent which is roughly true
     for standard rock */
  Eloss *= 0.217;


  /* calculate rate based on the starting momentum at the surface */
  zeta = cosTheta*(muMomentum + Eloss) + delta;

  /* adding extra factor sin(theta) from d(omega) to d(theta)*/
  /* adding extra factor 2pi from integration */
  result = twopi * sin(Theta) * pow(cosTheta,2.94)*0.14*pow(zeta,-2.7)*
              (1/(1 + 1.11*zeta/115) + 1/(1 + 1.11*zeta/850));

  return(result);
}



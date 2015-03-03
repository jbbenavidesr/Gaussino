// $Id: HerwigProduction.cpp,v 1.17 2009-01-29 12:33:47 cattanem Exp $
// Include files 

// local
#include "HerwigProduction.h"

// boost
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string/erase.hpp"

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Event
#include "Event/GenCollision.h"

// HepMC
#include "HepMC/IO_HERWIG.h"
#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "LbHerwig/Herwig.h"

//#include <cstdlib>
//#include <iostream>
//-----------------------------------------------------------------------------
// Implementation file for class : HerwigProduction
//
// 2005-10-24 : Karl Harrison
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HerwigProduction );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HerwigProduction::HerwigProduction( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  {
    declareInterface< IProductionTool >( this ) ;
    declareProperty( "Commands" , m_commandVector ) ;
    declareProperty( "BeamToolName" , m_beamToolName = "CollidingBeams" ) ;
    declareProperty( "useJimmy" , m_jimmy = false ) ;
    declareProperty( "useMcatnlo" , m_mcatnlo = false ) ;
    m_nevhep = 0;
    // Set the default settings for Herwig
    //       Here, pbeam1 and pbeam2 are set via beam tool
    m_defaultSettings.clear();
    m_defaultSettings.push_back( "autpdf 1 HWLHAPDF" );
    m_defaultSettings.push_back( "autpdf 2 HWLHAPDF" );
    m_defaultSettings.push_back( "effmin 0.00001" );
    m_defaultSettings.push_back( "iprint 0" );
    m_defaultSettings.push_back( "iproc 0" );
    m_defaultSettings.push_back( "lrdec 0" );
    m_defaultSettings.push_back( "lrsud 0" );
    m_defaultSettings.push_back( "lwdec 0" );
    m_defaultSettings.push_back( "lwsud 0" );
    m_defaultSettings.push_back( "maxev 10000000" );
    m_defaultSettings.push_back( "maxer 10000000" );
    m_defaultSettings.push_back( "maxpr 0" );
    m_defaultSettings.push_back( "modpdf 1 10042" );
    m_defaultSettings.push_back( "modpdf 2 10042" );
    m_defaultSettings.push_back( "part1 P" );
    m_defaultSettings.push_back( "part2 P" );
    // Set the default settings for MC@NLO
    m_defaultSettings.push_back( "epref mcatnlo" );
    m_defaultSettings.push_back( "fren 1." );
    m_defaultSettings.push_back( "ffact 1." );
    m_defaultSettings.push_back( "frenmc 1." );
    m_defaultSettings.push_back( "ffactmc 1." );
    m_defaultSettings.push_back( "it1 0" );
    m_defaultSettings.push_back( "it2 0" );
    m_defaultSettings.push_back( "iwgtnorm 1" );
    m_defaultSettings.push_back( "lambda5 -1." );
    m_defaultSettings.push_back( "nlopdf LHAPDF 10042" );
    m_defaultSettings.push_back( "scheme MS" );
    m_defaultSettings.push_back( "zi 0.3" );
    // Quark masses (GeV) for MC@NLO
    // Non-positive value: take mass from ParticlePropertySvc
    m_defaultSettings.push_back( "m_d 0.32" );
    m_defaultSettings.push_back( "m_u 0.32" );
    m_defaultSettings.push_back( "m_s 0.50" );
    m_defaultSettings.push_back( "m_c 0." );
    m_defaultSettings.push_back( "m_b 0." );
    m_defaultSettings.push_back( "m_t 0." );
    // Gluon masses (GeV) for MC@NLO
    // (independent of masses given by ParticlePropertySvc)
    m_defaultSettings.push_back( "m_gluon 0.75" );
    // Set the default settings for Jimmy
    m_defaultSettings.push_back( "jmbug 0" );
    m_defaultSettings.push_back( "jmrad 73 0.71" );
    m_defaultSettings.push_back( "prsof 1.0" );
    m_defaultSettings.push_back( "ptjim 3.0" );

  }

//=============================================================================
// Destructor 
//=============================================================================
HerwigProduction::~HerwigProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode HerwigProduction::initialize( )
{
  debug() << "HerwigProduction::initialize called" << endmsg;

  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  always()
    << "=================================================================="
    << endmsg;
  always()
    << "Using as production engine  " << this->type()
    << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  // Declare loop indices
  unsigned int i;
  unsigned int j;

  // Set flags for using Jimmy and/or MC@NLO on basis of tool name
  boost::char_separator<char> sep(".");
  boost::tokenizer< boost::char_separator<char> > 
    strList( name() , sep ) ;
  
  std::string result = "" ;
  for ( boost::tokenizer< boost::char_separator<char> >::iterator 
          tok_iter = strList.begin();
        tok_iter != strList.end(); ++tok_iter)
    result = (*tok_iter) ;
  m_hepMCName = boost::algorithm::ierase_last_copy( result , "Production" ) ;

  if ( std::string::npos != m_hepMCName.find( "Jimmy" ) )
    m_jimmy = true;
  if ( std::string::npos != m_hepMCName.find( "MCatNLO" ) )
    m_mcatnlo = true;

  info() << "Production engine called with name " << result << endmsg;

  if ( m_jimmy ) info() << "Use of Jimmy requested" << endmsg;
  if ( m_mcatnlo ) info() << "Use of MC@NLO requested" << endmsg;

  // Retrieve beam tool
  m_beamTool = tool< IBeamTool >( m_beamToolName , this );

  // Retrieve the particle-property service
  m_ppSvc = svc< IParticlePropertySvc >( "ParticlePropertySvc" );

  // Define event type
  setGenerationEventType();

  // Map Herwig common blocks to global C structures
  Herwig::Herwig().initHerwigCommonBlocks();

  // Note: HWIGIN assumes that values have already been set for
  //       maxev, iproc, and doesn't set defaults for
  //       part1, part2, pbeam1, pbeam2
  //       
  //       To ensure values from command vectors are used,
  //       parameters are initialised both before and after call to HWIGIN
  sc = setHerwigParameters();
  if ( ! sc.isSuccess() ) 
    return sc;

  // Define Herwig process code if not already set via Command vector
  if ( 0 == gHwproc->iproc )
  {
    gHwproc->iproc = m_herwigProcess;
  }
  // Ensure process code is negative if m_mcatnlo is true, and vice versa
  if ( m_mcatnlo ) gHwproc->iproc = -abs( gHwproc->iproc );
  if ( gHwproc->iproc < 0 ) m_mcatnlo = true;
  m_herwigProcess = gHwproc->iproc;

  // Main initialisation of Herwig common blocks
  Herwig::hwigin();

  // Initially decalre all Herwig particles as unknown to decay tool
  for ( i = 0; i < 1 + m_nmxres; i++ )
    {
      gHwdktl->dktl[ i ] = false;
    } 

  // Set default values for Jimmy
  if ( m_jimmy ) Herwig::jimmin();

  // Set verbose print level for Herwig if OutputLevel set to DEBUG
  //
  // Note that print level can still be redefined by setting values
  // for Herwig paramters iprint and maxpr via Commands job option
  if ( msgLevel( MSG::DEBUG ) )
  {
    gHwpram->iprint = 3;
    gHwevnt->maxpr = gHwproc->maxev;
  }

  // Set default values for prefix of BASES file
  std::string basesPrefix;
  basesPrefix = "bases";
  if ( ( gMcnlopar->it1 <= 0 ) || ( gMcnlopar->it2 <= 0 ) )
  {
    if ( getenv( "MCATNLODATAROOT" ) != NULL )
    {
      basesPrefix = std::string( getenv( "MCATNLODATAROOT"  ) ) +
        std::string( "/data/bases" );
    }
  }
  for ( i = 0; i < 80; i++ )
    {
      gMcnlopar->pref[ i ] = ( i < basesPrefix.length() ) ?
        basesPrefix[ i ] : ' ';
    } 

  // Reprocess command vectors, as values may have been overwritten in HWIGIN
  sc = setHerwigParameters();
  if ( ! sc.isSuccess() ) 
    return sc;

  // Reset process code
  gHwproc->iproc = m_herwigProcess;

  // Release the particle-property service
  release( m_ppSvc );

  // Disable soft ME corrections where already taken care of in MC@NLO
  if ( m_mcatnlo )
  {
    gHwgupr->lhsoft = false;
    switch ( gHwproc->iproc )
    {
    case -1704:
    case -11704:
    case -1705:
    case -11705:
    case -1706:
    case -11706:
      break;
    default:
      error() << "Use of MC@NLO not enabled for Herwig process code "
        << gHwproc ->iproc << endmsg;
      error() << "Initialisation failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else
  {
    gHwgupr->lhsoft = true;
  }

  // Set fragmentation paramter for B production
  if ( abs( gHwproc->iproc ) == 1705 || abs( gHwproc->iproc ) == 11705 )
  {
    gHwpram->psplt[ 1 ] = 0.5;
  }

  char lhapdf[ 71 ];
  std::string mode;
  int index;

  // If using MC@NLO and default Herwig value set for autpdf,
  // then change to default MC@NLO value
  if ( m_mcatnlo )
  {
    std::string autpdfDefault = "LHAPDF";
    for ( i = 0; i < 2; i++ )
    {
      index = strstr( gHwprch->autpdf[ i ], "HWLHAPDF" ) - gHwprch->autpdf[ i ];
      if ( 0 == index )
      {
        for ( j = 0; j < 20; j++ )
        {
          gHwprch->autpdf[ i ][ j ] =
            ( j < autpdfDefault.length() ) ? autpdfDefault[ j ] : ' ';
        } 
      }
    }
  }

  debug() << "autpdf[ 0 ]" << gHwprch->autpdf[ 0 ] << endmsg;
  debug() << "autpdf[ 1 ]" << gHwprch->autpdf[ 1 ] << endmsg;

  mode = "FREEZE";
  index = strstr( gHwprch->autpdf[ 0 ], "LHAPDF" ) - gHwprch->autpdf[ 0 ];

  if ( 0 != index )
  {
     mode = "EXTRAPOLATE";
     index = strstr( gHwprch->autpdf[ 0 ], "LHAEXT" ) - gHwprch->autpdf[ 0 ];
  }

  if ( 0 == index )
  {
      for ( i = 0; i < 70; i++ )
      {
        lhapdf[ i ] = ( i < mode.length() ) ? mode[ i ] : ' ';
      } 
    lhapdf[ 70 ] = '\0';
    Herwig::setlhacblk( * lhapdf );
  }

  // Initialise MC@NLO
  if ( m_mcatnlo )
  {
    // Set events-file name:
    j = 0;
    for ( i = 0; i < 50; i++ )
    {
      if ( ' ' != gMcnlopar->prefev[ i ] )
      {
        gVvjin->qqin[ i ] = gMcnlopar->prefev[ i ];
        j++;
      }
      else
      {
        break;
      }
    }
    char evSuffix[ 8 ] = ".events";
    for ( i = 0; i < 7; i++ )
    {
      if ( j + i < 50 )
      {
        gVvjin->qqin[ j + i ] = evSuffix[ i ];
      }
      else
      {
        error() << "Error setting name of MC@NLO events file" << endmsg;
        error() << "Initialisation failed" << endmsg;
        return StatusCode::FAILURE;
      }
    }
    // Only ever want to generate one MC@NLO event at a time
    gMcnlopar->maxevt = 1;
    // Set MC@NLO process to be the same as Herwig process
    gMcnlopar->itmpvv = gHwproc->iproc;
    // Set heavy-quark scale for MC@NLO
    if ( ( -1704 == gHwproc->iproc ) || ( -11704 == gHwproc->iproc ) )
    {
      gMcnlopar->xm = gParmass->xmass[ 5 + 4 ];
    }
    else if ( ( -1705 == gHwproc->iproc ) || ( -11705 == gHwproc->iproc ) )
    { 
      gMcnlopar->xm = gParmass->xmass[ 5 + 5 ];
    }
    else if ( ( -1706 == gHwproc->iproc ) || ( -11706 == gHwproc->iproc ) )
    { 
      gMcnlopar->xm = gParmass->xmass[ 5 + 6 ];
    }
    // Ensure consistency of parton masses
    for ( i = 1; i <=6; i++ )
    {
      gHwprop->rmass[ i ] = gParmass->xmass[ 5 + i ];
      gHwprop->rmass[ i + 6 ] = gParmass->xmass[ 5 + i ];
    }
    gHwprop->rmass[ 13 ] = gParmass->xmass[ 5 + 21 ];
    // Perform initialisation for heavy-quark production
    Herwig::mcatnlo_qqinit();
    Herwig::mcatnlo_header();
  }

  // Compute parameter-dependent constants
  Herwig::hwuinc();
                                                                                
  // User's initial calculations
  Herwig::hwabeg();
                                                                                
  // Initialise elementary process
  Herwig::hweini();

  // Initialise Jimmy
  if ( m_jimmy )
  {
    if ( ( 0 == gJmparm->jmbug )  && ( msgLevel( MSG::DEBUG ) ) )
      gJmparm->jmbug = 20;
    Herwig::jminit();
    Herwig::jmwjmx();
    debug() << "Jimmy initialised" << endmsg;
  }

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int( 4 ) ;
  HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 ) ;
  HepMC::HEPEVT_Wrapper::set_max_number_entries( 10000 ) ; 

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Herwig
//=============================================================================
StatusCode HerwigProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                         LHCb::GenCollision * /*theCollision*/ )
{

  debug() << "HerwigProduction::generateEvent called" << endmsg;
                                                                                
  int kerror;
  kerror = -1;
  int abort;

  while ( 0 != kerror )
  {
    HepMC::HEPEVT_Wrapper::set_event_number( m_nevhep );
    if ( gHwproc->iproc < 0 )
    {
      Herwig::mcatnlo_qqgen();
      Herwig::hwupinit();
    }

    // Initialise event
    Herwig::hwuine();
                                                                                
    // Generate hard sub-process
    Herwig::hwepro();
                                                                                
    // Generate parton cascades
    Herwig::hwbgen();

    // Generate multiparton interactions
    abort = 0;
    if ( m_jimmy ) Herwig::hwmsct( abort );
                                                                                
    if ( 1 == abort )
    {
      debug() << "Event aborted by Jimmy" << endmsg;
    }
    else
    {
      // Do heavy object decays
      Herwig::hwdhob();
                                                                                
      // Do cluster formation
      Herwig::hwcfor();
                                                                                
      // Do cluster decays
      Herwig::hwcdec();
                                                                                
      // Do unstable particle decays
      Herwig::hwdhad();
                                                                                
      // Do heavy flavour hadron decays
      Herwig::hwdhvy();
                                                                                
      // Add soft underlying event
      Herwig::hwmevt();

      // Remove from event record decays of particles known to decay tool
      Herwig::hwrmdk();
                                                                                
      // Finish event
      Herwig::hwufne();
    }
    kerror = gHwevnt->ierror;
    if ( 0 != kerror )
    {
      debug() << "Herwig returns error code " << kerror << endmsg;
      gHwevnt->ierror = 0;
    }
  }

  // Perform user event analysis
  Herwig::hwanal();
  
  // Convert event to HepMC Format
  m_nevhep = HepMC::HEPEVT_Wrapper::event_number();
  HepMC::IO_HERWIG theHepIO;
  if ( ! theHepIO.fill_next_event( theEvent ) ) 
    return Error( "Could not fill HepMC event" );

  // Set null value for signal process vertex
  theEvent->set_signal_process_vertex( 0 );

  // Convert momenta from GeV to MeV
  for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
        p != theEvent->particles_end(); ++p ) 
    (*p)->set_momentum( HepMC::FourVector( (*p)->momentum().x() * Gaudi::Units::GeV , 
                                           (*p)->momentum().y() * Gaudi::Units::GeV , 
                                           (*p)->momentum().z() * Gaudi::Units::GeV , 
                                           (*p)->momentum().t() * Gaudi::Units::GeV ));

  // Convert 
  for ( HepMC::GenEvent::vertex_iterator v = theEvent -> vertices_begin();
        v != theEvent->vertices_end(); ++v )
  {
    HepMC::FourVector newPos;
    newPos.setX( (*v)->position().x() );
    newPos.setY( (*v)->position().y() );
    newPos.setZ( (*v)->position().z() );
    newPos.setT( ( (*v)->position().t() * Gaudi::Units::mm ) / 
                 Gaudi::Units::c_light );
    
    (*v) -> set_position( newPos );
  }

  // Write out event if OutputLevel set to DEBUG
  if ( msgLevel( MSG::DEBUG ) )
  {
    debug() << "Dump of HepMC::HEPEVT (momenta in GeV, ct in mm)" << endmsg;
    HepMC::HEPEVT_Wrapper::print_hepevt();

    debug() << "Dump of theEvent (momenta in MeV, ct in ns)" << endmsg;
    theEvent->print();
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Set stable the given particle in Herwig
//=============================================================================
void HerwigProduction::setStable( const ParticleProperty * thePP )
{
  debug() << "HerwigProduction::setStable called" << endmsg;

  int pdgId = thePP -> pdgID() ;

  if ( 0 != pdgId )
  {
    int ihw = -1;
    int j = 1;
    while ( ( ihw < 0 ) && ( j <= gHwprop->nres ) )
    {
      if ( pdgId == gHwprop->idpdg[ j ] )
        ihw = j;
      j++;
    }

    if ( ihw >= 0 )
    {
      char partName[ 9 ];
      int i;
      for ( i = 0; i < 8; i++ )
      {
        partName[ i ] = gHwunam->rname[ ihw ][ i ];
      } 
      partName[ 8 ] = '\0';
//    Herwig::hwusta( * partName );
      gHwdktl->dktl[ ihw ] = true;
      debug() << partName <<" (PDG id " << pdgId << ", Herwig id " << ihw
        << ") declared as known to decay tool" << endmsg;
    }
    else
    {
      debug() << "Particle with PDG id " << pdgId << " not known to Herwig"
        << endmsg;
    }

  }
}

//=============================================================================
// Retrieve the Hard scatter information
//=============================================================================
void HerwigProduction::hardProcessInfo( LHCb::GenCollision * /*theCollision*/ )
{
  warning() << "Dummy method HerwigProduction::hardProcessInfo called"
            << endmsg;
} 

//=============================================================================
// Finalize method
//=============================================================================
StatusCode HerwigProduction::finalize( )
{
  debug() << "HerwigProduction::finalize called" << endmsg;

  // Terminate elementary process
  HepMC::HEPEVT_Wrapper::set_event_number( m_nevhep );
  Herwig::hwefin();

  // Jimmy termination
  Herwig::jmefin();
                                                                                
  // Finalise user calculations
  Herwig::hwaend();

  // Delete MC@NLO events file unless in debug mode
  if ( ( gHwproc->iproc < 0 ) &&  ! ( msgLevel( MSG::DEBUG ) ) )
  {
    unsigned int i, j, k;
    int status;
    j = 0;
    char fileName[ 51 ];
    fileName[ 50 ] = '\0';
    for ( i = 0; i < 50; i++ )
    {
      if ( ' ' != gMcnlopar->prefev[ i ] )
      {
        fileName[ i ] = gMcnlopar->prefev[ i ];
        j++;
      }
      else
      {
        break;
      }
    }
    std::string evSuffix;

    for ( i = 0; i < 3; i ++ )
    {
      switch ( i )
      {
      case 0:
        evSuffix = ".events";
        break;
      case 1:
        evSuffix = "_a.events";
        break;
      case 2:
        evSuffix = "_b.events";
        break;
      }
      if ( j + evSuffix.length() >= 50 )
      {
        error() << "Name of MC@NLO events file too long" << endmsg;
      }
      else
      {
        for ( k = 0; k < evSuffix.length(); k++ )
        {
          fileName[ j + k ] = evSuffix[ k ];
        }
        fileName[ j + evSuffix.length() ] = '\0';
        status = remove( fileName );
        if ( 0 != status )
        {
          error() << "Error code " << status
            << " when trying to remove MC@NLO events file "
            << fileName << endmsg;
        }
        else
        {
          debug() << "Successfully removed MC@NLO events file "
            << fileName << endmsg;
        }
      }
    }
  }

  return GaudiTool::finalize( ) ;
}  

//=============================================================================
// Set Herwig parameters
//=============================================================================
StatusCode HerwigProduction::setHerwigParameters( )
{
  debug() << "HerwigProduction::setHerwigParameters called" << endmsg;

  // Use beam tool to retrieve beam-particle momenta
  // and set values (GeV/c) for Herwig and MCNLO
  Gaudi::XYZVector pBeam1, pBeam2;
  m_beamTool -> getMeanBeams( pBeam1 , pBeam2 );
  gHwproc->pbeam1 = sqrt( pBeam1.mag2() )/Gaudi::Units::GeV;
  gHwproc->pbeam2 = sqrt( pBeam2.mag2() )/Gaudi::Units::GeV;
  
  gMcnlopar->ecm = gHwproc->pbeam1 + gHwproc->pbeam2 ;

  debug() << "Momentum for beam particle 1: " << gHwproc->pbeam1 << " GeV"
          << endmsg;
  debug() << "Momentum for beam particle 2: " << gHwproc->pbeam2 << " GeV"
          << endmsg;
  debug() << "Centre of mass energy: " << gMcnlopar->ecm << " GeV" << endmsg;

  // Process command vector with default settings
  StatusCode sc;
  sc = parseHerwigCommands( m_defaultSettings );
  if ( ! sc.isSuccess() ) 
    return Error( "Error in Herwig default settings" , sc );

  // Process command vector with user settings
  sc = parseHerwigCommands( m_commandVector );
  if ( ! sc.isSuccess() ) 
    return Error( "Error in Herwig user settings" , sc );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Parse Herwig commands stored in a vector
//=============================================================================
StatusCode HerwigProduction::parseHerwigCommands( const CommandVector & 
                                                  theCommandVector )
{
  debug() << "HerwigProduction::parseHerwigCommands called" << endmsg;

  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin();
        theCommandVector.end() != iter; 
        ++iter )
  {
    debug() << " Command is: " << (*iter) << endmsg ;

    StringParse mystring( *iter ) ;
    std::string item = mystring.piece(1);
    std::string str1 = mystring.piece(2);
    std::string str2 = mystring.piece(3);
    int int1 = mystring.intpiece(2);
    int int2 = mystring.intpiece(3);
    int int3 = mystring.intpiece(4);
    int int4 = mystring.intpiece(5);
    double fl1 = mystring.numpiece(2);
    double fl2 = mystring.numpiece(3);
    double fl3 = mystring.numpiece(4);
    
    debug() << "  item:  " << item << endmsg;
    debug() << "  value [as string]: " << str1 << " " << str2 << endmsg; 
    debug() << "  value [as ints]:  " << int1 << " " << int2
            << " " << int3 << " " << int4 <<endmsg;
    debug() << "  value [as floats]:  " << fl1 << " " << fl2 << " "
            << fl3 << endmsg;

    unsigned int i;

    // =====================================================================
    // Variables in HWBEAM common block of Herwig
    // =====================================================================

    // Name of beam particle 1
    if ( "part1" == item )
    {
      for ( i = 0; i < 8; i++ )
      {
        gHwbmch->part1[ i ] = ( i < str1.length() ) ? str1[ i ] : ' ';
        // Also set particle name for MC@NLO
        if ( i < 4 ) gMcnlopar->part1[ i ] = gHwbmch->part1[ i ];
      } 
    }

    // Name of beam particle 2
    else if ( "part2" == item )
    {
      for ( i = 0; i < 8; i++ )
      {
        gHwbmch->part2[ i ] = ( i < str1.length() ) ? str1[ i ] : ' ';
        // Also set particle name for MC@NLO
        if ( i < 4 ) gMcnlopar->part2[ i ] = gHwbmch->part2[ i ];
      } 
    }

    // =====================================================================
    // Variables in HWDSPN common block of Herwig
    // =====================================================================

    // Unit for reading three/four body decays
    else if ( "lrdec" == item ) gHwdspn->lrdec = int1;

    // Unit for writing three/four body decays
    else if ( "lwdec" == item ) gHwdspn->lwdec = int1;

    // =====================================================================
    // Variables in HWPROC common block of Herwig
    // =====================================================================

    // Maximum number of events
    else if ( "maxev" == item ) gHwproc->maxev = int1;

    // Process identifier
    else if ( "iproc" == item ) gHwproc->iproc = int1;

    // Momentum of beam-particle 1
    else if ( "pbeam1" == item )
    {
      warning() << "Beam momentum is set via beam tool" << endmsg;
      warning() << "Value passed to pbeam1 is disregarded" << endmsg;
    }

    // Momentum of beam-particle 2
    else if ( "pbeam2" == item )
    {    
      warning() << "Beam momentum is set via beam tool" << endmsg;
      warning() << "Value passed to pbeam2 is disregarded" << endmsg;
    }

    // =====================================================================
    // Variables in HWEVNT common block of Herwig
    // =====================================================================

    // Maximum number of errors to tolerate
    else if ( "maxer" == item ) gHwevnt->maxer = int1;

    //Maximum number of events to print
    else if ( "maxpr" == item ) gHwevnt->maxpr = int1;

    // =====================================================================
    // Variables in HWPRAM common block of Herwig
    // =====================================================================

    // Minimum acceptable generation efficiency
    else if ( "effmin" == item ) gHwpram->effmin = fl1;

    // Print level
    else if ( "iprint" == item ) gHwpram->iprint = int1;

    // Unit for reading Sudakov table
    else if ( "lrsud" == item ) gHwpram->lrsud = int1;

    // Unit for writing Sudakov table
    else if ( "lwsud" == item ) gHwpram->lwsud = int1;

    // PDF mode
    else if ( "modpdf" == item ) gHwpram->modpdf[ int1 - 1 ] = int2;

    // Probability of soft underlying event
    else if ( "prsof" == item ) gHwpram->prsof = fl1;

    // =====================================================================
    // Variables in HWPRCH common block of Herwig
    // =====================================================================

    // PDF author group
    else if ( "autpdf" == item )
    {
      for ( i = 0; i < 20; i++ )
      {
        gHwprch->autpdf[ int1 - 1 ][ i ] =
          ( i < str2.length() ) ? str2[ i ] : ' ';
      } 
    }

    // =====================================================================
    // Variables in HWUSUD common block of Herwig
    // =====================================================================

    // Alpha_s order in Sudakov table
    else if ( "sudord" == item ) gHwusud->sudord = int1;

    // =====================================================================
    // Variables in JMPARM common block of Jimmy
    // =====================================================================

    // Jimmy debug flag
    else if ( "jmbug" == item ) gJmparm->jmbug = int1;

    // Inverse proton-radius squared
    else if ( "jmrad" == item ) gJmparm->jmrad[ int1 - 1 ] = fl2;

    // Minimum transverse momentum of secondary scatters
    else if ( "ptjim" == item ) gJmparm->ptjim = fl1;

    // =====================================================================
    // Variables in CETACUT common block of MC@NLO
    // =====================================================================

    else if ( "zi" == item )
    {
      gCetacut->etacut = fl1;
    }

    // =====================================================================
    // Variables in CIWGTNORM common block of MC@NLO
    // =====================================================================

    else if ( "iwgtnorm" == item )
    {
      gCiwgtnorm->iwgtnorm = int1;
    }

    // =====================================================================
    // Variables in FIXVAR common block of MC@NLO
    // =====================================================================

    else if ( "lambda5" == item )
    {
      gFixvar->xlam = fl1;
    }

    // =====================================================================
    // Variables in MCNLOPAR common block of MC@NLO
    // =====================================================================

    else if ( "bpref" == item )
    {
      for ( i = 0; i < 80; i++ )
      {
        gMcnlopar->pref[ i ] = ( i < str1.length() ) ? str1[ i ] : ' ';
      } 
    }

    else if ( "epref" == item )
    {
      for ( i = 0; i < 80; i++ )
      {
        gMcnlopar->prefev[ i ] = ( i < str1.length() ) ? str1[ i ] : ' ';
      } 
    }

    else if ( "fren" == item )
    {
      gMcnlopar->xren = fl1;
    }

    else if ( "ffact" == item )
    {
      gMcnlopar->xfh = fl1;
    }

    else if ( "frenmc" == item )
    {
      gMcnlopar->xrenmc = fl1;
    }

    else if ( "ffactmc" == item )
    {
      gMcnlopar->xfhmc = fl1;
    }

    else if ( "it1" == item )
    {
      gMcnlopar->it1 = int1;
    }

    else if ( "it2" == item )
    {
      gMcnlopar->it2 = int1;
    }

    else if ( "nlopdf" == item )
    {
      for ( i = 0; i < 20; i++ )
      {
        gMcnlopar->gname[ i ] = ( i < str1.length() ) ? str1[ i ] : ' ';
      } 
      gMcnlopar->idpdfset = int2;
    }

    else if ( "scheme" == item )
    {
      for ( i = 0; i < 2; i++ )
      {
        gMcnlopar->scheme[ i ] = ( i < str1.length() ) ? str1[ i ] : ' ';
      } 
    }

    // =====================================================================
    // Parton masses in PARMASS common block of MC@NLO
    // =====================================================================

    else if ( "m_d" == item )
    {
      gParmass->xmass[ 5 - 1 ] = getMass( 1, fl1 );
      gParmass->xmass[ 5 + 1 ] = gParmass->xmass[ 5 - 1 ];
    }

    else if ( "m_u" == item )
    {
      gParmass->xmass[ 5 - 2 ] = getMass( 2, fl1 );
      gParmass->xmass[ 5 + 2 ] = gParmass->xmass[ 5 - 2 ];
    }

    else if ( "m_s" == item )
    {
      gParmass->xmass[ 5 - 3 ] = getMass( 3, fl1 );
      gParmass->xmass[ 5 + 3 ] = gParmass->xmass[ 5 - 3 ];
    }

    else if ( "m_c" == item )
    {
      gParmass->xmass[ 5 - 4 ] = getMass( 4, fl1 );
      gParmass->xmass[ 5 + 4 ] = gParmass->xmass[ 5 - 4 ];
    }

    else if ( "m_b" == item )
    {
      gParmass->xmass[ 5 - 5 ] = getMass( 5, fl1 );
      gParmass->xmass[ 5 + 5 ] = gParmass->xmass[ 5 - 5 ];
    }

    else if ( "m_t" == item )
    {
      gParmass->xmass[ 5 + 6 ] = getMass( 6, fl1 );
    }

    else if ( "m_gluon" == item )
    {
      gParmass->xmass[ 5 + 21 ] = fl1;
    }

    // Print warning if item not recognised
    else
    {
      warning() << "Parameter '" << item << "'not known'" << endmsg;
      warning() << "If this is a valid Herwig parameter," << endmsg;
      warning() << "contact package maintainer to have it added" << endmsg;
    }

  }
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Print Herwig parameters
//=============================================================================
void HerwigProduction::printHerwigParameter( )
{  
  warning() << "Dummy method HerwigProduction::printHerwigParameter called"
            << endmsg;
}

//=============================================================================
// TRUE if the particle is a special particle and must not be modified
//=============================================================================
bool HerwigProduction::isSpecialParticle( const ParticleProperty * thePP ) const
{

  debug() << "HerwigProduction::isSpecialParticle called" << endmsg;

  switch ( abs( thePP -> pdgID() ) )
  {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 17:
  case 18:
  case 21:
  case 110:
  case 990:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 39:
  case 41:
  case 42:
  case 43:
  case 44:
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
  case 88:
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 9900110:
  case 9900210:
  case 9900220:
  case 9900330:
  case 9900440:
  case 9902110:
  case 9902210:
  case 1101:
  case 1103:
  case 2101:
  case 2103:
  case 2201:
  case 2203:
  case 3101:
  case 3103:
  case 3201:
  case 3203:
  case 3301:
  case 3303:
  case 4101:
  case 4103:
  case 4201:
  case 4203:
  case 4301:
  case 4303:
  case 4401:
  case 4403:
  case 5101:
  case 5103:
  case 5201:
  case 5203:
  case 5301:
  case 5303:
  case 5401:
  case 5403:
  case 5501:
  case 5503:
    return true ;
    break ;
  default:
    return false ;
    break ;
    return false ;  
  }
}

//=============================================================================
// Update particle properties
//=============================================================================
void HerwigProduction::updateParticleProperties
  ( const ParticleProperty * thePP )
{

  debug() << "HerwigProduction::updateParticleProperties called" << endmsg;

  int pdgId = thePP -> pdgID() ;
  double lifetime ;
  double mass ;

  if ( 0 != pdgId )
  {
    int ihw = -1;
    int j = 1;
    while ( ( ihw < 0 ) && ( j <= gHwprop->nres ) )
    {
      if ( pdgId == gHwprop->idpdg[ j ] )
        ihw = j;
      j++;
    }

    if ( ihw >= 0 )
    {
/*
      double ctau
      ctau =  thePP -> lifetime() * c_light / mm ;
      if ( ctau <= 1.e-4 * mm )
        lifetime = 0.;
      else
        lifetime = thePP -> lifetime() / second;
*/

      if ( gHwprop->rltim[ ihw ] > 1.e16 )
         lifetime = gHwprop->rltim[ ihw ];
      else
        lifetime = thePP -> lifetime() / Gaudi::Units::second;

      if ( abs( pdgId ) <= 6 )
      {
        // Use quark masses defined for MC@NLO
        mass = gParmass->xmass[ 5 + abs( pdgId ) ];  
      }
      else
      {
        mass = thePP -> mass() / Gaudi::Units::GeV;
      }
      
      verbose() << "Change particle property of ihw = " << ihw 
                << " (" << pdgId << ")" << endmsg;
      verbose() << "Mass (GeV) from " << gHwprop->rmass[ ihw ]
                << " to " << mass << endmsg;
      verbose() << "Lifetime from " << gHwprop->rltim[ ihw ]
                << " to " << lifetime << endmsg;
      
      gHwprop->rmass[ ihw ] = mass;
      gHwprop->rltim[ ihw ] = lifetime;
    }
  }
}

//=============================================================================
// Turn on fragmentation in Herwig
//=============================================================================
void HerwigProduction::turnOnFragmentation()
{
  warning() << "Dummy method HerwigProduction::turnOnFragmentation called"
            << endmsg;
}

//=============================================================================
// Turn off fragmentation in Herwig
//=============================================================================
void HerwigProduction::turnOffFragmentation()
{
  warning() << "Dummy method HerwigProduction::turnOffFragmentation called"
            << endmsg;
}

//=============================================================================
// Save parton event
//=============================================================================
void HerwigProduction::savePartonEvent( HepMC::GenEvent * /*theEvent*/ )
{
  warning() << "Dummy method HerwigProduction::savePartonEvent called"
            << endmsg;
}

//=============================================================================
// Load parton event
//=============================================================================
void HerwigProduction::retrievePartonEvent( HepMC::GenEvent * /*theEvent*/ )
{
  warning() << "Dummy method HerwigProduction::retrievePartonEvent called"
            << endmsg;
}

//=============================================================================
// Hadronize Herwig event
//=============================================================================
StatusCode HerwigProduction::hadronize( HepMC::GenEvent * /*theEvent*/, 
                                        LHCb::GenCollision * /*theCollision*/ )
{
  warning() << "Dummy method HerwigProduction::hadronize called" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Debug print out to be printed after all initializations
//=============================================================================
void HerwigProduction::printRunningConditions()
{
  debug() << "HerwigProduction::printRunningConditions called" << endmsg;

  // Write out decay table if OutputLevel set to DEBUG
  if ( msgLevel( MSG::DEBUG ) )
  {
    Herwig::hwudpr();
  }
  
  // Write out MC@NLO settings if OutputLevel set to DEBUG
  if ( gHwproc->iproc < 0 )
  {
    unsigned int i;

    debug() << "" << endmsg;
    debug() << "MC@NLO settings" << endmsg;
    debug() << "" << endmsg;

    char pref[ 81 ];
    pref[ 80  ] = '\0';
    for ( i = 0; i < 80; i++ )
    {
      if ( ' ' == gMcnlopar->pref[ i ] )
      {
        pref[ i ] = '\0';
        break;
      }
      else
      {   
        pref[ i ] = gMcnlopar->pref[ i ];
      }
    }
    debug() << "Prefix for BASES files: " << pref << endmsg;

    char prefev[ 81 ];
    prefev[ 80  ] = '\0';
    for ( i = 0; i < 80; i++ )
    {
      if ( ' ' == gMcnlopar->prefev[ i ] )
      {
        prefev[ i ] = '\0';
        break;
      }
      else
      {   
        prefev[ i ] = gMcnlopar->prefev[ i ];
      }
    }
    debug() << "Prefix for event file: " << prefev << endmsg;

    debug() << "Centre of mass energy (GeV): " << gMcnlopar->ecm << endmsg;

    debug() << "mu_ren/mu0 [NLO]: " << gMcnlopar->xren << endmsg;

    debug() << "mu_fac/mu0 [NLO]: " << gMcnlopar->xfh << endmsg;

    debug() << "mu_ren/mu0 [MC]: " << gMcnlopar->xrenmc << endmsg;

    debug() << "mu_fac/mu0 [MC]: " << gMcnlopar->xfhmc << endmsg;

    debug() << "Process id: " << gMcnlopar->itmpvv << endmsg;

    debug() << "Heavy-quark scale (GeV): " << gMcnlopar->xm << endmsg;

    debug() << "Lambda QCD (GeV): " << gFixvar->xlam << endmsg;

    debug() << "Parton masses (GeV): " << gParmass->xmass[ 5 + 1 ]
            << ", " << gParmass->xmass[ 5 + 2 ]
            << ", " << gParmass->xmass[ 5 + 3 ]
            << ", " << gParmass->xmass[ 5 + 4 ]
            << ", " << gParmass->xmass[ 5 + 5 ]
            << ", " << gParmass->xmass[ 5 + 21 ] << endmsg;

    char part1[ 5 ];
    char part2[ 5 ];
    for ( i = 0; i < 4; i++ )
    {
      part1[ i ] = gMcnlopar->part1[ i ];
      part2[ i ] = gMcnlopar->part2[ i ];
    }
    part1[ 4 ] = '\0';
    part2[ 4 ] = '\0';
    debug() << "Hadron types: " << part1 << ", " << part2 << endmsg;

    char gname[ 21 ];
    for ( i = 0; i < 20; i++ ) gname[ i ] = gMcnlopar->gname[ i ];
    gname[ 20 ] = '\0';
    debug() << "PDF group: " << gname << endmsg;

    debug() << "PDF id: " << gMcnlopar->idpdfset << endmsg;

    char scheme[ 3 ];
    for ( i = 0; i < 2; i++ ) scheme[ i ] = gMcnlopar->scheme[ i ];
    scheme[ 2 ] = '\0';
    debug() << "Renormalisation scheme: " << scheme << endmsg;

    debug() << "Number of events: " << gMcnlopar->maxevt << endmsg;

    debug() << "Weights scheme: " << gCiwgtnorm->iwgtnorm << endmsg;

    debug() << "Eta cut: " << gCetacut->etacut << endmsg;

    debug() << "Iterations for grid setup: " << gMcnlopar->it1 << endmsg;

    debug() << "Iterations for integration: " << gMcnlopar->it2 << endmsg;

    debug() << "" << endmsg;
  }
}

//=============================================================================
// Setup for forced fragmentation 
//=============================================================================
StatusCode HerwigProduction::setupForcedFragmentation( const int /*thePdgId*/ )
{
  warning() << "Dummy method HerwigProduction::setupForcedFragmentation called"
            << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get mass (GeV) for particle with PDG identifier thePdgId
//=============================================================================
double HerwigProduction::getMass( const int thePdgId, double value )
{
  debug() << "HerwigProduction::getMass called" << endmsg;

  double mass;
  if ( value > 0. )
  {
    mass = value;
  }
  else
  {
    mass = m_ppSvc->findByStdHepID( thePdgId )->mass() / Gaudi::Units::GeV;
  }

  debug() << "PDG identifier = " << thePdgId << endmsg;
  debug() << "Input value = " << value << endmsg;
  debug() << "Returned mass = " << mass << endmsg;

  return mass;
}

//=============================================================================
// Get event type defined in Generation algorithm
//=============================================================================
void HerwigProduction::setGenerationEventType()
{
  debug() << "HerwigProduction::setGenerationEventType called" << endmsg;

  m_eventType = 30000000;
  std::string theName = "Generation";
  const std::string propName = "EventType";
  std::string propValue;

  IAlgManager* theAlgMgr = svc<IAlgManager>("ApplicationMgr");
  IAlgorithm* theIAlg;

  StatusCode sc = theAlgMgr->getAlgorithm( theName, theIAlg );
  releaseSvc(theAlgMgr).ignore();
  if ( sc.isSuccess() )
  {
    Algorithm* theAlgorithm;
    try
    {
      theAlgorithm = dynamic_cast<Algorithm*>( theIAlg );
    }
    catch( ... )
    {
      sc = StatusCode::FAILURE;
    }
    if ( sc.isSuccess() )
    {
      sc = theAlgorithm->getProperty( propName, propValue );
    }
  }

  if ( sc.isSuccess() )
  {
    m_eventType = std::atoi( propValue.c_str() );
    debug() << "Event type is " << m_eventType << endmsg;
  }
  else
  {
    warning() << "Unable to determine event type from Generation algorithm"
      << endmsg;
    warning() << "Using default event type" << m_eventType << endmsg;
  }

    switch ( m_eventType / 10000000 )
    {
    case 1:
      m_herwigProcess = 1705;
      break;
    case 2:
      m_herwigProcess = 1704;
      break;
    default:
      m_herwigProcess = 8000;
    }

  return;
}

//=============================================================================
// Dummy method
//=============================================================================
StatusCode HerwigProduction::initializeGenerator() {
                                                                                
  return StatusCode::SUCCESS;
                                                                                
}
                                                                                
                                                                                
//=============================================================================

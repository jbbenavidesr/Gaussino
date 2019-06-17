// from STD & STL
#include <algorithm>
#include <future>
#include <list>
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"

// from G4
#include "Geant4/G4MaterialTable.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4ParticlePropertyTable.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4UIsession.hh"
#include "Geant4/G4VUserActionInitialization.hh"
#include "Geant4/G4VUserPhysicsList.hh"
#include "Geant4/G4VVisManager.hh"

// from GiGaMT
#include "CLHEP/Random/RandomEngine.h"
#include "GiGaMT/GiGaActionInitializer.h"
#include "GiGaMTCore/GiGaMTRunManager.h"
#include "GiGaMTCore/GiGaWorkerPayload.h"
#include "GiGaMTCore/GiGaWorkerPilot.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "SimInterfaces/IG4MonitoringTool.h"

#include "HepMC3/GenEvent.h"

// local
#include "GiGaMT.h"

//-----------------------------------------------------------------------------
// Implementation of the general components of the GiGaMT service.
// Set-up related information is given in GiGaMTISetUpSvc.cpp
//
// 20.6.2018 Dominik Muller
//
//-----------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_COMPONENT( GiGaMT )

//=============================================================================
// query interface
//=============================================================================
StatusCode GiGaMT::queryInterface( const InterfaceID& id, void** ppI )
{
  if ( 0 == ppI ) {
    return StatusCode::FAILURE; //  RETURN !!!
  } else if ( IGiGaMTSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaMTSvc*>( this );
  } else if ( IGiGaMTSetUpSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaMTSetUpSvc*>( this );
  } else {
    return Service::queryInterface( id, ppI ); //  RETURN !!!
  }

  addRef();

  return StatusCode::SUCCESS;
}

//=============================================================================
// service initialization
//=============================================================================
StatusCode GiGaMT::initialize()
{
  // initialize the base class
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) {
    return Error( "Unable to initialize the base class Service ", sc );
  }

  /// print ALL properties
  typedef std::vector<Property*> Properties;
  const Properties& properties = getProperties();
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << " List of ALL properties of " << System::typeinfoName( typeid( *this ) ) << "/" << this->name()
      << "   #properties = " << properties.size() << endmsg;
  for ( Properties::const_reverse_iterator property = properties.rbegin(); properties.rend() != property; ++property ) {
    msg << MSG::DEBUG << "Property ['Name': Value] = " << ( **property ) << endmsg;
  }

  // locate  services,
  if ( 0 == svcLoc() ) {
    return Error( "ISvcLocator* points to NULL!" );
  }
  // ChronoStatSvc
  {
    StatusCode sc = svcLoc()->service( "ChronoStatSvc", m_chronoSvc, true );
    if ( sc.isFailure() ) {
      return Error( "Unable to locate Chrono & Stat Service", sc );
    }
    if ( 0 == chronoSvc() ) {
      return Error( "Unable to locate Chrono & Stat Service" );
    }
  }
  // ToolSvc
  {
    StatusCode sc = svcLoc()->service( "ToolSvc", m_toolSvc, true );
    if ( sc.isFailure() ) {
      return Error( "Unable to locate Tool Service", sc );
    }
    if ( 0 == toolSvc() ) {
      return Error( "Unable to locate Tool Service" );
    }
  }

  if ( m_nWorkerThreads == (size_t)0 ) {
    m_nWorkerThreads = std::thread::hardware_concurrency();
    if ( m_nWorkerThreads == (size_t)0 )
      return Error( "Unable to automatically determine the number of worker threads." );
  }

  m_mTRunManagerFactory = tool<GiGaFactoryBase<GiGaMTRunManager>>( m_MTRunMgrFactoryName, this );
  if ( 0 == m_mTRunManagerFactory ) {
    return Error( "Unable to create/locate factory for GiGaMTRunManager" );
  }
  m_physListFactory = tool<GiGaFactoryBase<G4VUserPhysicsList>>( m_PhysListFactoryName, this );
  if ( 0 == m_physListFactory ) {
    return Error( "Unable to create/locate factory for G4VUserPhysicsList" );
  }
  m_workerPilotFactory = tool<GiGaFactoryBase<GiGaWorkerPilot>>( m_WorkerPilotFactoryName, this );
  if ( 0 == m_workerPilotFactory ) {
    return Error( "Unable to create/locate factory for GiGaWorkerPilot" );
  }
  m_ActionInitializerFactory = tool<GiGaFactoryBase<G4VUserActionInitialization>>( m_UserActionInitializerName, this );
  if ( 0 == m_ActionInitializerFactory ) {
    return Error( "Unable to create/locate GiGaActionInitializer" );
  }
  m_detConstFactory = tool<GiGaFactoryBase<G4VUserDetectorConstruction>>( m_DetectorConstructionName, this );
  if ( 0 == m_detConstFactory ) {
    return Error( "Unable to create/locate factory for G4VUserDetectorConstruction" );
  }
  for ( auto& toolname : m_MoniToolNames ) {
    auto tmp_tool = tool<IG4MonitoringTool>( toolname, this );
    if ( 0 == tmp_tool ) {
      return Error( "Unable to create/locate monitoring tool " + toolname );
    } else {
      m_MoniTools.push_back( tmp_tool );
    }
  }

  /// Dump all particles known to Geant4
  if ( m_printMaterials.value() ) {
    G4cout << *G4Material::GetMaterialTable();
    G4cout << "Nist Materials\n";
    G4NistManager::Instance()->ListMaterials( "all" );
  }

  // Main initialization of the run managers using the tools above
  sc = InitializeMainThread();
  if ( sc.isFailure() ) {
    return Error( "Unable to initialize main G4 thread" );
  }

  sc = InitializeWorkerThreads();
  if ( sc.isFailure() ) {
    return Error( "Unable to initialize G4 worker threads" );
  }

  /// Dump all particles known to Geant4
  if ( m_printParticles.value() ) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    particleTable->DumpTable( "all" );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// service finalization
//=============================================================================
StatusCode GiGaMT::finalize()
{
  Print( "Finalization", MSG::DEBUG, StatusCode::SUCCESS );
  // Trigger the termination of the worker threads which are blocking
  // on an empty queue right now by pushing the sentinel the worker threads
  m_payloadQueue.enqueue( std::nullopt );

  // Wait for the worker threads that will finalize now automatically
  for ( auto& t : m_workerThreads ) {
    t.join();
  }
  Print( "Finalized all G4 worker threads", MSG::ALWAYS, StatusCode::SUCCESS );
  delete GiGaMTRunManager::GetGiGaMTRunManager();

  // error printout
  if ( 0 != m_errors.size() || 0 != m_warnings.size() || 0 != m_exceptions.size() ) {
    MsgStream log( msgSvc(), name() );
    // format printout
    log << MSG::ALWAYS << " Exceptions/Errors/Warnings statistics:  " << m_exceptions.size() << "/" << m_errors.size()
        << "/" << m_warnings.size() << endmsg;
    // print exceptions counter
    for ( Counter::const_iterator excp = m_exceptions.begin(); excp != m_exceptions.end(); ++excp ) {
      log << MSG::ALWAYS << " #EXCEPTIONS= " << excp->second << " Message='" << excp->first << "'" << endmsg;
    }
    // print errors counter
    for ( Counter::const_iterator error = m_errors.begin(); error != m_errors.end(); ++error ) {
      log << MSG::ALWAYS << " #ERRORS    = " << error->second << " Message='" << error->first << "'" << endmsg;
    }
    // print warnings
    for ( Counter::const_iterator warning = m_warnings.begin(); warning != m_warnings.end(); ++warning ) {
      log << MSG::ALWAYS << " #WARNINGS  = " << warning->second << " Message='" << warning->first << "'" << endmsg;
    }
  }
  m_errors.clear();
  m_warnings.clear();
  m_exceptions.clear();

  // release all used services
  if ( 0 != toolSvc() ) {
    toolSvc()->release();
    m_toolSvc = 0;
  }
  if ( 0 != chronoSvc() ) {
    chronoSvc()->release();
    m_chronoSvc = 0;
  }
  // if( 0 != geoSrc   ()  ) { geoSrc    () -> release () ; m_geoSrc     = 0 ; }

  ///  finalize the base class
  return Service::finalize();
}

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaMT::simulate( Gaussino::MCTruthConverterPtrs&& _in,
                                                                    HepRandomEnginePtr& engine ) const
{
  auto start_time = Clock::now();
  std::list<std::promise<GiGaSimReturn>> promises;
  std::list<std::future<GiGaSimReturn>> futures;
  if ( m_splitPileUp.value() ) {
    // Submit every HepMC event separarely to the queue
    for ( auto& conv : _in ) {
      auto& prom = promises.emplace_back();
      futures.emplace_back( prom.get_future() );
      m_payloadQueue.enqueue( GiGaWorkerPayload{std::move( conv ), engine.createSubRndmEngine(), &prom} );
    }
  } else {
    auto& prom = promises.emplace_back();
    futures.emplace_back( prom.get_future() );
    Gaussino::MCTruthConverterPtr conv = Gaussino::MergeConverters( std::begin( _in ), std::end( _in ) );
    // Merge the individual pileup converters into one
    m_payloadQueue.enqueue( GiGaWorkerPayload{std::move( conv ), engine, &prom} );
  }
  G4EventProxies return_events;
  Gaussino::MCTruthPtrs return_truths;
  return_events.reserve( promises.size() );
  return_truths.reserve( promises.size() );
  for ( auto& fut : futures ) {
    auto[evt, tru] = fut.get(); // Copy illision
    return_events.emplace_back( std::move( evt ) );
    return_truths.emplace_back( std::move( tru ) );
  }

  auto end_time = Clock::now();
  debug() << "Simulation complete after " << std::setprecision( 2 )
          << std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time ).count() / 1e9 << " seconds."
          << endmsg;
  for ( auto monitool : m_MoniTools ) {
    for ( auto& g4eventproxy : return_events ) {
      monitool->monitor( *g4eventproxy.event() );
    }
  }

  return std::make_tuple<G4EventProxies, Gaussino::MCTruthPtrs>( std::move( return_events ),
                                                                 std::move( return_truths ) );
}

StatusCode GiGaMT::Print( const std::string& Message, const MSG::Level& level, const StatusCode& Status ) const
{
  MsgStream log( msgSvc(), name() );
  log << level << Message << endmsg;
  return Status;
}

StatusCode GiGaMT::Error( const std::string& Message, const StatusCode& Status ) const
{
  Stat stat( chronoSvc(), name() + ":Error" );
  // increase error counter
  m_errors[Message] += 1;
  return Print( Message, MSG::ERROR, Status );
}

StatusCode GiGaMT::Warning( const std::string& Message, const StatusCode& Status ) const
{
  Stat stat( chronoSvc(), name() + ":Warning" );
  // increase counter of warnings
  m_warnings[Message] += 1;
  return Print( Message, MSG::WARNING, Status );
}

StatusCode GiGaMT::Exception( const std::string& Message, const GaudiException& Excp, const MSG::Level& level,
                              const StatusCode& Status ) const
{
  Stat stat( chronoSvc(), Excp.tag() );
  Print( "GaudiException: catch and re-throw " + Message, level, Status );
  // increase counter of exceptions
  m_exceptions[Message] += 1;
  throw GiGaException( name() + "::" + Message, Excp, Status );
  return Status;
}

StatusCode GiGaMT::Exception( const std::string& Message, const std::exception& Excp, const MSG::Level& level,
                              const StatusCode& Status ) const
{
  Stat stat( chronoSvc(), Excp.what() );
  Print( "std::exception: catch and re-throw " + Message, level, Status );
  // increase counter of exceptions
  m_exceptions[Message] += 1;
  throw GiGaException( name() + "::" + Message + " (" + Excp.what() + ")", Status );
  return Status;
}

StatusCode GiGaMT::Exception( const std::string& Message, const MSG::Level& level, const StatusCode& Status ) const
{
  Stat stat( chronoSvc(), "*UNKNOWN Exception*" );
  Print( "GiGaException throw " + Message, level, Status );
  // increase counter of exceptions
  m_exceptions[Message] += 1;
  throw GiGaException( name() + "::" + Message, Status );
  return Status;
}

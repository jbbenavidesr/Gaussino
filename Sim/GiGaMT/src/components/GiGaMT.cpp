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
#include "GiGaMTCoreRun/GiGaMTRunManager.h"
#include "GiGaMTCoreRun/GiGaWorkerPayload.h"
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "SimInterfaces/IG4MonitoringTool.h"

#include "HepMC3/GenEvent.h"

// local
#include "GiGaMT.h"
#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"

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
    error() << "Unable to initialize the base class Service " << endmsg;
    return StatusCode::FAILURE;
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

  /// Dump all particles known to Geant4
  if ( m_printMaterials.value() ) {
    G4cout << *G4Material::GetMaterialTable();
    G4cout << "Nist Materials\n";
    G4NistManager::Instance()->ListMaterials( "all" );
  }

  // Main initialization of the run managers using the tools above
  sc = InitializeMainThread();
  if ( sc.isFailure() ) {
    error() << "Unable to initialize main G4 thread" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = InitializeWorkerThreads();
  if ( sc.isFailure() ) {
    error() << "Unable to initialize G4 worker threads" << endmsg;
    return StatusCode::FAILURE;
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
  // Trigger the termination of the worker threads which are blocking
  // on an empty queue right now by pushing the sentinel the worker threads
  m_payloadQueue.enqueue( std::nullopt );

  // Wait for the worker threads that will finalize now automatically
  for ( auto& t : m_workerThreads ) {
    t.join();
  }
  always() << "Finalized all G4 worker threads" << endmsg;
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

  ///  finalize the base class
  return Service::finalize();
}

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaMT::simulate( Gaussino::MCTruthConverterPtrs&& _in,
                                                                    HepRandomEnginePtr& engine ) const
{
  auto start_time = Clock::now();
  std::list<std::promise<Gaussino::GiGaSimReturn>> promises;
  std::list<std::future<Gaussino::GiGaSimReturn>> futures;
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
    auto [evt, tru] = fut.get(); // Copy illision
    return_events.emplace_back( std::move( evt ) );
    return_truths.emplace_back( std::move( tru ) );
  }

  auto end_time = Clock::now();
  debug() << "Simulation complete after " << std::setprecision( 2 )
          << std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time ).count() / 1e9 << " seconds."
          << endmsg;
  for ( auto monitool : m_MoniTools ) {
    for ( auto& g4eventproxy : return_events ) {
      monitool->monitor( *g4eventproxy->event() );
    }
  }

  return std::make_tuple<G4EventProxies, Gaussino::MCTruthPtrs>( std::move( return_events ),
                                                                 std::move( return_truths ) );
}

std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaMT::simulate( const HepMC3::GenEventPtrs & _in,
                                                                    HepRandomEnginePtr& engine ) const {
  return simulate(m_converterTool->BuildConverter(_in), engine);
}

StatusCode GiGaMT::simulateDecay( const HepMC3::GenParticlePtr & _in, HepRandomEnginePtr& engine ) const {

  auto result = simulate({m_converterTool->BuildConverter(_in)}, engine);
  return StatusCode::SUCCESS;
}

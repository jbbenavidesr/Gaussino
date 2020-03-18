// Gaudi includes
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/extends.h"

#include "GiGaMTReDecay/IRedecaySvc.h"
#include "HepMCUser/typedefs.h"
#include "range/v3/all.hpp"
#include <deque>
#include <sstream>
#include <future>
#include <mutex>

#include "GiGaMTCoreRun/SimResults.h"
#include "GiGaMTCoreRun/SimResultsProxyAttribute.h"
#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/Status.h"

// ============================================================================
// Interface file for class : GaussGeo
//
// ============================================================================
//
struct Counter {
  int toprocess;
  int tofinish;
};

typedef std::map<Random::SeedPair, Counter>   SeedPairCounters;
typedef std::shared_future<bool>              ReadyFlag;
typedef std::map<Random::SeedPair, ReadyFlag> ReadyMap;

class ReDecaySvc : public extends<Service, IReDecaySvc> {
public:
  Gaudi::Property<int> m_max_rd_counter{this, "nRedecay", 100};
  Gaudi::Property<int> m_rd_mode{this, "RedecayMode", 0,
                                 "0: Signal, 1: Everything heavier, 2: Heaviest ancestor of signal, 3: Heaviest "
                                 "particles of at least signal flavour."};

  Gaudi::Property<int> m_parallel_original_events{this, "ParallelOriginal", 1};
  // FIXME: Property that needs to be set to the evtmax. Should really get this from the ApplicationMgr somehow.
  Gaudi::Property<unsigned int> m_evtMax{this, "EvtMax", std::numeric_limits<unsigned int>::max(),
                                         "Number of events to be processed"};
  using extends::extends;
  virtual ~ReDecaySvc() = default;

  // Service pure member functions
  // StatusCode initialize() override ;
  // StatusCode finalize() override;

  virtual Gaussino::ReDecay::Token obtainToken( const Random::SeedPair& seedpair ) override;
  virtual void                     removeToken( Gaussino::ReDecay::Token& token ) override;
  virtual void storeOriginalHepMC(const Gaussino::ReDecay::Token &, std::vector<HepMC3::GenEventPtr> &, LHCb::GenCollisions&) override;
  virtual void storeOriginalSimResult(const Gaussino::ReDecay::Token &, const Gaussino::GiGaSimReturns &) override;

  using IReDecaySvc::getOriginalHepMCData;
  virtual std::vector<HepMCData> & getOriginalHepMCData(const Gaussino::ReDecay::Token &) override;
  using IReDecaySvc::getOriginalSimResult;
  virtual Gaussino::GiGaSimReturns getOriginalSimResult(const Gaussino::ReDecay::Token &) override;

  using IReDecaySvc::getNPileUp;
  virtual unsigned int getNPileUp(const Gaussino::ReDecay::Token &) override;

  using IReDecaySvc::getHepMCDataIterated;
  virtual HepMCData getHepMCDataIterated(const Gaussino::ReDecay::Token &) override;

  using IReDecaySvc::getEncodedOriginalEvtInfo;
  virtual unsigned long long getEncodedOriginalEvtInfo(const Gaussino::ReDecay::Token &) override;

private:
  std::recursive_mutex                                             m_svclock;
  std::atomic_uint                                                 m_processed{0};
  SeedPairCounters                                                 m_original_events_available{};
  std::map<Random::SeedPair, std::promise<Random::SeedPair>>       m_promise_store{};
  std::map<Random::SeedPair, std::shared_future<Random::SeedPair>> m_future_store{};
  std::map<Random::SeedPair, std::vector<HepMCData>> m_original_hepmc_store{};
  std::map<Random::SeedPair, Gaussino::GiGaSimReturns> m_original_simresult_store{};
  size_t                                                           QueuedEvents();
  void                                                             DumpQueue();
  LocalTL<size_t> m_ipileup_counter;
};

DECLARE_COMPONENT( ReDecaySvc )

size_t ReDecaySvc::QueuedEvents() {
  size_t c = 0;
  for ( auto& s : m_original_events_available ) { c += s.second.toprocess; }
  return c;
}

void ReDecaySvc::DumpQueue() {
  if ( !msgLevel( MSG::DEBUG ) ) { return; }
  debug() << "State of ReDecay processing queue:" << endmsg;
  for ( auto& s : m_original_events_available ) {
    debug() << " -- < " << s.first.first << " , " << s.first.second << " > Remaining: " << s.second.toprocess << " / "
            << s.second.tofinish;
    if( s.second.tofinish == m_max_rd_counter ){
      debug() << " Original in progress!" << endmsg;
    }
    debug() << endmsg;
  }
}

Gaussino::ReDecay::Token ReDecaySvc::obtainToken( const Random::SeedPair& seedpair ) {
  Gaussino::ReDecay::Token token;
  // Store the pointer to this service, this is used to trigger the callback in the
  // token destructor that tells the service that the token has been used.
  token.m_redecay_svc         = this;
  token.m_this_event_seedpair = seedpair;
  // First step, check if a new original event should be created
  // 1. There are less than m_parallel_original_events in the store of
  //    original events
  // 2. There are m_parallel_original_events in the set but the counter
  //    is 0. An original event with zero counter implies that another
  //    thread is currently finishing up the last redecay and the original
  //    event data will be cleaned once that thread is finished.
  m_processed++;
  bool                                 got_original = false;
  std::shared_future<Random::SeedPair> fut;
  // Entering critical code section that needs to be locked
  {
    std::lock_guard<std::recursive_mutex> lck{m_svclock};
    DumpQueue();
    auto nqueued     = QueuedEvents();
    token.m_original = false;

    if ( m_original_events_available.size() < (size_t)m_parallel_original_events || nqueued == 0 ) {
      // Only add a new original event if the total number of processed events plus numbers queued
      // in the redecay counters is less than the max number of events
      if ( ( m_processed - 1 + nqueued ) < m_evtMax.value() ) {
        // First check it isn't already in the map, shouldn't happen so crash if it does.
        if ( m_promise_store.find( seedpair ) != std::end( m_promise_store ) ) {
          throw GaudiException( "Original seedpair already in store", "ReDecaySvc::obtainToken", StatusCode::FAILURE );
        }
        // Register a new event
        if ( msgLevel( MSG::DEBUG ) ) {
          debug() << "Registering new original event with < " << seedpair.first << " , " << seedpair.second << " > "
                  << endmsg;
        }
        auto ret = m_promise_store.insert( std::make_pair( seedpair, std::promise<Random::SeedPair>{} ) );
        m_future_store.insert(
            std::make_pair( seedpair, std::future<Random::SeedPair>{ret.first->second.get_future()} ) );
        m_original_events_available[seedpair] = {m_max_rd_counter - 1, m_max_rd_counter};
        token.m_original                      = true;
        token.m_original_event_seedpair       = seedpair;
      }
    }
    if ( !token.m_original ) {
      // To loops, first try to find an original event which is completely processed to avoid the wait
      // Indicate by tofinish being smaller the number of redecays
      for ( auto& orgpair : m_original_events_available ) {
        if ( orgpair.second.toprocess > 0 && orgpair.second.tofinish < m_max_rd_counter ) {
          token.m_original_event_seedpair = orgpair.first;
          got_original                    = true;
          orgpair.second.toprocess--;
          // Make a copy of the shared future.
          // Access via copy is thread safe.
          fut = m_future_store[orgpair.first];
          break;
        }
      }
      // Second attempt only get those which have something to process, might need to wait!
      if ( !got_original ) {
        for ( auto& orgpair : m_original_events_available ) {
          if ( orgpair.second.toprocess > 0 ) {
            token.m_original_event_seedpair = orgpair.first;
            got_original                    = true;
            orgpair.second.toprocess--;
            // Make a copy of the shared future.
            // Access via copy is thread safe.
            fut = m_future_store[orgpair.first];
            break;
          }
        }
      }
    }
  }
  // Left critical section
  if ( !token.m_original && !got_original ) {
    throw GaudiException( "Not original but no valid original found!", "ReDecaySvc::obtainToken", StatusCode::FAILURE );
  }
  if ( !token.m_original ) {
    // Get the future. This trigger the current thread to wait until the original
    // event has been completed and the promised fulfilled
    auto retpair = fut.get();
    if ( retpair != token.m_original_event_seedpair ) {
      throw GaudiException( "Wrong SeedPair returned from future", __PRETTY_FUNCTION__, StatusCode::FAILURE );
    }
  }
  return token;
}

void ReDecaySvc::removeToken( Gaussino::ReDecay::Token& token ) {
  std::lock_guard<std::recursive_mutex> lck{m_svclock};
  if ( msgLevel( MSG::DEBUG ) ) { debug() << "Removing Token " << token << endmsg; }
  auto  orgseedpair = token.m_original_event_seedpair;
  auto& orginfo     = m_original_events_available[orgseedpair];
  orginfo.tofinish--;
  // Fulfill the promise if this was an original event. This should
  // wake up all other threads currently waiting for this original
  // event.
  if ( token.m_original ){
      m_promise_store[token.m_original_event_seedpair].set_value(token.m_original_event_seedpair);
  }
  if ( orginfo.tofinish == 0 ) {
    if ( msgLevel( MSG::DEBUG ) ) {
      DumpQueue();
      debug() << "Removing original event with < " << orgseedpair.first << " , " << orgseedpair.second << " > "
              << endmsg;
    }
    m_original_events_available.erase(orgseedpair);
    m_promise_store.erase(orgseedpair);
    m_future_store.erase(orgseedpair);
    // FIXME: Clean up actual event data
    m_original_hepmc_store.erase(orgseedpair);
    m_original_simresult_store.erase(orgseedpair);
  }
}

void ReDecaySvc::storeOriginalHepMC(const Gaussino::ReDecay::Token & token, std::vector<HepMC3::GenEventPtr> & events, LHCb::GenCollisions& collisions){
  std::lock_guard<std::recursive_mutex> lck{m_svclock};
    if ( m_original_hepmc_store.find(token.m_original_event_seedpair) != std::end(m_original_hepmc_store) ) {
      std::stringstream sstr; 
      auto [s1,s2] = token.m_original_event_seedpair;
      sstr << "Already have HepMC for original event <"  << s1 << ", " << s2 << ">";
      throw GaudiException( sstr.str(), __PRETTY_FUNCTION__, StatusCode::FAILURE );
    }
    m_original_hepmc_store[token.m_original_event_seedpair] = std::vector<HepMCData>{};
    auto & data = m_original_hepmc_store[token.m_original_event_seedpair];
    for(auto  [evt, col]: ranges::view::zip(events, collisions)){
      auto & [_evt, counter, particles, collisions] = data.emplace_back(  );
      counter = 0;
      _evt = evt;
      collisions = std::make_shared<LHCb::GenCollision>();
      collisions->setIsSignal(col->isSignal());
      collisions->setProcessType(col->processType());
      collisions->setSHat(col->sHat());
      collisions->setTHat(col->tHat());
      collisions->setUHat(col->uHat());
      collisions->setPtHat(col->ptHat());
      collisions->setX1Bjorken(col->x1Bjorken());
      collisions->setX2Bjorken(col->x2Bjorken());
      for(auto & part: evt->particles()){
        if(part->status() == HepMC3::Status::ReDecay){
          counter++;
          particles.push_back(part->id());
        }
      }
    }
}

void ReDecaySvc::storeOriginalSimResult(const Gaussino::ReDecay::Token & token, const Gaussino::GiGaSimReturns & results){
  std::lock_guard<std::recursive_mutex> lck{m_svclock};
    if ( m_original_simresult_store.find(token.m_original_event_seedpair) != std::end(m_original_simresult_store) ) {
      std::stringstream sstr; 
      auto [s1,s2] = token.m_original_event_seedpair;
      sstr << "Already have SimResults for original event <"  << s1 << ", " << s2 << ">";
      throw GaudiException( sstr.str(), __PRETTY_FUNCTION__, StatusCode::FAILURE );
    }
    m_original_simresult_store[token.m_original_event_seedpair] = results;
}

unsigned int ReDecaySvc::getNPileUp(const Gaussino::ReDecay::Token & token) {
  std::lock_guard<std::recursive_mutex> lck{m_svclock};
  auto & data = m_original_hepmc_store[token.m_original_event_seedpair];
  unsigned int ret_counter = 0;
  for(auto & [_evt, counter, particles, collisions]: data){
    if(counter>0){
      ret_counter++;
    }
  }
  return ret_counter;
}

std::vector<HepMCData> & ReDecaySvc::getOriginalHepMCData(const Gaussino::ReDecay::Token & token){
    if ( m_original_hepmc_store.find(token.m_original_event_seedpair) == std::end(m_original_hepmc_store) ) {
      std::stringstream sstr; 
      auto [s1,s2] = token.m_original_event_seedpair;
      sstr << "Could not find HepMC for original event <"  << s1 << ", " << s2 << ">";
      throw GaudiException( sstr.str(), __PRETTY_FUNCTION__, StatusCode::FAILURE );
    }
    return m_original_hepmc_store[token.m_original_event_seedpair];
}

Gaussino::GiGaSimReturns ReDecaySvc::getOriginalSimResult(const Gaussino::ReDecay::Token & token){
    if ( m_original_simresult_store.find(token.m_original_event_seedpair) == std::end(m_original_simresult_store) ) {
      std::stringstream sstr; 
      auto [s1,s2] = token.m_original_event_seedpair;
      sstr << "Could not find SimResults for original event <"  << s1 << ", " << s2 << ">";
      throw GaudiException( sstr.str(), __PRETTY_FUNCTION__, StatusCode::FAILURE );
    }
    return m_original_simresult_store[token.m_original_event_seedpair];
}

unsigned long long ReDecaySvc::getEncodedOriginalEvtInfo( const Gaussino::ReDecay::Token& token ) {
  auto [org_evtNumber, org_runNumber] = token.m_original_event_seedpair;
  unsigned long long paired =
      ( org_evtNumber + org_runNumber ) * ( org_evtNumber + org_runNumber + 1 ) / 2 + org_runNumber;
  return paired;
}

HepMCData ReDecaySvc::getHepMCDataIterated(const Gaussino::ReDecay::Token & token){
  std::lock_guard<std::recursive_mutex> lck{m_svclock};
  auto & data = m_original_hepmc_store[token.m_original_event_seedpair];
  std::vector<HepMCData> non_zero_data{};
  for(auto & d: data){
    if(std::get<1>(d)>0){
      non_zero_data.push_back(d);
    }
  }

  auto & ret_hepmcdata = non_zero_data.at(m_ipileup_counter.get());
  m_ipileup_counter.get()++;
  if(m_ipileup_counter.get() == non_zero_data.size()){
    m_ipileup_counter = 0;
  }
  return ret_hepmcdata;
}

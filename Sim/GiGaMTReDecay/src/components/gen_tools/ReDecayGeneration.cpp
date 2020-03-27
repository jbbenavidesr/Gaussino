#include "Generators/Generation.h"
#include "GiGaMTReDecay/Token.h"
#include "GiGaMTReDecay/IRedecaySvc.h"
#include "GiGaMTReDecay/IReDecaySorter.h"

#include "NewRnd/RndGlobal.h"


class ReDecayGeneration: public Generation{
  public:
  using Generation::Generation;
  using Generation::initialize;
  using Generation::finalize;

  virtual std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader>
  operator()( const LHCb::GenHeader& ) const override;

  private:
  DataObjectReadHandle<Gaussino::ReDecay::Token> m_tokenhandle{Gaussino::ReDecayToken::Default, this};
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
  ToolHandle<IReDecaySorter> m_redecaysorter{this, "Sorter", "SignalOnly"};
};

DECLARE_COMPONENT( ReDecayGeneration )




std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader> ReDecayGeneration::
operator()( const LHCb::GenHeader& old_gen_header) const {
  auto engine = createRndmEngine();
  ThreadLocalEngine::Guard rnd_guard(engine);
  auto & token = *m_tokenhandle.get();
  // Now we store a thread-local reference to this token in the service.
  // This enables us to get the current processing phase in any of the
  // subsequent tools without having to change all the interfaces again to explicitly pass
  // it around (this might be changed in the future)
  auto tokenguard = m_redecaysvc->setCurrentToken(token);
  if(m_redecaysvc->isCurrentOriginal()){
  // This calls the original generation algorithm in its default configuration.
    auto generation_return = callOperatorImplementation(old_gen_header, engine);
    auto & events = std::get<0>(generation_return);
    auto & collisions = std::get<1>(generation_return);
    // Now loop over the exisiting events and identify all particles that need to be redecayed.
    // Their decay trees are deleted in the tool
    m_redecaysorter->FlagAndRemoveReDecays(events);
    // Now save the events in the service. As they are stored as shared_ptr, we do not have to
    // do any copies as they will not be deleted at the end of the Gaudi event. GenCollisions
    // and GenHeader are recreated when needed in the ReDecay events.
    // This step also adds the callbacks to the event that will later be used to extract the signal
    // parts of the event.
    m_redecaysvc->storeOriginalHepMC(token, events, collisions);

    return generation_return;
  } else {
    auto & hepmc_data = m_redecaysvc->getOriginalHepMCData(token);
    std::tuple<std::vector<HepMC3::GenEventPtr>, LHCb::GenCollisions, LHCb::GenHeader> rettuple;
    auto & [retevents, collisions, header] = rettuple;
    for(auto & [evt, n_redecays, ids, col]: hepmc_data){
      retevents.push_back(evt);
      auto _col = new LHCb::GenCollision();
      _col->setIsSignal(col->isSignal());
      _col->setProcessType(col->processType());
      _col->setSHat(col->sHat());
      _col->setTHat(col->tHat());
      _col->setUHat(col->uHat());
      _col->setPtHat(col->ptHat());
      _col->setX1Bjorken(col->x1Bjorken());
      _col->setX2Bjorken(col->x2Bjorken());
      collisions.insert(_col);
    }
    header.setEvType(old_gen_header.evType());
    header.setEvtNumber(old_gen_header.evtNumber());
    header.setRunNumber(old_gen_header.runNumber());
    // Get the header and update the information
    if( !header.evType() ){
      header.setEvType( m_eventType );  
    }

    return rettuple;
  }
}

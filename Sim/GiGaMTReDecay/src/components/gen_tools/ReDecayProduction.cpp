#include "Event/GenCollision.h"
#include "Kernel/IParticlePropertySvc.h"

#include "GaudiAlg/GaudiTool.h"
#include "GenInterfaces/IProductionTool.h"
#include "GiGaMTReDecay/IRedecaySvc.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/ParticleAttribute.h"

/**
 * Implementation of a production tool to be used in SignalPlain as the
 * production tool when generating the redecayed event. Its only purpose is the
 * construction of an event based on the information stored in the service when
 * asked to 'generate' an event. The decay happens due to the decay heavy
 * particles mechanism in SignalPlain.
 *
 * @class  RedecayProduction
 * @file   RedecayProduction.h
 * @author Dominik Muller
 * @date   2020-02-19
 */
class ReDecayProduction : public GaudiTool, virtual public IProductionTool {
public:
  /// Default constructor.
  ReDecayProduction( const std::string& type, const std::string& name, const IInterface* parent );
  /// Set of functions we need for this
  StatusCode generateEvent( HepMC3::GenEventPtr, LHCb::GenCollision*, HepRandomEnginePtr& ) const override;

  // set of functions which need a dummy implementation as they are abstract
  // in IProductionTool and some are called in ExternalGenerator so be sure to
  // not do something silly
  StatusCode initializeGenerator() override { return StatusCode::SUCCESS; };
  void       setStable( const LHCb::ParticleProperty* ) override {}
  void       updateParticleProperties( const LHCb::ParticleProperty* ) override {}
  void       turnOnFragmentation() override {}
  void       turnOffFragmentation() override {}
  StatusCode hadronize( HepMC3::GenEventPtr, LHCb::GenCollision*, HepRandomEnginePtr& ) override {
    return StatusCode::SUCCESS;
  }
  void       savePartonEvent( HepMC3::GenEventPtr ) override {}
  void       retrievePartonEvent( HepMC3::GenEventPtr ) override {}
  void       printRunningConditions() const override {}
  bool       isSpecialParticle( const LHCb::ParticleProperty* ) const override { return false; }
  StatusCode setupForcedFragmentation( const int ) override { return StatusCode::SUCCESS; }

private:
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
};

DECLARE_COMPONENT( ReDecayProduction )

ReDecayProduction::ReDecayProduction( const std::string& type, const std::string& name, const IInterface* parent )
    : GaudiTool( type, name, parent ) {

  // Declare the tool properties.
  declareInterface<IProductionTool>( this );
}

StatusCode ReDecayProduction::generateEvent( HepMC3::GenEventPtr theEvent, LHCb::GenCollision*,
                                             HepRandomEnginePtr& ) const {
  // Let's construct a fake event
  auto original_hepmcdata                 = m_redecaysvc->getHepMCDataIterated();
  auto& [orgevt, _counter, redids, _cols] = original_hepmcdata;

  auto momentum_unit = orgevt->momentum_unit();
  auto length_unit = orgevt->length_unit();
  theEvent->set_units(momentum_unit, length_unit);
  HepMC3::GenVertexPtr dummy_vertex{nullptr};
  for ( auto partid : redids ) {
    // Get the particle from its ID. HepMC3 respect the position in the
    // internal particle vector but shifted by one, hence -1
    auto orgpart  = orgevt->particles().at( partid - 1 );
    auto mom      = orgpart->momentum();
    auto origin   = orgpart->production_vertex()->position();
    auto thePdgId = orgpart->pdg_id();
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Making a particle for PDG ID " << thePdgId << endmsg;
      debug() << "#### Momentum (PT, Eta, Phi, E) = (" << mom.pt() << ", " << mom.eta() << ", " << mom.phi() << ", "
              << mom.e() << ")" << endmsg;
    }
    /*Structure of this event: -N> particle, X vertex*/
    /*-beam-> X -tag1-> X -redecay1-> */
    /*          -tag2-> X -redecay2-> */
    /*          -tag3-> X -redecay3-> */
    /*          etc ... */

    auto v =
        std::make_shared<HepMC3::GenVertex>( HepMC3::FourVector( origin.x(), origin.y(), origin.z(), origin.t() ) );
    if ( dummy_vertex == nullptr ) {
      dummy_vertex =
          std::make_shared<HepMC3::GenVertex>( HepMC3::FourVector( origin.x(), origin.y(), origin.z(), origin.t() ) );
      auto beam = std::make_shared<HepMC3::GenParticle>( HepMC3::FourVector( 0, 0, 1, 1 ), 0,
                                                         LHCb::HepMCEvent::DocumentationParticle );
      dummy_vertex->add_particle_in( beam );
      theEvent->add_vertex( dummy_vertex );
      if ( theEvent->beams().size() == 0 ) { theEvent->set_beam_particles( beam, beam ); }
    }
    // create HepMC particle
    auto p = std::make_shared<HepMC3::GenParticle>( HepMC3::FourVector( mom.px(), mom.py(), mom.pz(), mom.e() ),
                                                    thePdgId, LHCb::HepMCEvent::StableInProdGen );
    // Create tagging HepMC particle and attach to the same vertex
    // This particle is needed to link the actual production vertex of the particle
    // to the dummy PV vertex.
    auto tag = std::make_shared<HepMC3::GenParticle>( HepMC3::FourVector( mom.px(), mom.py(), mom.pz(), mom.e() ), 0,
                                                      LHCb::HepMCEvent::StableInProdGen );
    v->add_particle_in( tag );
    dummy_vertex->add_particle_out( tag );

    v->add_particle_out( p );
    theEvent->add_vertex( v );
    // Only add the attribute after the vertex is pushed into the event, otherwise they are not saved ...
    p->add_attribute( Gaussino::HepMC::Attributes::ReDecayOriginalParticle,
                      std::make_shared<HepMC3::ParticleAttribute>( orgpart ) );

    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Generated fake event" << endmsg;
      debug() << PrintDecay( *std::begin( theEvent->beams() ) ) << endmsg;
    }
  }
  // Copy the signal process ID of the main event to ensure this event is converted identically
  // when preparing the simulation
  if ( auto attr = orgevt->attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::SignalProcessID ); attr ) {
    theEvent->add_attribute( Gaussino::HepMC::Attributes::SignalProcessID,
                             std::make_shared<HepMC3::IntAttribute>( attr->value() ) );
  }

  return StatusCode::SUCCESS;
}

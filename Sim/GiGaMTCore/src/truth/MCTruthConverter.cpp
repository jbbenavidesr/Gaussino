#include "GiGaMTTruth/MCTruthConverter.h"
#include "ConverterInfo.h"
#include <functional>
#include <stdexcept>

namespace Gaussino
{
  MCTruthBase::MCTruthBase( MCTruthBase&& right )
      : m_convInfos{std::move( right.m_convInfos )}
      , m_linkedParticles{std::move( right.m_linkedParticles )}
      , m_hepmc_to_linked{std::move( right.m_hepmc_to_linked )}
      , m_geant4_to_linked{std::move( right.m_geant4_to_linked )}
      , m_tracking_to_linked{std::move( right.m_tracking_to_linked )}
      , m_root_particles{std::move( right.m_root_particles )}
      , m_hepmc_event{right.m_hepmc_event}
      , m_geant4_event{right.m_geant4_event}
  {
  }

  ///////////////////////////////////////////////////////////
  // MCTruthConverter
  //////////////////////////////////////////////////////////

  void MCTruthConverter::Declare( HepMC::GenParticlePtr& particle, ConversionType type )
  {
    if ( m_hepmc_event != particle->parent_event() ) {
      throw std::runtime_error( "Particles of different HepMC events registered in same converter." );
    }
    auto ptr = std::make_unique<LinkedParticle>( particle );
    ptr->SetType( type );
    m_hepmc_to_linked[particle->id()] = ptr.get();
    m_linkedParticles.push_back( std::move( ptr ) );
    m_hepmc_event = particle->parent_event();
  }

  ///////////////////////////////////////////////////////////
  // MCTruthTracker
  //////////////////////////////////////////////////////////

  MCTruthTracker::MCTruthTracker( MCTruthConverter&& converter, G4Event* event )
      : MCTruthConverter{std::move( converter )}
  {
    m_geant4_event = event;
    DoInitialLinking();
    if ( m_geant4_event ) {
      AddToG4Event( event );
    }
  }
  void MCTruthTracker::DoInitialLinking()
  {
    // We find a root particle and then process and link all its children by linking the LinkedParticle
    // instances together.
    // Root particles are identified as those with the smallest ID
    std::set<int> IDs;
    for ( auto& l : m_linkedParticles ) {
      if ( auto& part = l->HepMC(); part ) {
        IDs.insert( part->id() );
      }
    }
    while ( IDs.size() > 0 ) {
      // Make use of the set being sorted, with smallest ID at the front
      // While taking care of a particle we also remove its ID from the
      // set before moving on to its children, doing the same etc ...
      // After a full decay tree is processed, the first element will then
      // again be the start of a new decay chain so the procedure is 
      // repeated until the set is empty
      int root_id  = *std::begin( IDs );
      auto root_lp = m_hepmc_to_linked[root_id];
      m_root_particles.insert( root_lp );
      std::function<void( HepMC::GenParticlePtr, HepMC::GenParticlePtr )> child_converter =
          [&]( HepMC::GenParticlePtr part, HepMC::GenParticlePtr parent = nullptr ) {
            LinkedParticle* plinked{nullptr};
            LinkedParticle* clinked{nullptr};
            if ( parent ) {
              // The parent should always have a linked particle if provided
              plinked = m_hepmc_to_linked[parent->id()];
            }
            if ( part && m_hepmc_to_linked.find( part->id() ) != std::end(m_hepmc_to_linked) ) {
              // HepMC child might not have a matching linked particle if it is not supposed to be converted
              clinked = m_hepmc_to_linked[part->id()];
              // As we are now taking care of this particle, remove it from the set.
              // NOTE: HepMC3 does not allow loops so this is safe
              IDs.erase(part->id());
            }
            if ( clinked && plinked ) {
              clinked->AddParent( plinked );
              plinked->AddChild( clinked );
            }
            // If a clinked particle was found, the current particle becomes the parent for its children,
            // otherwise this HepMC particle was not supposed to be converted and is skipped.
            auto new_parent = clinked ? part : parent;
            for ( auto& child : part->children()){
              child_converter(child, new_parent);
            }
          };
    }
  }
  void MCTruthTracker::AddToG4Event( G4Event* g4event ) { DoInitialLinking(); }
}

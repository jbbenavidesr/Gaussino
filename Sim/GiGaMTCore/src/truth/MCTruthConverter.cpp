#include "GiGaMTCore/Truth/MCTruthConverter.h"
#include "ConverterInfo.h"
#include "Geant4/G4SystemOfUnits.hh"
#include "GiGaMTCore/Truth/GaussinoPrimaryParticleInformation.h"
#include "Helpers.h"
#include <functional>
#include <stdexcept>
#include "GiGaMTCore/Truth/LinkedParticleHelpers.h"

bool essentiallyEqual( float a, float b, float epsilon = 0.00001 )
{
  return fabs( a - b ) <= ( ( fabs( a ) > fabs( b ) ? fabs( b ) : fabs( a ) ) * epsilon );
}

// Helper function to verify that ones particles endvertex is at the same position as the others
// origin in the original HepMC record, i.e. that particles that have been skipped in between didn't fly
bool VerifyLink( LinkedParticle* a, LinkedParticle* b )
{
  if ( !essentiallyEqual( a->GetEndPosition().x(), b->GetOriginPosition().x() ) ) return false;
  if ( !essentiallyEqual( a->GetEndPosition().y(), b->GetOriginPosition().y() ) ) return false;
  if ( !essentiallyEqual( a->GetEndPosition().z(), b->GetOriginPosition().z() ) ) return false;
  if ( !essentiallyEqual( a->GetEndPosition().t(), b->GetOriginPosition().t() ) ) return false;
  return true;
}

namespace Gaussino
{
  MCTruthData::MCTruthData( MCTruthData&& right ) noexcept
      : m_linkedParticles{std::move( right.m_linkedParticles )}
      , m_hepmc_to_linked{std::move( right.m_hepmc_to_linked )}
      , m_primary_to_linked{std::move( right.m_primary_to_linked )}
      , m_tracking_to_linked{std::move( right.m_tracking_to_linked )}
      , m_root_particles{std::move( right.m_root_particles )}
      , m_geant4_event{right.m_geant4_event}
      , m_geant4_vertex{right.m_geant4_vertex}
  {
    right.m_linkedParticles.clear();
    right.m_hepmc_to_linked.clear();
    right.m_primary_to_linked.clear();
    right.m_tracking_to_linked.clear();
    right.m_root_particles.clear();
  }
  MCTruthData::~MCTruthData()
  {
    for ( auto lp : m_linkedParticles ) {
      delete lp;
    }
  }

  size_t MCTruthData::GetNParticles() const { return m_linkedParticles.size(); }

  size_t MCTruthData::GetNVertices() const
  {
    std::set<LinkedVertex*> tmp_store;
    for ( auto& lp : m_linkedParticles ) {
      tmp_store.insert( lp->GetProdVtx().get() );
      for ( auto& ev : lp->GetEndVtxs() ) {
        tmp_store.insert( ev.get() );
      }
    }
    return tmp_store.size();
  }

  ///////////////////////////////////////////////////////////
  // MCTruthConverter
  //////////////////////////////////////////////////////////

  void MCTruthConverter::Declare( const HepMC3::ConstGenParticlePtr& particle, ConversionType type )
  {
    auto ptr = new LinkedParticle{particle};
    ptr->SetType( type );
    if ( m_hepmc_to_linked.find( particle->parent_event() ) == std::end( m_hepmc_to_linked ) ) {
      m_hepmc_to_linked[particle->parent_event()] = {};
    }
    // Now fill the table for the specific event
    auto& table           = m_hepmc_to_linked[particle->parent_event()];
    table[particle->id()] = ptr;
    m_linkedParticles.insert( ptr );
  }

  void MCTruthConverter::AddConverter( MCTruthConverter&& conv )
  {
    // As this can only be called on a MCTruthConverter, only Declare could have been used.
    // Hence only need to merge the contents of m_linkedParticles and m_hepmc_to_linked
    for ( auto& lp : conv.m_linkedParticles ) {
      m_linkedParticles.insert( std::move( lp ) );
    }
    for ( auto& hep_lp : conv.m_hepmc_to_linked ) {
      if ( m_hepmc_to_linked.find( hep_lp.first ) != std::end( m_hepmc_to_linked ) ) {
        for ( auto& int_lp : hep_lp.second ) {
          m_hepmc_to_linked[hep_lp.first].insert( int_lp );
        }
      } else {
        m_hepmc_to_linked.insert( hep_lp );
      }
    }
    conv.m_linkedParticles.clear();
  }

  ///////////////////////////////////////////////////////////
  // MCTruthTracker
  //////////////////////////////////////////////////////////

  MCTruthTracker::MCTruthTracker( MCTruthConverter&& converter, G4Event* event ) : MCTruthData{std::move( converter )}
  {
    m_geant4_event = event;
    DoInitialLinking();
    if ( m_geant4_event ) {
      AddToG4Event( event );
    } else {
      for ( auto& lp : m_linkedParticles ) {
        lp->SetType( Gaussino::ConversionType::MC );
      }
    }
  }
  void MCTruthTracker::DoInitialLinking()
  {
    // We find a root particle and then process and link all its children by linking the LinkedParticle
    // instances together.
    // Root particles are identified as those with the smallest ID
    for ( auto& hepmcs : m_hepmc_to_linked ) {
      auto& table = hepmcs.second;
      std::set<int> IDs;
      for ( auto& l : table ) {
        if ( auto part = l.second->HepMC(); part ) {
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
        auto root_lp = table[root_id];
        m_root_particles.insert( root_lp );
        std::function<void( const HepMC3::GenParticle*, const HepMC3::GenParticle* )> child_converter =
            [&]( const HepMC3::GenParticle* part, const HepMC3::GenParticle* parent ) {
              LinkedParticle* plinked{nullptr};
              LinkedParticle* clinked{nullptr};
              if ( parent ) {
                // The parent should always have a linked particle if provided
                plinked = table[parent->id()];
              }
              if ( part && table.find( part->id() ) != std::end( table ) ) {
                // HepMC child might not have a matching linked particle if it is not supposed to be converted
                clinked = table[part->id()];
                // As we are now taking care of this particle, remove it from the set.
                // NOTE: HepMC3 does not allow loops so this is safe
                IDs.erase( part->id() );
                clinked->m_tracker = this;
              }
              if ( clinked && plinked ) {
                if ( !VerifyLink( plinked, clinked ) ) {
                  throw std::runtime_error( "LinkedParticles to be linked have mismatching vertex positions" );
                }
                clinked->AddParent( plinked );
                plinked->AddChild( clinked );
              }
              // If a clinked particle was found, the current particle becomes the parent for its children,
              // otherwise this HepMC particle was not supposed to be converted and is skipped.
              const HepMC3::GenParticle* new_parent = clinked ? part : parent;
              // Check if the particle has oscillated
              auto oscillated = Gaussino::LinkedParticleHelpers::hasOscillated( part );
              if ( oscillated ) {
                // Now setting part to oscillated which will skip the oscillated version
                // of the particle and assign them to the linked particle of the
                // non-oscillated version
                if ( clinked ) {
                  clinked->m_hasOscillated = true;
                }
                if ( table.find( oscillated->id() ) != std::end( table ) ) {
                  // This check is performed using the HepMC particle structure.
                  // If the particles has oscillated, we should mark the corresponding
                  // LinkedParticle accordingly and skip the direct daughter (the oscillated particle)
                  // If the oscillated particle is declared as a LinkedParticle we need to remove it.
                  auto oscillated_lp = table[oscillated->id()];
                  table.erase( oscillated->id() );
                  IDs.erase( oscillated->id() );
                  m_linkedParticles.erase( oscillated_lp );
                  delete oscillated_lp;
                }
                part = oscillated.get();
              }
              for ( auto& child : part->children() ) {
                child_converter( child.get(), new_parent );
              }
            };
        child_converter( root_lp->HepMC(), nullptr );
      }
    }

    // We check that none of the root particle had a LinkedVertex assigned as production vertex.
    // Otherwise throw because this should not happen.
    for ( auto& rp : m_root_particles ) {
      if ( rp->m_prodvtx ) {
        std::stringstream msg;
        msg << __PRETTY_FUNCTION__ << "LinkedParticle " << *rp
            << " in root particle set but has production vertex assigned";
        throw std::runtime_error( msg.str() );
      }
    }
  }
  void MCTruthTracker::AddToG4Event( G4Event* g4event )
  {
    // Start conversion at the ROOT particles as always, first identify their origin vertex
    unsigned int iPrimary = 1;
    for ( auto& rp : m_root_particles ) {
      G4PrimaryVertex* g4vertex{nullptr};
      // Check if we have created another vertex at this position before. In the most likely
      // use-case, as one HepMC event <-> one pp interaction <-> one converter, all root particles
      // should originate from the same position (the PV). However, some other non LHCb users might
      // come up with something weird so we will account for this possibility.
      for ( auto vtx : m_geant4_vertex ) {
        if ( essentiallyEqual( vtx->GetPosition().x(), rp->HepMC()->production_vertex()->position().x() ) &&
             essentiallyEqual( vtx->GetPosition().y(), rp->HepMC()->production_vertex()->position().y() ) &&
             essentiallyEqual( vtx->GetPosition().z(), rp->HepMC()->production_vertex()->position().z() ) &&
             essentiallyEqual( vtx->GetT0(), rp->HepMC()->production_vertex()->position().t() ) ) {
          g4vertex = vtx;
          break;
        }
      }
      if ( !g4vertex ) {
        auto PVposition = rp->HepMC()->production_vertex()->position();
        g4vertex = new G4PrimaryVertex( PVposition.x() * mm, PVposition.y() * mm, PVposition.z() * mm, PVposition.t() );
        m_geant4_vertex.insert( g4vertex );
        g4event->AddPrimaryVertex( g4vertex );
      }
      std::function<void( LinkedParticle * part, LinkedParticle * parent )> convert = [&]( LinkedParticle* part,
                                                                                           LinkedParticle* parent ) {
        // If part already has a G4Primary associated we stop here as we have already treated
        // this decay tree.
        if ( part->G4Primary() ) {
          return;
        }
        // Part is the current particle, parent points to the parent linkedparticle that was last
        // converted to geant4
        if ( part->m_conversion_type == Gaussino::ConversionType::G4 ) {
          part->G4Primary() = new G4PrimaryParticle( part->GetPDG(), part->GetMomentum().px() * MeV,
                                                     part->GetMomentum().py() * MeV, part->GetMomentum().pz() * MeV );
          part->G4Primary()->SetMass( part->HepMC()->generated_mass() * MeV );

          // Register the particle in the map and save this ID with the primary particle to
          // later register the result of the simulation. This ID is used instead of directly
          // storing the LinkedParticle in the PrimaryParticleInformation to prevent the
          // users from accessing the LinkedParticles before everything is ready.
          m_primary_to_linked[iPrimary] = part;
          GaussinoPrimaryParticleInformation::Get( part->G4Primary() )->setLinkedID( iPrimary );
          iPrimary++;

          if ( parent && parent->G4Primary() ) {
            // Attach as daughter if parent exists and has a G4Primary associated to it
            parent->G4Primary()->SetDaughter( part->G4Primary() );
          } else {
            // If not, this is a root G4 particle so attach it to the vertex
            g4vertex->SetPrimary( part->G4Primary() );
          }
          double dtime = part->GetDecayTimeHepMC();
          if ( dtime >= 0 ) {
            part->G4Primary()->SetProperTime( dtime );
          }
          // If converted, this particle will be the parent of its children tracks
          parent = part;
        }
        for ( auto lp : part->GetChildren() ) {
          convert( lp, parent );
        }
      };
      // Initiate the conversion for the root particle.
      // Nullptr indicates that no parent is present.
      convert( rp, nullptr );
    }
  }

  void MCTruthTracker::RegisterPrimary( Gaussino::G4TruthParticle* particle, unsigned int primaryID )
  {
    auto primaryLP                               = m_primary_to_linked[primaryID];
    primaryLP->G4Truth()                         = particle;
    m_tracking_to_linked[particle->GetTrackID()] = primaryLP;
  }

  void MCTruthTracker::Declare( Gaussino::G4TruthParticle* particle, int parentID )
  {
    if ( m_tracking_to_linked.find( parentID ) == std::end( m_tracking_to_linked ) ) {
      G4cerr << __PRETTY_FUNCTION__ << " no linked particle with parentID found. Skipping!" << G4endl;
      return;
    }
    auto parentLP = m_tracking_to_linked[parentID];
    auto ptr      = new LinkedParticle{particle};
    ptr->SetType( ConversionType::FROMG4 );
    m_tracking_to_linked[particle->GetTrackID()] = ptr;

    // Link the particles
    parentLP->AddChild( ptr );
    ptr->AddParent( parentLP );
    // Need to move it into the storage at the end so ptr remains valid for setting the links
    m_linkedParticles.insert( ptr );
  }

  ///////////////////////////////////////////////////////////
  // MCTruth
  //////////////////////////////////////////////////////////

  MCTruth::MCTruth( MCTruthTracker&& tracker ) : MCTruthData{std::move( tracker )} { DoCleanup(); }

  void MCTruth::DoCleanup()
  {
    // Idea: Loop recursively and remove un-used decay trees:
    // Definitely remove particles and their decays that have been handed over
    // to G4 but did not receive a corresponding G4Truth particle. Typical example
    // is a long-lived particle decaying into stable particles but undergoing some
    // material interaction before this decay, in which case the children need
    // to be removed. We assume no loops for now and remove the entire decay tree of such particles
    //

    std::function<bool( LinkedParticle* )> hasG4ChildWithoutG4Truth = [&]( LinkedParticle* lp ) {
      bool found = false;
      for ( auto child : lp->GetChildren() ) {
        if ( Gaussino::LPUtils::ShouldHaveButWasNotSimulated( child ) ) {
          found = true;
        } else if ( child->GetType() == ConversionType::MC ) {
          // Recursively call on child if child itself was not given
          // to G4
          found = found || hasG4ChildWithoutG4Truth( child );
        }
      }
      return found;
    };
    // Identify the heads of decays that need to be deleted. Two options:
    // 1. MC particle produced that did not get produced in decay of simulated G4 particle
    // 2. G4 particle that was not simulated but whose parent was
    std::set<LinkedParticle*> to_delete;
    for ( auto& lp : m_linkedParticles ) {
      if ( lp->GetType() == ConversionType::MC && Gaussino::LPUtils::HasSimulatedG4Parent( lp ) &&
           hasG4ChildWithoutG4Truth( lp ) ) {
        to_delete.insert( lp );
      }
      if ( lp->GetType() == ConversionType::G4 && Gaussino::LPUtils::HasSimulatedG4Parent( lp ) &&
           Gaussino::LPUtils::ShouldHaveButWasNotSimulated( lp ) ) {
        to_delete.insert( lp );
      }
    }
    for ( auto lp : to_delete ) {
      EraseDecayTree( lp );
    }
    to_delete.clear();
  }

  const LinkedParticle* MCTruth::GetParticleFromTrackID( int trackid ) const
  {
    if ( auto it = m_tracking_to_linked.find( trackid ); it != std::end( m_tracking_to_linked ) ) {
      return it->second;
    }
    return nullptr;
  }

  void MCTruth::EraseLinkedParticle( LinkedParticle* lp )
  {
    m_linkedParticles.erase( lp );
    // Get copies of the container holding smartpointers to the
    // vertices. This is done to allow a cleanup of the vertices
    // after the particle itself has been deleted.
    // If one of the vertices is obsolete after the cleanup, the
    // destruction of this container will trigger the deletion
    // of the vertex itself.
    auto prodvtx = lp->GetProdVtx();
    auto endvtxs = lp->GetEndVtxs();
    // Get the sets of parents and children for convenience. These
    // are raw pointers and only used to fix the connections if
    // an intermediate particle is removed.
    auto parents  = lp->GetParents();
    auto children = lp->GetChildren();
    delete lp; // Destructor will remove this particle from the vertices
    // The production vertex of this particle was particle was an endvertex
    // of its parents. If the particle was the only one produced in this vertex,
    // the vertex now has no more outgoing particles and is deleted from the partents.
    if ( prodvtx || prodvtx->outgoing_particles.size() == 0 ) {
      for ( auto& in : prodvtx->incoming_particle ) {
        in->GetEndVtxs().erase( prodvtx );
      }
    }
    for ( auto& endvtx : endvtxs ) {
      if ( endvtx->incoming_particle.size() == 0 ) {
        // Unset the production vertex for particles leaving this
        // vertex. Reconnecting the particles in the following
        // will give the particles a new production vertex
        for ( auto& out : endvtx->outgoing_particles ) {
          out->GetProdVtx() = nullptr;
        }
      }
    }
    // Lastly, reconnect the particles correctly
    for ( auto& parent : parents ) {
      for ( auto& child : children ) {
        parent->AddChild( child );
        child->AddParent( parent );
      }
    }
  }
  void MCTruth::EraseDecayTree( LinkedParticle* lp )
  {
    for ( auto child : lp->GetChildren() ) {
      EraseDecayTree( child );
    }
    EraseLinkedParticle( lp );
  }
}

/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#include "GiGaMTCoreTruth/MCTruthConverter.h"
#include "ConverterInfo.h"
#include "GaudiKernel/GaudiException.h"
#include "Geant4/G4SystemOfUnits.hh"
#include "GiGaMTCoreTruth/GaussinoPrimaryParticleInformation.h"
#include "GiGaMTCoreTruth/LinkedParticleHelpers.h"
#include "GiGaMTCoreRun/SimResults.h"
#include "GiGaMTCoreRun/SimResultsProxyAttribute.h"
#include "Defaults/HepMCAttributes.h"
#include "Helpers.h"
#include "HepMC3/Relatives.h"
#include "HepMCUser/Status.h"
#include "HepMCUtils/HepMCUtils.h"
#include "range/v3/all.hpp"
#include <functional>
#include <stdexcept>

namespace Gaussino
{
  MCTruthData::MCTruthData( MCTruthData&& right ) noexcept
      : m_linkedParticles{std::move( right.m_linkedParticles )}
      , m_hepmc_to_linked{std::move( right.m_hepmc_to_linked )}
      , m_primary_to_linked{std::move( right.m_primary_to_linked )}
      , m_tracking_to_linked{std::move( right.m_tracking_to_linked )}
      , m_buffered_endvertices{std::move( right.m_buffered_endvertices )}
      , m_root_particles{std::move( right.m_root_particles )}
      , m_geant4_event{right.m_geant4_event}
      , m_geant4_vertex{right.m_geant4_vertex}
      , m_pcounter{right.m_pcounter}
  {
    right.m_linkedParticles.clear();
    right.m_hepmc_to_linked.clear();
    right.m_primary_to_linked.clear();
    right.m_buffered_endvertices.clear();
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

  int MCTruthData::VerifyStructure() const
  {

    std::set<LinkedParticle*> visited;
    std::function<void( LinkedParticle* )> rec_down = [&]( LinkedParticle* lp ) {
      visited.insert( lp );
      for ( auto& dp : lp->GetChildren() ) {
        rec_down( dp );
      }
    };
    for ( auto& rp : m_root_particles ) {
      rec_down( rp );
    }
    if ( visited.size() < m_linkedParticles.size() ) {
      return 1;
      throw GaudiException{"Not all particles reachable from root particles", "MCTruth", StatusCode::FAILURE};
    }

    visited.clear();
    std::function<void( LinkedParticle* )> rec_up = [&]( LinkedParticle* lp ) {
      visited.insert( lp );
      for ( auto& dp : lp->GetParents() ) {
        rec_up( dp );
      }
    };
    for ( auto& rp : m_linkedParticles ) {
      if ( rp->GetEndVtxs().empty() || rp->GetChildren().size() == 0 ) {
        rec_up( rp );
      }
    }
    if ( visited.size() < m_linkedParticles.size() ) {
      return 2;
    }
    return 0;
  }

  void MCTruthData::RemoveDecayTreesWithSimResults() {
    std::function<void( LinkedParticle* )> rec_checker = [&]( LinkedParticle* lp ) {
      if ( auto hepmc = lp->HepMC(); hepmc ) {
        if ( auto [g4proxyptr, mctruthptr] =
                 hepmc->attribute<HepMC3::SimResultsAttribute>( Gaussino::HepMC::Attributes::SimResults )->value();
             g4proxyptr && mctruthptr ) {
          if ( auto prodvtx = lp->GetProdVtx(); prodvtx ) {
            prodvtx->outgoing_mctruths.insert( mctruthptr );
          } else {
            this->m_slave_mctruths.push_back( mctruthptr );
          }
          m_contained_proxies.insert( g4proxyptr );
          for ( auto& contained : mctruthptr->m_contained_proxies ) { m_contained_proxies.insert( contained ); }
          EraseDecayTree( lp );
        } else {
          for(auto child:lp->GetChildren()){
            rec_checker(child);
          }
        }
      }
    };
    for ( auto lp : m_root_particles ) { rec_checker( lp ); }
  }

  ///////////////////////////////////////////////////////////
  // MCTruthConverter
  //////////////////////////////////////////////////////////

  void MCTruthConverter::Declare( const HepMC3::ConstGenParticlePtr& particle, ConversionType type )
  {
    auto ptr = new LinkedParticle{m_pcounter++, particle};
    // If the particle has the ReDecay status code, mark the corresponding linked particle accordingly.
    // This information will later be used to associate the correct signal event components here.
    ptr->SetType( type );
    if ( m_hepmc_to_linked.find( particle->parent_event() ) == std::end( m_hepmc_to_linked ) ) {
      m_hepmc_to_linked[particle->parent_event()] = {};
    }
    // Now fill the table for the specific event
    auto& table           = m_hepmc_to_linked[particle->parent_event()];
    table[particle->id()] = ptr;
    m_linkedParticles.insert( ptr );

    if ( particle->status() == HepMC3::Status::SignalInLabFrame ) {
      ptr->m_isSignal = true;
    }
  }

  void MCTruthConverter::AddConverter( MCTruthConverter&& conv )
  {
    // As this can only be called on a MCTruthConverter, only Declare could have been used.
    // Hence only need to merge the contents of m_linkedParticles and m_hepmc_to_linked
    for ( auto& lp : conv.m_linkedParticles ) {
      // Merge linked particles. This implies that conv transfers ownership to this
      m_linkedParticles.insert( lp );
    }
    for ( auto& hep_lp : conv.m_hepmc_to_linked ) {
      // Need ensure that the HepMC event / all its linked particles are correctly sorted.
      // This is necessary to avoid collisions in HepMC particle ID which is used to identify
      // root vertices during the particle linking
      if ( m_hepmc_to_linked.find( hep_lp.first ) != std::end( m_hepmc_to_linked ) ) {
        for ( auto& int_lp : hep_lp.second ) {
          m_hepmc_to_linked[hep_lp.first].insert( int_lp );
        }
      } else {
        m_hepmc_to_linked.insert( hep_lp );
      }
      // Shift the IDs of the just merged particles to avoid collisions
      for ( auto lp : conv.m_linkedParticles ) {
        lp->SetID( m_pcounter + lp->GetID() );
      }
      m_pcounter += conv.m_linkedParticles.size();
    }
    // Explicitly clear the container of linked particles of conv. As this is a set of plain
    // ptr, the particles themselves are not deleted. However, this prevents the destructor
    // of conv deleting them
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
        if(lp->GetType() == Gaussino::ConversionType::G4 )lp->SetType( Gaussino::ConversionType::MC );
      }
    }
  }

  LinkedParticle::PtrSet MCTruthTracker::IdentifyRootParticles( const HepMC3::GenEvent* evt )
  {
    LinkedParticle::PtrSet root_particles;
    auto& table = m_hepmc_to_linked[evt];
    std::vector<LinkedParticle*> tmp_linked;
    auto tmp_link = table | ranges::views::transform( []( auto& pr ) { return pr.second; } );

    // Find all root particles, i.e. those
    std::copy_if( std::begin( tmp_link ), std::end( tmp_link ),
                  std::inserter( root_particles, std::begin( root_particles ) ), [&]( auto v ) {
                    for ( auto hepmcptr : HepMC3::Relatives::ANCESTORS( v->HepMC() ) ) {
                      if ( table.find( hepmcptr->id() ) != std::end( table ) ) {
                        return false;
                      }
                    }
                    return true;
                  } );
    return root_particles;
  }

  void MCTruthTracker::DoInitialLinking()
  {
    // We find a root particle and then process and link all its children by linking the LinkedParticle
    // instances together.
    // Root particles are identified as those with the smallest ID
    // If this converter contains multiple HepMC events (i.e. pileup collisions), their IDs would collide so
    // we do this separately for each HepMC event
    for ( auto& hepmcs : m_hepmc_to_linked ) {
      auto& table             = hepmcs.second;
      auto tmp_root_particles = IdentifyRootParticles( hepmcs.first );
      for ( auto root_lp : tmp_root_particles ) {
        m_root_particles.insert( root_lp );
        std::function<void( HepMC3::ConstGenParticlePtr, HepMC3::ConstGenParticlePtr )> child_converter =
            [&]( HepMC3::ConstGenParticlePtr part, HepMC3::ConstGenParticlePtr parent ) {
              LinkedParticle* plinked{nullptr};
              LinkedParticle* clinked{nullptr};
              if ( parent ) {
                // The parent should always have a linked particle if provided
                plinked = table[parent->id()];
              }
              if ( part && table.find( part->id() ) != std::end( table ) ) {
                // HepMC child might not have a matching linked particle if it is not supposed to be converted
                clinked = table[part->id()];

                clinked->m_tracker = this;
              }
              if ( clinked && plinked ) {
                if ( !Gaussino::LinkedParticleHelpers::VerifyLink( plinked, clinked ) ) {
                  throw std::runtime_error( "LinkedParticles to be linked have mismatching vertex positions" );
                }
                clinked->AddParent( plinked );
                plinked->AddChild( clinked );
              }
              // If a clinked particle was found, the current particle becomes the parent for its children,
              // otherwise this HepMC particle was not supposed to be converted and is skipped.
              HepMC3::ConstGenParticlePtr new_parent = clinked ? part : parent;
              // Check if the particle has oscillated
              auto oscillated = HepMCUtils::hasOscillated( part );
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
                  m_linkedParticles.erase( oscillated_lp );
                  delete oscillated_lp;
                }
                part = oscillated;
              }
              for ( auto& child : part->children() ) {
                child_converter( child, new_parent );
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
  } // namespace Gaussino
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
        if ( Gaussino::LinkedParticleHelpers::essentiallyEqual( vtx->GetPosition().x(),
                                                                rp->HepMC()->production_vertex()->position().x() ) &&
             Gaussino::LinkedParticleHelpers::essentiallyEqual( vtx->GetPosition().y(),
                                                                rp->HepMC()->production_vertex()->position().y() ) &&
             Gaussino::LinkedParticleHelpers::essentiallyEqual( vtx->GetPosition().z(),
                                                                rp->HepMC()->production_vertex()->position().z() ) &&
             Gaussino::LinkedParticleHelpers::essentiallyEqual( vtx->GetT0(),
                                                                rp->HepMC()->production_vertex()->position().t() ) ) {
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

  bool MCTruthTracker::AlreadyRegisteredPrimary( unsigned int primaryID ) const
  {
    auto primary = m_primary_to_linked.find(primaryID);
    if(primary == std::end(m_primary_to_linked)){
      throw GaudiException{"PrimaryID does not exist", "MCTruth", StatusCode::FAILURE};
    }
    return primary->second->G4Truth();
  }

  void MCTruthTracker::RegisterPrimary( Gaussino::G4TruthParticle* particle, unsigned int primaryID )
  {
    auto primaryLP = m_primary_to_linked[primaryID];
    if ( primaryLP->HepMC()->pdg_id() != particle->GetPdgID() ) {
      throw GaudiException{"G4Truth PDGid does not match", "MCTruth", StatusCode::FAILURE};
    }
    if ( primaryLP->G4Truth() ) {
      std::stringstream sstr;
      sstr << "Linked particle already has G4Truth: \n" << *primaryLP;
      throw GaudiException{sstr.str(), "MCTruth", StatusCode::FAILURE};
    }
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
    auto ptr      = new LinkedParticle{m_pcounter++, particle};
    ptr->SetType( ConversionType::FROMG4 );
    m_tracking_to_linked[particle->GetTrackID()] = ptr;

    // Link the particles
    parentLP->AddChild( ptr );
    ptr->AddParent( parentLP );
    // Need to move it into the storage at the end so ptr remains valid for setting the links
    m_linkedParticles.insert( ptr );
  }

  void MCTruthTracker::DeclareEnd(const HepMC3::FourVector& position, int procid, int trackID ){
    if ( m_tracking_to_linked.find( trackID ) == std::end( m_tracking_to_linked ) ) {
      G4cerr << __PRETTY_FUNCTION__ << " no linked particle with trackID found. Skipping!" << G4endl;
      return;
    }
    auto parentLP = m_tracking_to_linked[trackID];
    parentLP->AddEndVertex(position, procid);
  }

  void MCTruthTracker::BufferEnd(const HepMC3::FourVector& position, int procid, int trackID ){
    if ( m_tracking_to_linked.find( trackID ) == std::end( m_tracking_to_linked ) ) {
      G4cerr << __PRETTY_FUNCTION__ << " no linked particle with trackID found. Skipping!" << G4endl;
      return;
    }
    m_buffered_endvertices.emplace( std::piecewise_construct,
              std::forward_as_tuple(trackID),
              std::forward_as_tuple(position, procid ));
  }

  ///////////////////////////////////////////////////////////
  // MCTruth
  //////////////////////////////////////////////////////////

  MCTruth::MCTruth( MCTruthTracker&& tracker ) : MCTruthData{std::move( tracker )}
  {
    DoCleanup();
    std::function<void( LinkedParticle* )> signal_flagging = [&]( LinkedParticle* lp ) {
      lp->m_isSignal = true;
      for ( auto child : lp->GetChildren() ) {
        signal_flagging( child );
      }
    };
    for ( auto lp : m_linkedParticles ) {
      if ( lp->m_isSignal ) {
        signal_flagging( lp );
      }
    }
  }

  void MCTruth::DoCleanup()
  {
    // First, loop over all particles and add the buffered endvertices if none are present
    // at the correct location
    for(auto & [trackID, data]: m_buffered_endvertices){
      auto position = data.first;
      auto type = data.second;
      auto & lp = m_tracking_to_linked[trackID];
      auto found = std::any_of(std::begin(lp->GetEndVtxs()), std::end(lp->GetEndVtxs()), [=] (auto & vertex){
          return Gaussino::LinkedParticleHelpers::CompareFourVector( vertex->GetPosition(), position );
          });
      if(!found){
        lp->AddEndVertex(position, type);
      }
    }
    m_buffered_endvertices.clear();

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

  void MCTruthData::EraseLinkedParticle( LinkedParticle* lp )
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
    if ( prodvtx && prodvtx->outgoing_particles.size() == 0 ) {

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

  LinkedParticle::PtrSet MCTruth::GetRootParticlesIncludingSlaves() const {
    LinkedParticle::PtrSet tmp_ret = m_root_particles;
    for(auto & slave : m_slave_mctruths){
      for(auto & srp : slave->GetRootParticles()){
        tmp_ret.insert(srp);
      }
    }
    return tmp_ret; }
  void MCTruthData::EraseDecayTree( LinkedParticle* lp )
  {
    for ( auto child : lp->GetChildren() ) {
      EraseDecayTree( child );
    }
    EraseLinkedParticle( lp );
  }
} // namespace Gaussino

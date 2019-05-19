#pragma once
#include <functional>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMCUtils/PrintDecayTree.h"

#include "GiGaMTCore/Truth/Common.h"
#include "GiGaMTCore/Truth/LinkedParticle.h"

#include "Geant4/G4Event.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4PrimaryVertex.hh"

// MCTruthConverter objects build on top of each other. To prevent incorrect use, this object evolves in stages, that
// each trigger internal transformations of the event structure.

namespace Gaussino
{

  class MCTruthData
  {
  public:
    MCTruthData( const MCTruthData& ) = delete;
    virtual ~MCTruthData();
    template <typename STREAM>
    STREAM& DumpToStream( STREAM&,
                          std::function<std::string( int )> pdg_to_name = []( int i ) { return std::to_string( i ); } );
    size_t GetNParticles() const;
    size_t GetNVertices() const;

  protected:
    MCTruthData() = default;
    MCTruthData( MCTruthData&& right ) noexcept;
    // Owning container of the linked particle objects
    std::set<LinkedParticle*> m_linkedParticles;
    // Some helpful maps to organise the data
    // Map GenParticle ID to LinkedParticle, hidden in a
    // map for to separate for every GenEvent
    std::unordered_map<const HepMC3::GenEvent*, std::unordered_map<int, LinkedParticle*>> m_hepmc_to_linked;
    // Map G4 barcode to LinkedParticle
    std::unordered_map<unsigned int, LinkedParticle*> m_primary_to_linked;
    // Map G4TruthParticles (i.e. make during tracking) to LinkedParticle
    std::unordered_map<int, LinkedParticle*> m_tracking_to_linked;
    std::set<LinkedParticle*> m_root_particles;

    // Some consistence checking internal variables
    G4Event* m_geant4_event{nullptr};
    std::set<G4PrimaryVertex*> m_geant4_vertex;
  };

  // Class to register HepMC particles with their conversion type.
  class MCTruthConverter : public MCTruthData
  {
  public:
    MCTruthConverter() : MCTruthData(){};
    MCTruthConverter( const MCTruthConverter& ) = delete;
    MCTruthConverter( MCTruthConverter&& right ) noexcept : MCTruthData( std::move( right ) ){};
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    void Declare( const HepMC3::ConstGenParticlePtr& particle, ConversionType type );
    void AddConverter( MCTruthConverter&& conv );
  };

  // Main class used during the Geant4 tracking.
  // Takes an r-value MCTruthConverter and links the particles in its internal storage.
  // No more HepMC particles can be added at this point to ensure the internal structure remains
  // consistent. Optionally can fill a provided G4Event (and becomes linked to this event via GaussinoEventInformation)
  class MCTruthTracker : public MCTruthData
  {
  public:
    MCTruthTracker()                        = delete;
    MCTruthTracker( const MCTruthTracker& ) = delete;
    MCTruthTracker( MCTruthTracker&& right ) noexcept : MCTruthData( std::move( right ) ){};
    // Constructs the linkage between the MCTruthConverter contents fills the vertices and primary particles
    // into the G4Event (optional in case of generator only MC). If no Geant4 event is passed, any previously
    // set ConversionsType flags will be overwritten to ConversionType::MC before proceeding.
    MCTruthTracker( MCTruthConverter&& right, G4Event* event = nullptr );
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    void Declare( Gaussino::G4TruthParticle* particle, int parentID );
    void RegisterPrimary( Gaussino::G4TruthParticle* particle, unsigned int primaryID );

  private:
    // Intended to be called after all particles from the HepMC event have been
    // declared.
    void DoInitialLinking();

    void AddToG4Event( G4Event* );
  };

  // Final class, takes an MCTruthTracker and cleans up the internally stored particles (e.g. particle reacted with
  // something in the detector before it could perform it's assigned decay, and thus we need to remove these decay
  // products). Only object that allows access to the internally sorted LinkedParticles so users can convert them to
  // their event model.
  class MCTruth : public MCTruthData
  {
  public:
    MCTruth()                 = delete;
    MCTruth( const MCTruth& ) = delete;
    MCTruth( MCTruth&& right ) noexcept : MCTruthData( std::move( right ) ){};
    // Constructs the linkage between the MCTruthConverter contents fills the vertices and primary particles
    // into the G4Event (optional in case of generator only MC). If no Geant4 event is passed, any previously
    // set ConversionsType flags will be overwritten to ConversionType::MC before proceeding.
    MCTruth( MCTruthTracker&& right );
    std::set<LinkedParticle*> GetRootParticles() const { return m_root_particles; }
    const LinkedParticle* GetParticleFromTrackID(int trackid) const;

  private:
    void DoCleanup();
    void EraseLinkedParticle( LinkedParticle* lp );
    void EraseDecayTree( LinkedParticle* lp );
  };

  typedef std::unique_ptr<MCTruthConverter> MCTruthConverterPtr;
  typedef std::vector<MCTruthConverterPtr> MCTruthConverterPtrs;
  typedef std::unique_ptr<MCTruthTracker> MCTruthTrackerPtr;
  typedef std::vector<MCTruthTrackerPtr> MCTruthTrackerPtrs;
  typedef std::unique_ptr<MCTruth> MCTruthPtr;
  typedef std::vector<MCTruthPtr> MCTruthPtrs;
  // Helper function to merge containers of MCTruthConverterPtr into a single converter
  // Useful when splitting/assigning the work to to Geant4 workers
  // Will return a new converter with all input converters become invalid
  template <typename Iter>
  MCTruthConverterPtr MergeConverters( Iter it, Iter end )
  {
    MCTruthConverterPtr ret = std::make_unique<MCTruthConverter>();
    while ( it != end ) {
      ret->AddConverter( std::move( **it ) );
      it++;
    }
    return ret;
  }
}

template <typename STREAM>
STREAM& Gaussino::MCTruthData::DumpToStream( STREAM& out, std::function<std::string( int )> pdg_to_name )
{
  const std::string spacer = "|---";
  out << "#############################################\n";
  out << "# Beginning dump of converter\n";
  out << "#############################################\n";

  std::set<LinkedParticle*> visited;
  unsigned int i_root = 1;
  for ( auto& rp : m_root_particles ) {
    out << "-------- Beginning root particle " << i_root << " --------\n";
    std::function<void( LinkedParticle*, std::string )> rec_print = [&]( LinkedParticle* lp, std::string spacing ) {
      out << spacing << " " << pdg_to_name( lp->GetPDG() ) << *lp << "\n";
      if ( visited.count( lp ) > 0 ) {
        out << spacing << " *** Already printed this tree\n";
      } else {
        visited.insert( lp );
        for ( auto& dp : lp->GetChildren() ) {
          rec_print( dp, spacing + spacer );
        }
      }
    };
    rec_print( rp, "" );
    i_root++;
  }

  out << "Visited " << visited.size() << " out of " << m_linkedParticles.size() << " LinkedParticle\n";
  out << "#############################################\n";
  out << "# Finished dump of converter\n";
  out << "#############################################\n";
  return out;
}

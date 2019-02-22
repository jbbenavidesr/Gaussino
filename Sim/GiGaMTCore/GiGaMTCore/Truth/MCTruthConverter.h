#pragma once
#include <functional>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMCUtils/PrintDecayTree.h"

#include "GiGaMTCore/Truth/Common.h"
#include "GiGaMTCore/Truth/LinkedParticle.h"

#include "Geant4/G4Event.hh"

// MCTruthConverter objects build on top of each other. To prevent incorrect use, this object evolves in stages, that
// each trigger internal transformations of the event structure.

namespace Gaussino
{

  class MCTruthBase
  {
  public:
    MCTruthBase( const MCTruthBase& ) = delete;
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    // void Declare(HepMC::GenParticlePtr & particle, ConversionType type);
    //// Intended to be called after all particles from the HepMC event have been
    //// declared.

  protected:
    MCTruthBase() = default;
    MCTruthBase( MCTruthBase&& right ) noexcept;
    // Owning container of the linked particle objects
    std::vector<std::unique_ptr<LinkedParticle>> m_linkedParticles;
    // Some helpful maps to organise the data
    // Map GenParticle ID to LinkedParticle
    std::unordered_map<int, LinkedParticle*> m_hepmc_to_linked;
    // Map G4 barcode to LinkedParticle
    std::unordered_map<int, LinkedParticle*> m_geant4_to_linked;
    // Map G4TruthParticles (i.e. make during tracking) to LinkedParticle
    std::unordered_map<int, LinkedParticle*> m_tracking_to_linked;
    std::set<LinkedParticle*> m_root_particles;

    // Some consistence checking internal variables
    HepMC::GenEvent* m_hepmc_event{nullptr};
    G4Event* m_geant4_event{nullptr};
  };

  class MCTruthConverter : protected MCTruthBase
  {
  public:
    MCTruthConverter() : MCTruthBase(){};
    MCTruthConverter( const MCTruthConverter& ) = delete;
    MCTruthConverter( MCTruthConverter&& right ) noexcept : MCTruthBase( std::move( right ) ){};
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    void Declare( const HepMC::GenParticlePtr& particle, ConversionType type );
  };

  class MCTruthTracker : protected MCTruthConverter
  {
  public:
    MCTruthTracker( const MCTruthTracker& ) = delete;
    MCTruthTracker( MCTruthTracker&& right ) noexcept : MCTruthConverter( std::move( right ) ){};
    // Constructs the linkage between the MCTruthConverter contents fills the vertices and primary particles
    // into the G4Event (optional in case of generator only MC). If no Geant4 event is passed, any previously
    // set ConversionsType flags will be overwritten to ConversionType::MC before proceeding.
    MCTruthTracker( MCTruthConverter&& right, G4Event* event = nullptr );
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    void Declare( [[maybe_unused]] Gaussino::G4TruthParticle particle ){};
    std::set<LinkedParticle*> GetRootParticles() const { return m_root_particles; }
    template <typename STREAM>
    STREAM & DumpToStream( STREAM&,
                       std::function<std::string( int )> pdg_to_name = []( int i ) { return std::to_string( i ); } );

  private:
    // Intended to be called after all particles from the HepMC event have been
    // declared.
    void DoInitialLinking();

    void AddToG4Event( G4Event* );
  };

  typedef std::shared_ptr<MCTruthConverter> MCTruthConverterPtr;
  typedef std::vector<MCTruthConverterPtr> MCTruthConverterPtrs;
  typedef std::shared_ptr<MCTruthTracker> MCTruthTrackerPtr;
  typedef std::vector<MCTruthTrackerPtr> MCTruthTrackerPtrs;
}

template <typename STREAM>
STREAM& Gaussino::MCTruthTracker::DumpToStream( STREAM& out, std::function<std::string( int )> pdg_to_name )
{
  const std::string spacer = "|---";
  out << "#############################################\n";
  out << "# Beginning dump of converter\n";
  out << "#############################################\n";

  unsigned int i_root = 1;
  for ( auto& rp : m_root_particles ) {
    out << "-------- Beginning root particle " << i_root << " --------\n";
    std::function<void( LinkedParticle*, std::string )> rec_print = [&]( LinkedParticle* lp, std::string spacing ) {
      out << spacing << " " << pdg_to_name( lp->GetPDG() ) << *lp << "\n";
      for ( auto& dp : lp->GetChildren() ) {
        rec_print( dp, spacing + spacer );
      }
    };
    rec_print( rp, "" );
    i_root++;
  }

  out << "#############################################\n";
  out << "# Finished dump of converter\n";
  out << "#############################################\n";
  return out;
}

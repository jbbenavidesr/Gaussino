#pragma once
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "GiGaMTTruth/Common.h"
#include "GiGaMTTruth/LinkedParticle.h"

#include "Geant4/G4Event.hh"

namespace Gaussino
{
  class ConverterInfo;
}

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
    MCTruthBase();
    MCTruthBase( MCTruthBase&& right );
    // Owning container of ConverterInfo objects.
    std::vector<std::unique_ptr<ConverterInfo>> m_convInfos;
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
    MCTruthConverter( MCTruthConverter&& right ) : MCTruthBase( std::move( right ) ){};
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    void Declare( HepMC::GenParticlePtr& particle, ConversionType type );
  };

  class MCTruthTracker : protected MCTruthConverter
  {
  public:
    MCTruthTracker( const MCTruthTracker& ) = delete;
    MCTruthTracker( MCTruthTracker&& right ) : MCTruthConverter( std::move( right ) ){};
    // Constructs the linkage between the MCTruthConverter contents fills the vertices and primary particles
    // into the G4Event (optional in case of generator only MC)
    MCTruthTracker( MCTruthConverter&& right, G4Event* event=nullptr);
    // Declare the particle and its intended conversion type. This will register the necessary
    // information in the internal storage elements.
    void Declare( Gaussino::G4TruthParticle particle );
    std::set<LinkedParticle*> GetRootParticles() const {return m_root_particles;}

    private:
    // Intended to be called after all particles from the HepMC event have been
    // declared.
    void DoInitialLinking();

    void AddToG4Event( G4Event* );
  };
}

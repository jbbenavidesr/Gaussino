#pragma once
#include <set>

#include "Geant4/G4PrimaryParticle.hh"
#include "GiGaMTCore/Truth/Common.h"
#include "GiGaMTCore/Truth/G4TruthParticle.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include <ostream>

namespace Gaussino
{
  class MCTruthConverter;
  class MCTruthTracker;
}

class LinkedVertex;

class LinkedParticle
{

public:
  friend Gaussino::MCTruthConverter;
  friend Gaussino::MCTruthTracker;
  friend std::ostream& operator<<( std::ostream&, const LinkedParticle& );
  // Stupid const_cast to get access to the non-constant only cast method of the HepMC::SmartPointer.
  // Doesn't matter here as the raw pointer is then internally stored as a ptr to const again.
  LinkedParticle( const HepMC::GenParticlePtr& part ) : m_hepmc( const_cast<HepMC::GenParticlePtr&>( part ) ) {}
  LinkedParticle( const HepMC::GenParticlePtr& part, G4PrimaryParticle* g4part )
      : m_hepmc( const_cast<HepMC::GenParticlePtr&>( part ) )
  {
    m_primary = g4part;
  }
  LinkedParticle( G4PrimaryParticle* g4part ) { m_primary = g4part; }
  LinkedParticle( Gaussino::G4TruthParticle* g4truth ) { m_tracking = g4truth; }
  LinkedParticle() = delete;
  virtual ~LinkedParticle()
  {
    if ( m_tracking ) delete m_tracking;
  };
  const HepMC::GenParticle* HepMC() { return m_hepmc; }
  G4PrimaryParticle*& G4Primary() { return m_primary; }
  Gaussino::G4TruthParticle*& G4Truth() { return m_tracking; }
  Gaussino::ConversionType GetType() const { return m_conversion_type; }
  void SetType( Gaussino::ConversionType type ) { m_conversion_type = type; };
  // Two convenient functions, both link bidirectional
  void AddParent( LinkedParticle* part );
  void AddChild( LinkedParticle* part );
  std::set<LinkedParticle*> GetParents();
  std::set<LinkedParticle*> GetChildren();

  std::shared_ptr<LinkedVertex>& GetProdVtx() { return m_prodvtx; }
  std::set<std::shared_ptr<LinkedVertex>>& GetEndVtxs() { return m_endvtxs; }

  // A couple of accessor functions to simplify access to common properties.
  // Will extract the result from the stored particles in the following order:
  // 1. HepMC
  // 2. G4Primary
  // 3. G4Truth from tracking
  int GetPDG() const;
  HepMC::FourVector GetMomentum() const;
  HepMC::FourVector GetOriginPosition() const;
  HepMC::FourVector GetEndPosition() const;
  // Calculates the decay time of the particle based on the internal HepMC
  // particle. Returns -1 if particle is stable and is not decayed in the HepMC
  // record
  double GetDecayTimeHepMC() const;
  Gaussino::MCTruthTracker* GetTracker() { return m_tracker; }

private:
  // Two vectors to store the relationships, extracted from whatever source we can find.
  // Vertex positions are taken from any of the contained particle in some smart order
  // I haven't decided on yet
  const HepMC::GenParticle* m_hepmc{nullptr};
  G4PrimaryParticle* m_primary{nullptr};
  Gaussino::G4TruthParticle* m_tracking{nullptr};
  Gaussino::ConversionType m_conversion_type;
  Gaussino::MCTruthTracker* m_tracker{nullptr};
  bool m_isSignal{false};
  bool m_hasOscillated{false};
  std::shared_ptr<LinkedVertex> m_prodvtx;
  std::set<std::shared_ptr<LinkedVertex>> m_endvtxs;
};

// Small helper class to facilitate a cleaner linking between the LinkedParticles.
// This is a purely logical vertex, its real position etc is obtained from the respective linked particles
// and their various representations
class LinkedVertex
{
public:
  LinkedVertex() = default;
  std::set<LinkedParticle*> incoming_particle;
  std::set<LinkedParticle*> outgoing_particles;
  HepMC::FourVector GetPosition() const
  {
    if ( outgoing_particles.size() > 0 ) {
      return ( *std::begin( outgoing_particles ) )->GetEndPosition();
    }
    if ( incoming_particle.size() > 0 ) {
      return ( *std::begin( incoming_particle ) )->GetEndPosition();
    }

    throw std::runtime_error(
        "Trying to access position of vertex without associated particles" );
  }
  const HepMC::GenVertex* hepmc_vtx{nullptr};
};

std::ostream& operator<<( std::ostream&, const LinkedParticle& );

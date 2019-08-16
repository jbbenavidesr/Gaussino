#pragma once
#include <set>

#include "Geant4/G4PrimaryParticle.hh"
#include "GiGaMTCore/Truth/Common.h"
#include "GiGaMTCore/Truth/G4TruthParticle.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include <ostream>

namespace Gaussino
{
  class MCTruth;
  class MCTruthConverter;
  class MCTruthTracker;
  class MCTruthData;
}

class LinkedVertex;

template <typename PartPtr>
struct id_comparer {
  bool operator()( const PartPtr& left, const PartPtr& right ) const { return left->GetID() < right->GetID(); }
};

class LinkedParticle
{

public:
  typedef std::set<LinkedParticle*, id_comparer<LinkedParticle*>> PtrSet;
  typedef std::set<std::shared_ptr<LinkedParticle>, id_comparer<std::shared_ptr<LinkedParticle>>> SharedPtrSet;
  typedef std::set<std::shared_ptr<LinkedVertex>, id_comparer<std::shared_ptr<LinkedVertex>>> VertexSharedPtrSet;

public:
  friend Gaussino::MCTruthConverter;
  friend Gaussino::MCTruthTracker;
  friend Gaussino::MCTruth;
  friend std::ostream& operator<<( std::ostream&, const LinkedParticle& );

private:
  LinkedParticle( unsigned int id, const HepMC3::ConstGenParticlePtr& part ) : m_hepmc( part ), m_id( id ) {}
  LinkedParticle( unsigned int id, const HepMC3::ConstGenParticlePtr& part, G4PrimaryParticle* g4part )
      : m_hepmc( part ), m_id( id )
  {
    m_primary = g4part;
  }
  LinkedParticle( unsigned int id, G4PrimaryParticle* g4part ) : m_id( id ) { m_primary = g4part; }
  LinkedParticle( unsigned int id, Gaussino::G4TruthParticle* g4truth ) : m_id( id ) { m_tracking = g4truth; }
  LinkedParticle() = delete;

  // Allows to update the ID.
  // Used to move the IDs when merging converters to avoid conflicts
  void SetID(unsigned int id) { m_id=id;};
public:
  virtual ~LinkedParticle();
  G4PrimaryParticle*& G4Primary() { return m_primary; }
  Gaussino::G4TruthParticle*& G4Truth() { return m_tracking; }
  HepMC3::ConstGenParticlePtr HepMC() const { return m_hepmc; }
  const G4PrimaryParticle* G4Primary() const { return m_primary; }
  const Gaussino::G4TruthParticle* G4Truth() const { return m_tracking; }
  Gaussino::ConversionType GetType() const { return m_conversion_type; }
  void SetType( Gaussino::ConversionType type ) { m_conversion_type = type; };
  // Two convenient functions, both link bidirectional
  void AddParent( LinkedParticle* part );
  void AddChild( LinkedParticle* part );
  PtrSet GetParents() const;
  PtrSet GetChildren() const;

  std::shared_ptr<LinkedVertex>& GetProdVtx() { return m_prodvtx; }
  VertexSharedPtrSet& GetEndVtxs() { return m_endvtxs; }

  // A couple of accessor functions to simplify access to common properties.
  // Will extract the result from the stored particles in the following order:
  // 1. HepMC
  // 2. G4Primary
  // 3. G4Truth from tracking
  int GetPDG() const;
  bool HasOscillated() const { return m_hasOscillated; }
  HepMC3::FourVector GetMomentum() const;
  HepMC3::FourVector GetOriginPosition() const;
  HepMC3::FourVector GetEndPosition() const;
  int GetCreatorID() const;
  // Calculates the decay time of the particle based on the internal HepMC
  // particle. Returns -1 if particle is stable and is not decayed in the HepMC
  // record
  double GetDecayTimeHepMC() const;
  Gaussino::MCTruthTracker* GetTracker() { return m_tracker; }
  // Helper function to return an ID (for sorting the ptr sets)
  unsigned int GetID() const { return m_id; };

private:
  // Two vectors to store the relationships, extracted from whatever source we can find.
  // Vertex positions are taken from any of the contained particle in some smart order
  // I haven't decided on yet
  HepMC3::ConstGenParticlePtr m_hepmc{nullptr};
  G4PrimaryParticle* m_primary{nullptr};
  Gaussino::G4TruthParticle* m_tracking{nullptr};
  Gaussino::ConversionType m_conversion_type;
  Gaussino::MCTruthTracker* m_tracker{nullptr};
  bool m_isSignal{false};
  bool m_corrected_momentum{false};
  bool m_hasOscillated{false};
  std::shared_ptr<LinkedVertex> m_prodvtx{nullptr};
  VertexSharedPtrSet m_endvtxs;
  unsigned int m_id;
};

// Small helper class to facilitate a cleaner linking between the LinkedParticles.
// This is a purely logical vertex, its real position etc is obtained from the respective linked particles
// and their various representations
class LinkedVertex
{
public:
  LinkedVertex() = delete;
  LinkedVertex( int id ) : m_id( id ) {}
  LinkedParticle::PtrSet incoming_particle;
  LinkedParticle::PtrSet outgoing_particles;
  unsigned int m_id;
  unsigned int GetID() const { return m_id; }
  int GetProcessID() const
  {
    if ( outgoing_particles.size() > 0 ) {
      return ( *std::begin( outgoing_particles ) )->GetCreatorID();
    }
    return -1;
  }
  HepMC3::FourVector GetPosition() const
  {
    // FIXME: Prioritize the location G4 simulated particles
    if ( outgoing_particles.size() > 0 ) {
      return ( *std::begin( outgoing_particles ) )->GetOriginPosition();
    }
    if ( incoming_particle.size() > 0 ) {
      return ( *std::begin( incoming_particle ) )->GetEndPosition();
    }

    throw std::runtime_error( "Trying to access position of vertex without associated particles" );
  }
  const HepMC3::GenVertex* hepmc_vtx{nullptr};
};

std::ostream& operator<<( std::ostream&, const LinkedParticle& );

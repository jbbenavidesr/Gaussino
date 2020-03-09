#pragma once
#include <set>

#include "Geant4/G4PrimaryParticle.hh"
#include "GiGaMTCoreTruth/Common.h"
#include "GiGaMTCoreTruth/G4TruthParticle.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMCUser/TemplateAttribute.h"
#include <ostream>

namespace Gaussino
{
  class MCTruth;
  class MCTruthConverter;
  class MCTruthTracker;
  class MCTruthData;
}

class LinkedVertex;
class LinkedParticle;
typedef HepMC3::TAttribute<LinkedParticle*> LinkedParticleAttribute;

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
// and their various representations. 
// It is however possible to attach a MCTruth instance to a LinkedVertex. This instance is then treated as
// outgoing from the vertex. It is stored as a shared pointer and the LinkedVertex is ultimatly responsible
// for deleting the structure. This structure is independent of the MCTruth object this vertex itself is stored
// in.
// This is not used during normal processing but can be employed in special cases,
// e.g. where parts of the event are simulated separately to optimise the processing (first simulate the signal decay and search for specific simulation outcome before continuing) or when parts of the event are going to be reused. This has to be treated explicitly during the EDM conversion (see the respective algorithms).
class LinkedVertex
{
public:
  LinkedVertex() = delete;
  LinkedVertex( int id ) : m_id( id ) {}
  LinkedParticle::PtrSet incoming_particle;
  LinkedParticle::PtrSet outgoing_particles;
  std::set<std::shared_ptr<Gaussino::MCTruth>> outgoing_mctruths;
  unsigned int m_id;
  unsigned int GetID() const { return m_id; }
  int GetProcessID() const;
  HepMC3::FourVector GetPosition() const;
  bool HasOutgoingMCTruth(){return outgoing_mctruths.size()>0;}
  const HepMC3::GenVertex* hepmc_vtx{nullptr};
};

std::ostream& operator<<( std::ostream&, const LinkedParticle& );

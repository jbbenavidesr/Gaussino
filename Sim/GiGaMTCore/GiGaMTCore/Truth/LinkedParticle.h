#pragma once
#include <set>

#include "Geant4/G4PrimaryParticle.hh"
#include "GiGaMTCore/Truth/Common.h"
#include "GiGaMTCore/Truth/G4TruthParticle.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include <ostream>

namespace Gaussino {
  class MCTruthConverter;
  class MCTruthTracker;
}

class LinkedParticle
{

public:
  friend Gaussino::MCTruthConverter;
  friend Gaussino::MCTruthTracker;
  friend std::ostream& operator<<( std::ostream&, const LinkedParticle& );
  // Stupid const_cast to get access to the non-constant only cast method of the HepMC::SmartPointer.
  // Doesn't matter here as the raw pointer is then internally stored as a ptr to const again.
  LinkedParticle( const HepMC::GenParticlePtr& part ):m_hepmc(const_cast<HepMC::GenParticlePtr&>(part)) {  }
  LinkedParticle( const HepMC::GenParticlePtr& part, G4PrimaryParticle* g4part ):m_hepmc(const_cast<HepMC::GenParticlePtr&>(part))
  {
    m_primary = g4part;
  }
  LinkedParticle( G4PrimaryParticle* g4part ) { m_primary = g4part; }
  LinkedParticle( Gaussino::G4TruthParticle* g4truth) { m_tracking = g4truth; }
  LinkedParticle() = delete;
  virtual ~LinkedParticle(){if(m_tracking) delete m_tracking;};
  const HepMC::GenParticle* HepMC() { return m_hepmc; }
  G4PrimaryParticle*& G4Primary() { return m_primary; }
  Gaussino::G4TruthParticle*& G4Truth() { return m_tracking; }
  Gaussino::ConversionType GetType() const { return m_conversion_type;}
  void SetType( Gaussino::ConversionType type ) { m_conversion_type = type; };
  void AddParent(LinkedParticle* part){m_parents.insert(part);}
  void AddChild(LinkedParticle* part){m_children.insert(part);}
  std::set<LinkedParticle*>& GetParents() { return m_parents;}
  std::set<LinkedParticle*>& GetChildren() { return m_children;}

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
  Gaussino::MCTruthTracker* GetTracker(){return m_tracker;}

private:
  // Two vectors to store the relationships, extracted from whatever source we can find.
  // Vertex positions are taken from any of the contained particle in some smart order
  // I haven't decided on yet
  std::set<LinkedParticle*> m_parents;
  std::set<LinkedParticle*> m_children;
  const HepMC::GenParticle* m_hepmc{nullptr};
  G4PrimaryParticle* m_primary{nullptr};
  Gaussino::G4TruthParticle* m_tracking{nullptr};
  Gaussino::ConversionType m_conversion_type;
  Gaussino::MCTruthTracker* m_tracker{nullptr};
};

std::ostream & operator<<(std::ostream &, const LinkedParticle&);

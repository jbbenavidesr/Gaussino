#pragma once
#include <vector>

#include "Geant4/G4PrimaryParticle.hh"
#include "GiGaMTCore/Truth/Common.h"
#include "GiGaMTCore/Truth/G4TruthParticle.h"
#include "HepMC/GenParticle.h"
#include <ostream>

class LinkedParticle
{

public:
  friend class MCTruthConverter;
  friend class MCTruthTracker;
  friend std::ostream& operator<<( std::ostream&, const LinkedParticle& );
  LinkedParticle( const HepMC::GenParticlePtr& part ):m_hepmc(part) {  }
  LinkedParticle( const HepMC::GenParticlePtr& part, G4PrimaryParticle* g4part ):m_hepmc(part)
  {
    m_primary = g4part;
  }
  LinkedParticle( G4PrimaryParticle* g4part ) { m_primary = g4part; }
  LinkedParticle() = delete;
  virtual ~LinkedParticle(){};
  const HepMC::GenParticlePtr& HepMC() { return m_hepmc; }
  G4PrimaryParticle*& G4Primary() { return m_primary; }
  Gaussino::G4TruthParticle*& G4Truth() { return m_tracking; }
  Gaussino::ConversionType GetType() const { return m_conversion_type;}
  void SetType( Gaussino::ConversionType type ) { m_conversion_type = type; };
  void AddParent(LinkedParticle* part){m_parents.push_back(part);}
  void AddChild(LinkedParticle* part){m_children.push_back(part);}
  std::vector<LinkedParticle*> GetParents() const { return m_parents;}
  std::vector<LinkedParticle*> GetChildren() const { return m_children;}

  // A couple of accessor functions to simplify access to common properties.
  // Will extract the result from the stored particles in the following order:
  // 1. HepMC
  // 2. G4Primary
  // 3. G4Truth from tracking
  int GetPDG() const;
  HepMC::FourVector GetMomentum() const;

private:
  // Two vectors to store the relationships, extracted from whatever source we can find.
  // Vertex positions are taken from any of the contained particle in some smart order
  // I haven't decided on yet
  std::vector<LinkedParticle*> m_parents;
  std::vector<LinkedParticle*> m_children;
  const HepMC::GenParticlePtr m_hepmc{nullptr};
  G4PrimaryParticle* m_primary{nullptr};
  Gaussino::G4TruthParticle* m_tracking{nullptr};
  Gaussino::ConversionType m_conversion_type{Gaussino::ConversionType::NONE};
};

std::ostream & operator<<(std::ostream &, const LinkedParticle&);

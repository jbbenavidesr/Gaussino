#pragma once
#include <vector>

#include "Geant4/G4PrimaryParticle.hh"
#include "GiGaMTTruth/Common.h"
#include "GiGaMTTruth/G4TruthParticle.h"
#include "HepMC/GenParticle.h"

class LinkedParticle
{

public:
  friend class MCTruthConverter;
  LinkedParticle( HepMC::GenParticlePtr& part ) { m_hepmc = part; }
  LinkedParticle( HepMC::GenParticlePtr& part, G4PrimaryParticle* g4part )
  {
    m_hepmc   = part;
    m_primary = g4part;
  }
  LinkedParticle( G4PrimaryParticle* g4part ) { m_primary = g4part; }
  LinkedParticle() = delete;
  virtual ~LinkedParticle();
  HepMC::GenParticlePtr& HepMC() { return m_hepmc; }
  G4PrimaryParticle*& G4Primary() { return m_primary; }
  Gaussino::G4TruthParticle*& G4Truth() { return m_tracking; }
  Gaussino::ConversionType GetType();
  void SetType( Gaussino::ConversionType type ) { m_conversion_type = type; };
  void AddParent(LinkedParticle* part){m_parents.push_back(part);}
  void AddChild(LinkedParticle* part){m_children.push_back(part);}

private:
  // Two vectors to store the relationships, extracted from whatever source we can find.
  // Vertex positions are taken from any of the contained particle in some smart order
  // I haven't decided on yet
  std::vector<LinkedParticle*> m_parents;
  std::vector<LinkedParticle*> m_children;
  HepMC::GenParticlePtr m_hepmc{nullptr};
  G4PrimaryParticle* m_primary{nullptr};
  Gaussino::G4TruthParticle* m_tracking{nullptr};
  Gaussino::ConversionType m_conversion_type{Gaussino::ConversionType::NONE};
};

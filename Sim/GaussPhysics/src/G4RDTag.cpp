// local
#include "G4RDTag.h"

#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                          RDTag                                 ###
// ######################################################################
G4RDTag* G4RDTag::theInstance = nullptr;

G4RDTag* G4RDTag::Definition() {
  if (theInstance != 0) return theInstance;

  const G4String name = "rdtag";
  // search in particle table]
  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* anInstance = pTable->FindParticle(name);
  if (anInstance == 0) {
    // create particle
    anInstance = new G4ParticleDefinition(name,       // the name
                                          0.0 * MeV,  // the mass
                                          0.0 * MeV,  // the width
                                          0.0,        // the charge
                                          0,          // the spin
                                          0,          // the parity
                                          0,          // the conjugation
                                          0,          // the isospin
                                          0,  // the z-projection of isospin
                                          0,  // the G-parity
                                          "rdtag",  // p-type
                                          0,        // lepton
                                          0,        // baryon
                                          424242,   // PDG encoding
                                          true,     // stable
                                          0.0,      // lifetime
                                          nullptr,  // decay table
                                          false,    // shortlived
                                          "rdtag",  // subtype
                                          424242    // antiparticle
                                          );
    pTable->Insert(anInstance);
  }
  theInstance = reinterpret_cast<G4RDTag*>(anInstance);
  return theInstance;
}

G4RDTag* G4RDTag::RDTagDefinition() { return Definition(); }

G4RDTag* G4RDTag::RDTag() { return Definition(); }

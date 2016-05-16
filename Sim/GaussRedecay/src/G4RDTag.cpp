// local
#include "G4RDTag.h"

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

// ######################################################################
// ###                          RDTag                                 ###
// ######################################################################
/*static*/ std::map<int, G4RDTag*> G4RDTag::m_pdg_to_object =
    std::map<int, G4RDTag*>();

/*static*/ G4ParticleDefinition* G4RDTag::make_definition(int pdg_id /*=424242*/) {
    std::string str_name = "rdtag";
    str_name += std::to_string(pdg_id);
    return new G4ParticleDefinition(str_name,   // the name
                                    0.0 * MeV,  // the mass
                                    0.0 * MeV,  // the width
                                    0.0,        // the charge
                                    0,          // the spin
                                    0,          // the parity
                                    0,          // the conjugation
                                    0,          // the isospin
                                    0,          // the z-projection of isospin
                                    0,          // the G-parity
                                    str_name,    // p-type
                                    0,          // lepton
                                    0,          // baryon
                                    pdg_id,     // PDG encoding
                                    true,       // stable
                                    0.0,        // lifetime
                                    nullptr,    // decay table
                                    false,      // shortlived
                                    str_name,    // subtype
                                    pdg_id      // antiparticle
                                    );
}

/*static*/ G4RDTag* G4RDTag::Definition(int pdg_id /*=424242*/) {
    /*try to find the definition in the map first.*/
    auto match = m_pdg_to_object.find(pdg_id);
    if (match != end(m_pdg_to_object)) {
        return match->second;
    }

    // search in particle table]
    G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* anInstance = pTable->FindParticle(pdg_id);
    if (anInstance == nullptr) {
        // create particle
        anInstance = make_definition(pdg_id);
        pTable->Insert(anInstance);
    } else {
      std::cout << "G4RDTag: ID " << pdg_id << "already in table!" << std::endl;
      return nullptr;
    }
    m_pdg_to_object[pdg_id] =
        reinterpret_cast<G4RDTag*>(anInstance);
    return m_pdg_to_object[pdg_id];
}

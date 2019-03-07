#pragma once
#include "HepMC/FourVector.h"

/* Basic particle object to hold the information extracted from Geant4 during the
 * tracking of a Geant4 track
 */

class LinkedParticle;
namespace Gaussino
{
  class G4TruthParticle
  {
    // TODO: Need to make sure all information is stored here
  public:
    G4TruthParticle() = delete;
    G4TruthParticle( int trackID, int pdgID, int creatorID, HepMC::FourVector momentum, HepMC::FourVector origin_vertex,
                     HepMC::FourVector end_vertex )
        : m_pdgID( pdgID )
        , m_trackID( trackID )
        , m_creatorID( creatorID )
        , m_momentum( momentum )
        , m_origin_vertex( origin_vertex )
        , m_end_vertex( end_vertex ){};
    virtual ~G4TruthParticle() = default;
    void SetLinkedParticle( LinkedParticle* lp ) { m_linkedParticle = lp; }
    LinkedParticle* GetLinkedParticle() { return m_linkedParticle; }
    int GetPdgID() { return m_pdgID; }
    int GetTrackID() { return m_trackID; }
    int GetCreatorID() { return m_creatorID; }
    HepMC::FourVector GetMomentum() { return m_momentum; }
    HepMC::FourVector GetOriginVertex() { return m_origin_vertex; }
    HepMC::FourVector GetEndVertex() { return m_end_vertex; }

  private:
    int m_pdgID;
    int m_trackID;
    int m_creatorID;
    HepMC::FourVector m_momentum;
    HepMC::FourVector m_origin_vertex;
    HepMC::FourVector m_end_vertex;
    LinkedParticle* m_linkedParticle{nullptr};
  };

} /* Gaussino */

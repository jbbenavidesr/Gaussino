#pragma once
#include "Geant4/G4VUserPrimaryParticleInformation.hh"
#include "Geant4/G4PrimaryParticle.hh"

/** @class GaussinoPrimaryParticleInformation
 *
 *  Class to store an ID for the linking
 *  This is to ensure that the information is associated later on with the correct particle.
 *  @author Dominik Muller
 *  @date   2019-02-27
 */
class GaussinoPrimaryParticleInformation : public G4VUserPrimaryParticleInformation
{
public:
  GaussinoPrimaryParticleInformation()          = default;
  virtual ~GaussinoPrimaryParticleInformation() = default;
  friend std::ostream& operator<<( std::ostream&, const GaussinoPrimaryParticleInformation& );

  /// Get the linked particle
  unsigned int getLinkedID() { return m_linkedID; }
  void setLinkedID(unsigned int lp) { m_linkedID = lp; }
  void Print() const override {}

  inline static GaussinoPrimaryParticleInformation* Get( G4PrimaryParticle* part )
  {
    auto info = part->GetUserInformation();
    GaussinoPrimaryParticleInformation* finfo{nullptr};
    if ( info ) {
      finfo = dynamic_cast<GaussinoPrimaryParticleInformation*>( info );
      if ( !finfo ) {
        // If cast failed we delete the existing info and create the correct one.
        // Though this indicates some problem.
        G4cerr << "Failed to cast existing UserPrimaryParticleInformation. Overwriting existing one!";
        delete info;
      } else {
        return finfo;
      }
    }
    info = new GaussinoPrimaryParticleInformation{};
    part->SetUserInformation( info );
    return dynamic_cast<GaussinoPrimaryParticleInformation*>( info );
  }

private:
  /// Flag to indicate if particle has oscillated
  unsigned int m_linkedID{0};
};



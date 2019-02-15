#pragma once

/* Basic particle object to hold the information extracted from Geant4 during the
 * tracking of a Geant4 track
 */
namespace Gaussino
{
  class G4TruthParticle
  {
  private:
    //TODO: Need to make sure all information is stored here
  public:
    G4TruthParticle()          = default;
    virtual ~G4TruthParticle() = default;
  };

} /* Gaussino */

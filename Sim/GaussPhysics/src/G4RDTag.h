// $Id: G4RDTag.h,v 1.0 2016-04-4 10:29:45 dmuller Exp $
// ============================================================================
#ifndef G4RDTAG_H
#define G4RDTAG_H 1
// ============================================================================
// Include files
// ============================================================================
// GEANT4
// ============================================================================
#include "G4ParticleDefinition.hh"
// ============================================================================
/** @class G4RDTAG
 *  Particle able to be tracked by Geant4 but does absolutely nothing, inspired
 *  by the G4Geantino. Implemented again to allow a nice and unique PID to
 *  tag the vertex in the redecay fast simulation approach.
 *  @author Dominik Muller dominik.muller@cern.ch
 *  @date 2016-04-4
 */
class G4RDTag : public G4ParticleDefinition {
  public:
  virtual ~G4RDTag();  // virtual desctructor
  static G4RDTag* Definition();
  static G4RDTag* RDTagDefinition();
  static G4RDTag* RDTag();

  private:
  G4RDTag() {}
  static G4RDTag* theInstance;
};

#endif


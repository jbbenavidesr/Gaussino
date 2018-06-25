#pragma once
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4Event;
namespace HepMC{
class GenEvent;
}

/** @class IHepMC3ToGeant4Tool SimG4Interface/IHepMC3ToGeant4Tool.h IHepMC3ToGeant4Tool.h
 *
 *  Abstract interface to tool converting HepMC3 to Geant4
 *
 *  @author Dominik Muller
 *  @date   22.6.2018
 */

class IHepMC3ToGeant4Tool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IHepMC3ToGeant4Tool, 1, 0);

  /** 
   * Converts an HepMC3 event to Geant4
   *  
   */
  virtual G4Event* g4Event(const std::vector<HepMC::GenEvent> &) = 0;
};

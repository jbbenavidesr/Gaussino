#pragma once
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GiGaMTTruth/MCTruthConverter.h"

class G4Event;
namespace HepMC{
class GenEvent;
}

/** @class IHepMC3ToMCTruthTrackerSimG4Interface/IHepMC3ToGeant4Tool.h IHepMC3ToGeant4Tool.h
 *
 *  Abstract interface to tool converting HepMC3 to internal MCTruthTrackers
 *
 *  @author Dominik Muller
 *  @date   15.02.2019
 */

class IHepMC3ToMCTruthTracker: virtual public IAlgTool {
public:
  DeclareInterfaceID(IHepMC3ToMCTruthTracker, 1, 0);

  /** 
   * Converts an HepMC3 event to Geant4
   *  
   */
  virtual  Gaussino::MCTruthTracker BuildConverter(const std::vector<const HepMC::GenEvent*> &) const = 0;
};

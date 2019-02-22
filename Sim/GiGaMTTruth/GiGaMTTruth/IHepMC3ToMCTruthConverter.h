#pragma once
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GiGaMTCore/Truth/MCTruthConverter.h"

class G4Event;
namespace HepMC{
class GenEvent;
}

/** @class IHepMC3ToMCTruthConverterSimG4Interface/IHepMC3ToGeant4Tool.h IHepMC3ToGeant4Tool.h
 *
 *  Abstract interface to tool converting HepMC3 to internal MCTruthConverters
 *
 *  @author Dominik Muller
 *  @date   15.02.2019
 */

class IHepMC3ToMCTruthConverter: virtual public IAlgTool {
public:
  DeclareInterfaceID(IHepMC3ToMCTruthConverter, 1, 0);

  /** 
   * Converts an HepMC3 event to Geant4
   *  
   */
  virtual  Gaussino::MCTruthConverterPtrs BuildConverter(const std::vector<HepMC::GenEvent> &) const = 0;
};

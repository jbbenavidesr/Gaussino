#pragma once
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GiGaMTCoreTruth/MCTruthConverter.h"
#include "HepMCUser/typedefs.h"

class G4Event;
namespace HepMC{
class GenEvent;
}

/** @class IHepMC3ToMCTruthConverter
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
  virtual  Gaussino::MCTruthConverterPtrs BuildConverter(const HepMC3::GenEventPtrs &) const = 0;
  virtual  Gaussino::MCTruthConverterPtr BuildConverter(const HepMC3::ConstGenParticlePtr &) const = 0;
};

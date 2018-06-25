#pragma once

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "SimInterfaces/IHepMC3ToGeant4Tool.h"


/** @class HepMC3ToGeant4Tool HepMC3ToGeant4Tool.h "HepMC3ToGeant4Tool.h"
*
*  Basic tool to translate an HepMC::GenEvent into a G4Event
*  Only stable particles are converted, i.e. those without
*  and end vertex in the HepMC record
*
*  @author Dominik Muller
*  @date   22.6.2018
*
*/
class HepMC3ToGeant4Tool : public extends<GaudiTool, IHepMC3ToGeant4Tool>{
public:
  using extends::extends;

  virtual ~HepMC3ToGeant4Tool();

  virtual G4Event* g4Event(const std::vector<HepMC::GenEvent> &) override final;

private:
};

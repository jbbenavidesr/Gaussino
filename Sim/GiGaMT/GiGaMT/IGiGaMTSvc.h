#pragma once

#include <vector>
#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GiGaMTCore/G4EventProxy.h"
#include "NewRnd/RndCommon.h"
#include "GiGaMTCore/Truth/MCTruthConverter.h"


// Forward declaration from G4
class G4Event;
class G4PrimaryVertex;
class G4HCofThisEvent;
class G4TrajectoryContainer;

namespace HepMC {
  class GenEvent;
}

namespace CLHEP {
  class HepRandomEngine;
}

/** @class IGiGaMTSvc IGiGaMTSvc.h GiGaMT/IGiGaMTSvc.h
 *
 * Definition of the abstract interface to the Geant4 MT implementenation.
 * Based on GiGa from Vanya Belyaev.
 *
 *  @author Dominik Muller
 */

class IGiGaMTSvc : virtual public IService
{
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IGiGaMTSvc, 1, 0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() = 0;

  /**  initialize
   *   TODO: Simulate HepMC events. Currently does not return anything,
   *   should return the simulation result ...
   *   @return status code
   */
  virtual std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> simulate(Gaussino::MCTruthConverterPtrs &&, HepRandomEnginePtr &) const = 0;

public:

protected:
  /// virtual destructor
  virtual ~IGiGaMTSvc(){};
};

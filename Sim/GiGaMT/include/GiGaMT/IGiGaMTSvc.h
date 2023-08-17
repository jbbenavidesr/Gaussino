/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "HepMCUser/typedefs.h"
#include "NewRnd/RndCommon.h"
#include <vector>

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

class IGiGaMTSvc : virtual public IService {
public:
  /// Retrieve interface ID
  DeclareInterfaceID( IGiGaMTSvc, 1, 0 );

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() override = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() override = 0;

  /**  initialize
   *   TODO: Simulate HepMC events. Currently does not return anything,
   *   should return the simulation result ...
   *   @return status code
   */
  virtual std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> simulate( const HepMC3::GenEventPtrs&,
                                                                      HepRandomEnginePtr& ) const      = 0;
  virtual std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> simulate( Gaussino::MCTruthConverterPtrs&&,
                                                                      HepRandomEnginePtr& ) const      = 0;
  virtual std::tuple<G4EventProxyPtr, Gaussino::MCTruthPtr> simulateDecay( const HepMC3::GenParticlePtr&,
                                                                           HepRandomEnginePtr& ) const = 0;

public:
  virtual bool particleKnownToGeant4( int pdg_id ) const = 0;

protected:
  /// virtual destructor
  virtual ~IGiGaMTSvc(){};
};

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
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "HepMCUser/typedefs.h"

class G4Event;
namespace HepMC {
  class GenEvent;
}

/** @class IHepMC3ToMCTruthConverter
 *
 *  Abstract interface to tool converting HepMC3 to internal MCTruthConverters
 *
 *  @author Dominik Muller
 *  @date   15.02.2019
 */

class IHepMC3ToMCTruthConverter : virtual public IAlgTool {
public:
  DeclareInterfaceID( IHepMC3ToMCTruthConverter, 1, 0 );

  /**
   * Converts an HepMC3 event to Geant4
   *
   */
  virtual Gaussino::MCTruthConverterPtrs BuildConverter( const HepMC3::GenEventPtrs& ) const        = 0;
  virtual Gaussino::MCTruthConverterPtr  BuildConverter( const HepMC3::ConstGenParticlePtr& ) const = 0;
};

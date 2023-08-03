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

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "HepMC3/GenParticle.h"
#include <algorithm>
#include <functional>

// Forward declarations
namespace HepMC3 {
  class GenEvent;
}

namespace LHCb {
  class GenCollision;
}

class IDecayTool;

/** @class IGenCutTool IGenCutTool.h "MCInterfaces/IGenCutTool.h"
 *
 *  Abstract interface to generator level cut. This type of cut is applied
 *  to the interaction containing the signal particle. The interaction, at
 *  this point contains undecayed particles (except excited heavy particles).
 *
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

class IGenCutTool : public extend_interfaces<IAlgTool> {
public:
  /// Vector of particles
  typedef std::vector<HepMC3::GenParticlePtr>      ParticleVector;
  typedef std::vector<HepMC3::ConstGenParticlePtr> ConstParticleVector;

  DeclareInterfaceID( IGenCutTool, 6, 0 );

  /** Applies the cut on the signal interaction.
   *  @param[in,out] theParticleVector  List of signal particles. The
   *                                    generator level cut is applied to
   *                                    all these particles and particles
   *                                    which do not pass the cut are removed
   *                                    from theParticleVector.
   *  @param[in]     theGenEvent        Generated interaction. The generator
   *                                    level cut can use the particles in
   *                                    this event to take the decision.
   *  @param[in]     theCollision       Hard process information of the
   *                                    interaction which can be used by
   *                                    the cut to take the decision.
   *  @return        true  if the event passes the generator level cut.
   */
  virtual bool applyCut( ParticleVector& theParticleVector, const HepMC3::GenEvent* theGenEvent,
                         const LHCb::GenCollision* theCollision ) const = 0;
};

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

#include <string>

namespace Gaussino {
  namespace HepMC {
    namespace Attributes {
      const std::string GeneratorName       = "GeneratorName";
      const std::string SignalProcessID     = "SignalProcessID";
      const std::string SignalProcessVertex = "SignalProcessVertex";
      const std::string GaudiEventNumber    = "GaudiEventNumber";
      const std::string GaudiRunNumber      = "GaudiRunNumber";

      const std::string SimResults              = "SimResults";
      const std::string LinkedParticle          = "LinkedParticle";
      const std::string ContainedInMCTruth      = "ContainedInMCTruth";
      const std::string G4EventProxy            = "G4EventProxy";
      const std::string ReDecayOriginalParticle = "ReDecayOriginalParticle";
    } // namespace Attributes
  }   // namespace HepMC
} // namespace Gaussino

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
  namespace HepMCEventLocation {
    const std::string Default = "/Event/Gen/HepMCEvents";
    const std::string Signal  = "/Event/Gen/SignalDecayTree";
    const std::string BInfo   = "/Event/Gen/BInfo";
  } // namespace HepMCEventLocation

  namespace MCParticleLocation {
    const std::string Default = "/Event/MC/Particles";
  }

  namespace MCVertexLocation {
    const std::string Default = "/Event/MC/Vertices";
  }

  namespace GenHeaderLocation {
    const std::string Default       = "/Event/Gen/Header";
    const std::string PreGeneration = "/Event/Gen/HeaderPreGen";
  } // namespace GenHeaderLocation
  namespace G4EventsLocation {
    const std::string Default = "/Event/Sim/G4Events";
  }
  namespace SimHeaderLocation {
    const std::string Default = "Sim/Header";
  }
  namespace MCTruthsLocation {
    const std::string Default         = "/Event/MC/Truths";
    const std::string SignalTruthsMap = "/Event/MC/SignalTruths";
  } // namespace MCTruthsLocation
  namespace LinkedParticleMCParticleLinksLocation {
    const std::string Default = "/Event/MC/LinkedParticleMCParticleLinksLocation";
  }
  namespace ReDecayToken {
    const std::string Default = "/Event/ReDecay/Token";
  }
} // namespace Gaussino

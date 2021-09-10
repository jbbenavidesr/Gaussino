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
#include<string>

namespace Gaussino
{
  namespace HepMCEventLocation
  {
    const std::string Default = "Gen/HepMCEvents";
    const std::string Signal  = "Gen/SignalDecayTree";
    const std::string BInfo   = "Gen/BInfo";
  }

  namespace MCParticleLocation {
    const std::string Default = "MC/Particles";
  }
  
  namespace MCVertexLocation {
    const std::string Default = "MC/Vertices";
  }

  namespace GenHeaderLocation {
    const std::string Default = "Gen/Header";
    const std::string PreGeneration = "Gen/HeaderPreGen";
  }
  namespace G4EventsLocation
  {
    const std::string Default = "Sim/G4Events";
  }
  namespace MCTruthsLocation
  {
    const std::string Default = "MC/Truths";
    const std::string SignalTruthsMap = "MC/SignalTruths";
  }
  namespace LinkedParticleMCParticleLinksLocation
  {
    const std::string Default = "MC/LinkedParticleMCParticleLinksLocation";
  }
  namespace ReDecayToken
  {
    const std::string Default = "ReDecay/Token";
  }
}

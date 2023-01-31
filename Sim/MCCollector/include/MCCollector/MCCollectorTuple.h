/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

// LHCb
#include "Event/MCHeader.h"
#include "Event/MCHit.h"
// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

namespace MCCollector {

  class TupleAlg : public GaudiTupleAlg {

    Gaudi::Property<std::string> m_tupleName{this, "TupleName", "Particles"};

  public:
    using GaudiTupleAlg::GaudiTupleAlg;

  protected:
    void                                   fillCollectorTuple( const LHCb::MCHeader&, const LHCb::MCHits& ) const;
    mutable Gaudi::Accumulators::Counter<> m_collHitsCounter{this, "CollectorHitsCounter"};
  };

} // namespace MCCollector

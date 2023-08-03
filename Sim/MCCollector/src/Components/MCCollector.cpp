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
// Gaudi
#include "GaudiAlg/Consumer.h"
// MCCollector
#include "MCCollector/MCCollectorTuple.h"

namespace MCCollector {
  using TupleBaseClass = Gaudi::Functional::Traits::BaseClass_t<TupleAlg>;
  class Collector
      : public Gaudi::Functional::Consumer<void( const LHCb::MCHeader&, const LHCb::MCHits& ), TupleBaseClass> {

  public:
    Collector( const std::string& name, ISvcLocator* pSvcLocator )
        : Consumer( name, pSvcLocator,
                    { KeyValue{ "MCHeader", LHCb::MCHeaderLocation::Default }, KeyValue{ "CollectorHits", "" } } ){};

    void operator()( const LHCb::MCHeader& evt, const LHCb::MCHits& collHits ) const override {
      fillCollectorTuple( evt, collHits );
    }
  };
} // namespace MCCollector

DECLARE_COMPONENT_WITH_ID( MCCollector::Collector, "MCCollector" )

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
#include "GiGaMTCoreRun/SimResults.h"
#include "HepMC3/Attribute.h"
#include <string>

namespace HepMC3 {
  class SimResultsAttribute : public Attribute {
  public:
    SimResultsAttribute() : Attribute(), m_val{ nullptr, nullptr } {}
    SimResultsAttribute( Gaussino::GiGaSimReturn val ) : Attribute(), m_val( val ) {}
    SimResultsAttribute( Gaussino::MCTruthPtr val, G4EventProxyPtr val2 ) : Attribute(), m_val{ val2, val } {}

    bool from_string( const string& ) override { return true; };

    bool to_string( string& ) const override { return true; };

    Gaussino::GiGaSimReturn value() const { return m_val; }

    // Pull in the base class init method accepting reference
    // to GenRunInfo to avoid failures in the template instantiation
    // of GenEvent::attribute<T>
    using Attribute::init;
    virtual bool init( const GenEvent& ) { return true; }

    void set_value( const Gaussino::GiGaSimReturn& ptr ) { m_val = ptr; }

  private:
    Gaussino::GiGaSimReturn m_val;
  };
} // namespace HepMC3

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
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "HepMC3/Attribute.h"
#include <string>

namespace HepMC3 {
  class MCTruthAttribute : public Attribute {
  public:
    MCTruthAttribute() : Attribute(), m_val( nullptr ) {}
    MCTruthAttribute( Gaussino::MCTruthPtr val ) : Attribute(), m_val( val ) {}

    bool from_string( const string& ) override { return true; };

    bool to_string( string& ) const override { return true; };

    Gaussino::MCTruthPtr value() const { return m_val; }

    // Pull in the base class init method accepting reference
    // to GenRunInfo to avoid failures in the template instantiation
    // of GenEvent::attribute<T>
    using Attribute::init;
    virtual bool init( const GenEvent& ) { return true; }

    void set_value( const Gaussino::MCTruthPtr& ptr ) { m_val = ptr; }

  private:
    Gaussino::MCTruthPtr m_val;
  };
} // namespace HepMC3

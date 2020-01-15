#include "GiGaMTCoreRun/SimResults.h"
#include "HepMC3/Attribute.h"
#include <string>

namespace HepMC3 {
  class SimResultsAttribute : public Attribute {
  public:
    SimResultsAttribute() : Attribute(), m_val{nullptr, nullptr} {}
    SimResultsAttribute( Gaussino::GiGaSimReturn val ) : Attribute(), m_val( val ) {}
    SimResultsAttribute( Gaussino::MCTruthPtr val, G4EventProxyPtr val2 ) : Attribute(), m_val{val2, val} {}

    bool from_string( const string& ) { return true; };

    bool to_string( string& ) const { return true; };

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

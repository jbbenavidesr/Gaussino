#include "HepMC3/Attribute.h"
#include "GiGaMTCoreTruth/MCTruthConverter.h"
#include <string>

namespace HepMC3
{
  class MCTruthAttribute : public Attribute
  {
  public:
    MCTruthAttribute() : Attribute(), m_val( nullptr ) {}
    MCTruthAttribute( Gaussino::MCTruthPtr val ) : Attribute(), m_val( val ) {}

    bool from_string( const string& ){ return true;};

    bool to_string( string& ) const { return true;};

    Gaussino::MCTruthPtr value() const { return m_val; }

    // Pull in the base class init method accepting reference
    // to GenRunInfo to avoid failures in the template instantiation
    // of GenEvent::attribute<T>
    using Attribute::init;
    virtual bool init( const GenEvent& )
    {
      return true;
    }

    void set_value( const Gaussino::MCTruthPtr& ptr ) { m_val = ptr; }

  private:
    Gaussino::MCTruthPtr m_val;
  };
} // End HepMC namespace

#include "HepMC3/Attribute.h"
#include "HepMC3/GenVertex.h"
#include <string>

namespace HepMC3
{
  // General template attribute that is not persistable.
  template <typename TYPE>
  class TAttribute : public Attribute
  {
  public:
    TAttribute() : Attribute(), m_val{} {}
    TAttribute( const TYPE & val ) : Attribute(), m_val( val ) {}

    bool from_string( const string& ) override
    {
      return true;
    }

    bool to_string( string& ) const override
    {
      return true;
    }

    TYPE value() const { return m_val; }

    void set_value( const TYPE& val ) { m_val = val; }

  private:
    TYPE m_val;
  };
} // End HepMC namespace

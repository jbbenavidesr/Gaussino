#include "HepMC3/Attribute.h"
#include "HepMC3/GenVertex.h"
#include <string>

namespace HepMC3
{
  class VertexAttribute : public Attribute
  {
  public:
    VertexAttribute() : Attribute(), m_val( nullptr ) {}
    VertexAttribute( GenVertexPtr val ) : Attribute(), m_val( val ) {}

    bool from_string( const string& att ) override
    {
      index = std::stoi( att );
      return true;
    }

    bool to_string( string& att ) const override
    {
      att = std::to_string( m_val->id() );
      return true;
    }

    GenVertexPtr value() { return m_val; }
    ConstGenVertexPtr value() const { return m_val; }

    void set_value( const GenVertexPtr& ptr ) { m_val = ptr; }

  private:
    GenVertexPtr m_val;
    int index = -1; // Only used when creating object from string
  };
} // End HepMC namespace

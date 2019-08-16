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

    bool from_string( const string& att )
    {
      index = std::stoi( att );
      return true;
    }

    bool to_string( string& att ) const
    {
      att = std::to_string( m_val->id() );
      return true;
    }

    ConstGenVertexPtr value() const { return m_val; }

    // Pull in the base class init method accepting reference
    // to GenRunInfo to avoid failures in the template instantiation
    // of GenEvent::attribute<T>
    using Attribute::init;
    virtual bool init( const GenEvent& geneve )
    {
      int pos_in_vec = -index - 1;
      if ( pos_in_vec >= (int) geneve.vertices().size() ) {
        return false;
      }
      m_val = geneve.vertices().at( pos_in_vec );
      return true;
    }

    void set_value( const GenVertexPtr& ptr ) { m_val = ptr; }

  private:
    ConstGenVertexPtr m_val;
    int index = -1; // Only used when creating object from string
  };
} // End HepMC namespace

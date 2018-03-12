#include "HepMC/Attribute.h"
#include "HepMC/GenVertex.h"
#include <string>

namespace HepMC
{
  class VertexAttribute : public Attribute
  {
  public:
    VertexAttribute() : Attribute(), m_val( nullptr ) {}
    VertexAttribute( GenVertexPtr val ) : Attribute(), m_val( val ) {}

    bool from_string( const string& att )
    {
      #ifdef HEPMC_HAS_CXX11
      index = std::stoi( att );
      #else
      index = atoi( att.c_str() );
      #endif
      return true;
    }

    bool to_string( string& att ) const
    {
      #ifdef HEPMC_HAS_CXX11
      att = std::to_string( m_val->id() );
      #else
      char buf[24];
      sprintf( buf, "%23li", m_val );
      att = buf;
      #endif
      return true;
    }

    GenVertexPtr value() const { return m_val; }

    // Pull in the base class init method accepting reference
    // to GenRunInfo to avoid failures in the template instantiation
    // of GenEvent::attribute<T>
    using Attribute::init;
    virtual bool init( const GenEvent& geneve )
    {
      if ( index >= (int) geneve.vertices().size() ) {
        return false;
      }
      m_val = geneve.vertices().at( index );
      return true;
    }

    void set_value( const GenVertexPtr& ptr ) { m_val = ptr; }

  private:
    GenVertexPtr m_val;
    int index = -1; // Only used when creating object from string
  };
} // End HepMC namespace

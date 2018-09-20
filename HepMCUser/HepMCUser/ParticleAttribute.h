#include "HepMC/Attribute.h"
#include "HepMC/GenParticle.h"
#include <string>

namespace HepMC
{
  class ParticleAttribute : public Attribute
  {
  public:
    ParticleAttribute() : Attribute(), m_val( nullptr ) {}
    ParticleAttribute( GenParticlePtr val ) : Attribute(), m_val( val ) {}

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

    GenParticlePtr value() const { return m_val; }

    // Pull in the base class init method accepting reference
    // to GenRunInfo to avoid failures in the template instantiation
    // of GenEvent::attribute<T>
    using Attribute::init;
    virtual bool init( const GenEvent& geneve )
    {
      int pos_in_vec = index - 1;
      if ( pos_in_vec >= (int) geneve.particles().size() ) {
        return false;
      }
      m_val = geneve.particles().at( pos_in_vec );
      return true;
    }

    void set_value( const GenParticlePtr& ptr ) { m_val = ptr; }

  private:
    GenParticlePtr m_val;
    int index = -1; // Only used when creating object from string
  };
} // End HepMC namespace

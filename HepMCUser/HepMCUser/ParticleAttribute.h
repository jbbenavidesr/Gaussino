#include "HepMC3/Attribute.h"
#include "HepMC3/GenParticle.h"
#include <string>

namespace HepMC3
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
      att = std::to_string( m_val->id() );
      return true;
    }

    ConstGenParticlePtr value() const { return m_val; }

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
    ConstGenParticlePtr m_val;
    int index = -1; // Only used when creating object from string
  };
} // End HepMC3 namespace

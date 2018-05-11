/////////////////////////////////////////////////////////////////////////
// CompareGenEvent.cc
//
// garren@fnal.gov, January 2008
// Free functions used to compare two copies of GenEvent
//////////////////////////////////////////////////////////////////////////
//

#include <iostream>

#include "Defaults/HepMCAttributes.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMCUser/VertexAttribute.h"
#include "HepMCUtils/CompareGenEvent.h"

namespace HepMC
{

  bool essentiallyEqual(float a, float b, float epsilon=0.01)
  {
      return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
  }

  bool compareGenEvent( const GenEvent& e1, const GenEvent& e2 )
  {
    // std::cout << "compareGenEvent: comparing event " << e1->event_number() << " to event "
    //          << e2->event_number() << std::endl;
    if ( e1.event_number() != e2.event_number() ) {
      std::cerr << "compareGenEvent: event numbers differ " << std::endl;
      return false;
    }
    // Compare all the attributes
    for ( auto& vt1 : e1.attributes() ) {
      if(vt1.first == "GenCrossSection"){
      continue;}
      for ( auto& vt2 : vt1.second ) {
        auto val1 = vt2.second->unparsed_string();
        auto val2 = e2.attributes().at( vt1.first ).at( vt2.first )->unparsed_string();
        if ( val1 != val2 ) {
          std::cerr << "compareAttributes: " << vt1.first << " = " << val1 << " & " <<val2 << std::endl;
          return false;
        }
      }
    }
    if ( !compareSignalProcessVertex( e1, e2 ) ) {
      return false;
    }
    if ( !compareBeamParticles( e1, e2 ) ) {
      return false;
    }
    if ( !compareWeights( e1, e2 ) ) {
      return false;
    }
    if ( e1.heavy_ion() != e2.heavy_ion() ) {
      std::cerr << "compareGenEvent: heavy ions differ " << std::endl;
      return false;
    }
    // if( e1.pdf_info() != e2.pdf_info() ) {
    // std::cerr << "compareGenEvent: pdf info differs " << std::endl;
    // return false;
    //}
    if ( !compareParticles( e1, e2 ) ) {
      return false;
    }
    if ( !compareVertices( e1, e2 ) ) {
      return false;
    }
    return true;
  }

  bool compareSignalProcessVertex( const GenEvent& e1, const GenEvent& e2 )
  {
    // compare signal process vertex

    auto sp1 = e1.attribute<VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex );
    auto sp2 = e2.attribute<VertexAttribute>( Gaussino::HepMC::Attributes::SignalProcessVertex );
    if ( !sp1 && !sp2 ) {
      return true;
    }
    if ( ( !sp1 && sp2 ) || ( sp1 && !sp2 ) ) {
      std::cerr << "compareSignalProcessVertex: mismatch " << std::endl;
      return false;
    }
    auto s1   = sp1->value();
    auto s2   = sp2->value();
    auto test = *s1;
    if ( s1 && s2 ) {
      if ( ( *s1 ) != ( *s2 ) ) {
        std::cerr << "compareSignalProcessVertex: signal process vertices differ " << std::endl;
        return false;
      }
    }
    return true;
  }

  bool compareBeamParticles( const GenEvent& e1, const GenEvent& e2 )
  {
    auto e1b1 = e1.beam_particles().first;
    auto e1b2 = e1.beam_particles().second;
    auto e2b1 = e2.beam_particles().first;
    auto e2b2 = e2.beam_particles().second;
    if ( e1b1 && e1b2 && e2b1 && e2b2 ) {
      if ( ( *e1b1 ) == ( *e2b1 ) && ( *e1b2 ) == ( *e2b2 ) ) {
      } else {
        std::cerr << "compareBeamParticles: beam particles differ " << std::endl;
        return false;
      }
    }
    return true;
  }

  bool compareWeights( const GenEvent& e1, const GenEvent& e2 )
  {
    if ( e1.weights() == e2.weights() ) return true;
    std::cerr << "compareWeights: weight containers differ " << std::endl;
    return false;
  }

  bool compareParticles( const GenEvent& e1, const GenEvent& e2 )
  {
    if ( e1.particles_size() != e2.particles_size() ) {
      std::cerr << "compareParticles: number of particles differs " << std::endl;
      return false;
    }
    if ( e1.particles_size() == 0 ) {
      return true;
    }
    HepMC::GenParticlePtr p1, p2;
    for ( auto pp_pair : ranges::view::zip( e1.particles(), e2.particles() ) ) {
      // for ( auto [a, b] : ranges::view::zip( e1.particles(), e2.particles() ) ) {
      std::tie( p1, p2 ) = pp_pair;
      if ( *p1 != *p2 ) {
        std::cerr << "compareParticles: particle " << p1->id() << " differs from " << p2->id() << std::endl;
        return false;
      }
    }
    return true;
  }

  bool compareVertices( const GenEvent& e1, const GenEvent& e2 )
  {
    if ( e1.vertices_size() != e2.vertices_size() ) {
      std::cerr << "compareVertices: number of vertices differs " << std::endl;
      return false;
    }
    HepMC::GenVertexPtr v1, v2;
    for ( auto vv_pair : ranges::view::zip( e1.vertices(), e2.vertices() ) ) {
      std::tie( v1, v2 ) = vv_pair;
      if ( ( *v1 ) != ( *v2 ) ) {
        std::cerr << "compareVertices: vertex " << v1->id() << " differs" << std::endl;
        return false;
      }
    }
    return true;
  }

  bool compareVertex( const GenVertex& v1, const GenVertex& v2 )
  {
    auto v1pos = v1.position();
    auto v2pos = v2.position();
    if ( !(essentiallyEqual(v1pos.x(), v2pos.x()) &&  essentiallyEqual(v1pos.y(), v2pos.y()) && essentiallyEqual(v1pos.z(), v2pos.z()) && essentiallyEqual(v1pos.t(), v2pos.t()))) {
      std::cerr << "compareVertex: position differs:\n" << v1 << v2 << std::endl;
      return false;
    }
    // if the size of the inlist differs, return false.
    if ( v1.particles_in_size() != v2.particles_in_size() ) {
      std::cerr << "compareVertex: particles_in_size " << v1.id() << " differs" << std::endl;
      return false;
    }
    // loop over the inlist and ensure particles are identical
    if ( v1.particles_in_const_begin() != v1.particles_in_const_end() ) {
      for ( GenVertex::particles_in_const_iterator ia = v1.particles_in_const_begin(),
                                                   ib = v2.particles_in_const_begin();
            ia != v1.particles_in_const_end(); ia++, ib++ ) {
        if ( **ia != **ib ) {
          std::cerr << "compareVertex: incoming particle " << v1.id() << " differs: " << ( *ia )->id() << " "
                    << ( *ib )->id() << std::endl;
        }
      }
    }
    // if the size of the outlist differs, return false.
    if ( v1.particles_out_size() != v2.particles_out_size() ) {
      std::cerr << "compareVertex: particles_out_size " << v1.id() << " differs" << std::endl;
      return false;
    }
    // loop over the outlist and ensure particles are identical
    if ( v1.particles_out_const_begin() != v1.particles_out_const_end() ) {
      for ( GenVertex::particles_out_const_iterator ia = v1.particles_out_const_begin(),
                                                    ib = v2.particles_out_const_begin();
            ia != v1.particles_out_const_end(); ia++, ib++ ) {
        if ( **ia != **ib ) {
          std::cerr << "compareVertex: outgoing particle differs for vertices: " << v1 << v2 << *( *ia )
                    << " compared to \n " << *( *ib ) << std::endl;
          return false;
        }
      }
    }
    return true;
  }

  bool compareParticle( const GenParticle& p1, const GenParticle& p2 )
  {
    auto p1mom = p1.momentum();
    auto p2mom = p2.momentum();
    if ( !(essentiallyEqual(p1mom.px(), p2mom.px()) &&  essentiallyEqual(p1mom.py(), p2mom.py()) && essentiallyEqual(p1mom.pz(), p2mom.pz()) && essentiallyEqual(p1mom.e(), p2mom.e()))) {
      std::cerr << "compareParticle: momentum different!\n" << p1 << p2 << std::endl;
      return false;
    }
    if ( p1.pdg_id() != p2.pdg_id() ) {
      std::cerr << "compareParticle: PDG different!\n" << p1 << p2 << std::endl;
      return false;
    }
    if ( p1.status() != p2.status() ) {
      std::cerr << "compareParticle: STATUS different!\n" << p1 << p2 << std::endl;
      return false;
    }
    // Do not do any comparisons with the vertices here as compareVertex
    // already checks ingoing and outgoing particles and this would end
    // up in an infinite loop

    return true;
  }

} // HepMC

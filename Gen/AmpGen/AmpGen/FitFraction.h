

#ifndef FITFRACTION_H
#define FITFRACTION_H
#include "AmpGen/Particle.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/EventType.h"

namespace AmpGen {
  class FitFraction {
    std::string m_name;
      double m_value;
    double m_error;
    std::shared_ptr<Particle> m_particle ;
    public :
    double val() const { return m_value ; }
    double err() const { return m_error ; }
    std::string name() const { return m_name ; }
    std::shared_ptr<Particle> particle() const { return m_particle ; }
    FitFraction( const std::string& line , const AmpGen::EventType& evtType = AmpGen::EventType() ){ 
      auto tokens = split( line, ' ');
      m_name = tokens[1];
      m_value = stod( tokens[2] );
      m_error = stod( tokens[3] );
      if( evtType.size() != 0 ){
        std::vector<std::string> finalStates = evtType.finalStates();
        m_particle = std::make_shared<Particle>( tokens[1], finalStates );
      } 
    }
    FitFraction( const std::string& name, const double& frac, const double& err, std::shared_ptr<Particle> part = nullptr ) : m_name( name ), m_value(frac),m_error(err),m_particle(part) {
      
    }

  };
}

#endif 

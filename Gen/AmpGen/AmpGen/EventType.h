#ifndef EVENTTYPE_H
#define EVENTTYPE_H 1

#include "AmpGen/Utilities.h"
#include "AmpGen/ParticlePropertiesList.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/TeXFormat.h"

namespace AmpGen { 
  class EventType { /// is a particle ordering /// 
    private:
      std::string m_mother;
      std::vector<std::string> m_particleNames;
      std::vector<std::string> m_particleNamesPickled;
      std::vector<double> m_particleMasses;
      double m_motherMass;
    public: 

      EventType(){};

      std::map<std::string, unsigned int> getEventFormat(){
        std::map<std::string,unsigned int> returnValue;

        for( unsigned int ip=0;ip < size();++ip ){
          returnValue[ std::to_string(ip) +"_Px" ] = 4*ip + 0;
          returnValue[ std::to_string(ip) +"_Py" ] = 4*ip + 1;
          returnValue[ std::to_string(ip) +"_Pz" ] = 4*ip + 2;
          returnValue[ std::to_string(ip) +"_E" ]  = 4*ip + 3;
        }
        return returnValue; 
      }

      std::pair<double, double> minmax( const std::vector<unsigned int>& indices , bool isGeV=false) const {

        std::vector<unsigned int> ivec( size() );
        std::iota( ivec.begin(), ivec.end(), 0 );
        double min(0);
        double max(motherMass());
        for( auto& x : indices )
          min += mass(x);
        for( auto& x : ivec )
          if( std::find( indices.begin(), indices.end(), x ) == indices.end() )
            max -= mass(x);
        DEBUG( "Binning = " << min << " , " << max );
        double units=isGeV?1000*1000:1;
        return std::pair<double,double>( min*min / (1.1*units), max*max * (1.1/units) );
      }
      std::vector<std::vector<unsigned int>> getBosePairs() const {
        std::map< std::string, std::vector<unsigned int> > particleOrdering; 
        for( unsigned int i = 0 ; i < m_particleNames.size(); ++i ) 
          particleOrdering[m_particleNames[i]].push_back( i );
        std::vector<std::vector<unsigned int>> orderings;
        for( auto& im : particleOrdering ) if( im.second.size() != 1 ) orderings.push_back( im.second );
        return orderings;
      }
      std::vector<std::string> finalStates() const { return m_particleNames ; } 
      std::vector<std::string> getPickledFinalStates(const bool flipped=false ) const { 
        if( ! flipped ) return  m_particleNamesPickled ; 
        std::vector<std::string> pickles;
        for( auto& particle : m_particleNames ) 
          pickles.push_back( replaceAll( replaceAll( particle , "+","#"), "-","~") );
        return pickles; 
      }
      std::vector<double> masses() const { return m_particleMasses ; } 
      unsigned int size() const { return m_particleNames.size(); }
      std::string mother() const { return m_mother ; } 
      std::string operator[]( const unsigned int& index ) const { return m_particleNames[index] ; }
      double mass( const unsigned int & index ) const { return m_particleMasses[index] ; }
      double motherMass() const { return m_motherMass ; }
      std::string label( const unsigned int& index, bool isRoot=true){ return getTexFromPDG( m_particleNames[index], isRoot ) ; }
      EventType( const std::string& mother,  
          const std::vector<std::string>& particleNames ) : m_mother(mother), m_particleNames( particleNames ) {
        auto motherProperties = ParticlePropertiesList::getMe()->get(m_mother);
        if( motherProperties != 0 ) m_motherMass = motherProperties->mass();
        else {
          ERROR("Particle not found : " << m_mother );
          return ;
        }
        for( auto& particle : particleNames ){
          auto prop = ParticlePropertiesList::getMe()->get(particle);
          if( prop != 0 ){
            m_particleMasses.push_back( prop->mass() );
          }
          else {
            ERROR("Particle not found : " << particle );
            return;
          }
          m_particleNamesPickled.push_back( replaceAll( replaceAll( particle , "+","~"), "-","#") );
        }
        DEBUG( mother <<  " = " << m_motherMass << " -> " );
        for( unsigned int i = 0 ; i < m_particleNames.size() ;++i ){
          DEBUG( m_particleNames[i] << " = " << m_particleNamesPickled[i] << " = " << m_particleMasses[i] );
        }
      }
  };
}

#endif

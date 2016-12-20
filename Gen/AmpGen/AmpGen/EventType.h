#ifndef EVENTTYPE_H
#define EVENTTYPE_H 1

#include <vector>
#include <map>

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
      EventType( const std::vector<std::string>& ) ;
      std::map<std::string, unsigned int> getEventFormat() ; 

      std::pair<double, double> minmax( const std::vector<unsigned int>& indices , bool isGeV=false) const ;
      std::vector<std::vector<unsigned int>> getBosePairs() const ;
      std::vector<std::string> finalStates() const ; 
      std::vector<std::string> getPickledFinalStates(const bool flipped=false ) const ;
      std::vector<double> masses() const ;
      unsigned int size() const ;
      std::string mother() const ; 
      std::string operator[]( const unsigned int& index ) const ; 
      double mass( const unsigned int & index ) const ;
      double motherMass() const;
      std::string label( const unsigned int& index, bool isRoot=true) const ; 
  };
}

#endif

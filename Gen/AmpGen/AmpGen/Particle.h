/***********************************************************************
  Particle class - encodes a multibody decay tree structure, essentially
  limited by spin factor and propagator combinations, which are limited 
  at three body.

  - Particle( string, vector<string> ) 
  Generates the decay tree structure from a tree using the following
  syntax X[l;prop]{Y,Z}, where Y, Z have the same tree structure.
  [] represents modifiers, l can be S,P,D for orbitals (defaults to ground
  state, assuming strong force conserves parity. Can also modify the lineshape 
  used (defaults to Breit-Wigner). This is the same syntax used in inverse 
  for uniqueString()

  - Expression getExpression( vector<DBYSMBOL>* ) 
  Gets the expression that represents this amplitude, taking as an argument
  a vector of named expressions that can be used for debugging.


  T.Evans 10/15


 ***********************************************************************/



#ifndef PARTICLE_H
#define PARTICLE_H

/// STL
#include <vector>
#include <complex>

/// CGEN
#include "AmpGen/Expression.h"
#include "AmpGen/Lineshapes.h"
#include "AmpGen/Tensor.h"

/// AmpGen
#include "AmpGen/ParticlePropertiesList.h"

namespace AmpGen { 
class Particle {
  private:
    //// Particle Properties ////  
    const ParticleProperties* m_props; 
    std::string m_name; /// name of particle ///
    unsigned int m_index; /// index - for final state particles ///
    std::vector<std::shared_ptr<Particle>> m_daughters; 
    /// array of daughters - sort of owned by mother, but shared anyway ///
    unsigned int m_spin; /// spin 
    int m_parity; /// parity 
    unsigned int m_orbital; 
    unsigned int m_spinConfigurationNumber; /// spin configuration number for decays such as V[P]->VV
    /// orbital angular momentum of daughter particles - only really makes sense for 1->2 decays
    bool m_istop; /// flag that the particle is the top of the decay chain ///
    unsigned int m_minL; /// minimum angular momentum, determined by selection rules ///
    std::string m_lineshape; /// lineshape modifier ////
    std::vector<std::string> m_modifiers; 
    std::string m_uniqueString;
    double  m_parity_factor; 
    bool m_conjThis;  /// flag to conjugate final state in LaTeX
    /// parity factor for CP conjugate amplitudes, determines sign of 3-momenta  
    bool m_isStateGood; /// flag to mark particles not correctly configured /// 
    void pdg_lookup();
    bool hasModifier( const std::string& modifier ) const ;
    std::string modifierString() const ; 
    void sortDaughters(){
      std::stable_sort( m_daughters.begin(), 
          m_daughters.end(), 
          []( std::shared_ptr<Particle> A, 
            std::shared_ptr<Particle> B ){ 
              if( A->isStable() && B->isStable() ) return A->index() < B->index() ;   
              if( ! A->isStable() && B->isStable() ) return true;
              if( A->isStable() && ! B->isStable() ) return false; 
              return A->spin() == B->spin() ? A->mass() > B->mass() : A->spin() > B->spin() ; 
           } );
    }
  public: 
    MultiQuarkContent quarks(){
      return m_lineshape == "NonRes" ? daughterQuarks() : m_props->netQuarkContent();}
    MultiQuarkContent daughterQuarks(){
      MultiQuarkContent quarks;
      for( auto& d : m_daughters ) quarks += d->quarks();
      return quarks;
    }


    /// Constructors for Particle( name / id -> ) -> P1, P2 
    Particle( const std::string& name, const Particle& p1, const Particle& p2) ;
    /// Quasi Two body process indexed by PDG rather than name 
    Particle( const int& pdg_id, const Particle& p1, const Particle& p2);

    /// for final state particles ///
    Particle( const std::string& name, const unsigned int& index ); 
    /// parses the decayString to a decayTree 
    Particle( const std::string& decayString, std::vector<std::string>& finalStates);
    bool checkExists() ;

    /// internally used hashing functions ///
    unsigned int TwoParticleHash() const; 
    

    /// property retrieval ///
    unsigned int spin() const; 
    bool isStateGood() const { return m_isStateGood ; }
    int parity() const{ return m_parity; }
    unsigned int orbital() const { return m_orbital; }
    std::string orbitalString() const;
    std::string tex() const; 
    double mass() const { return m_props->mass(); }

    const Expression massSq() const {
      if( isStable() ) return  mass() * mass();
      else return dot(P(),P());
    };
    void addModifier( const std::string& mod ){ 
      m_modifiers.push_back( mod );
      parseModifier( mod ); 
      m_uniqueString = makeUniqueString();
    }
    /// setters
    void parseModifier( const std::string& mod )  ; 
    void setConj(bool setConj){ m_conjThis=setConj; 
      for( auto& d : m_daughters ) d->setConj( setConj ) ; 
    }
    bool isTop(){ return m_istop; } 
    void setTop(bool state=true){ m_istop=state;}
    void setIndex( const unsigned int& index){ m_index = index; }
    void setWave( const unsigned int& wave){ 
      m_orbital = wave;       
      m_uniqueString = makeUniqueString();
    }
    void conjugateFlavour(){
      INFO("Conjugating flavour of " << m_name );
      if( m_props == 0 ) ERROR("Particle properties not found for " << m_name );
      if( !m_istop && m_props->hasDistinctAnti() ) {
        INFO("Trying to conjugate : " << m_name );
        m_props = ParticlePropertiesList::getMe()->get( - m_props->pdg_id() );
        m_name = m_props->name() ;
        pdg_lookup();
      }
      for( auto& d : m_daughters ) d->conjugateFlavour();
      std::string oldUniqueString = m_uniqueString ;
      m_uniqueString = makeUniqueString();
      INFO("Conjugate : " << oldUniqueString << " to " << m_uniqueString );
    };
    void setLineshape ( const std::string& lineshape ){ 
      m_lineshape = lineshape;   
      m_uniqueString = makeUniqueString();
    }
    void setParityFactor(const double& p){ 
      m_parity_factor = p;
      for( auto& d : m_daughters ) d->setParityFactor(m_parity_factor);
    }
    int finalStateParity(){
      int lpart = ( m_orbital % 2 == 0 ? 1 : -1 );
      for( auto& d : m_daughters) lpart *= d->parity();
      return lpart;
    };
    void CPConjugateThis();
    
    std::pair<unsigned int, unsigned int> lRange(bool converseParity=true ) const;
    bool conservesParity( unsigned int L=0) const {
      return parity() == daughter(0)->parity()*daughter(1)->parity()*( L % 2 == 0 ? 1 : -1 );
    }
    std::string getTeX() const;
    std::string makeUniqueString();
    bool isStable() const;

    std::string uniqueString() const { return m_uniqueString; }
    std::string topologicalString() const {
      std::string topo="";
      for( auto& d : m_daughters ) topo += d->m_props->SVPAT();
      if( m_daughters.size() == 2 && ! daughter(0)->isStable() && daughter(1)->isStable() ){
        auto cascade1 = daughter(0);
        auto cascade2 = daughter(0)->daughter(0);
        std::vector<std::string> cascade_daughters = { cascade2->daughter(0)->name(), cascade2->daughter(1)->name(), cascade1->daughter(1)->name() };
        std::sort( cascade_daughters.begin(), cascade_daughters.end() );
        for( auto& dp : cascade_daughters ) topo += dp ;
      }
      return topo; 
    };
    std::string name() const { return m_name ; }
    unsigned int index() const { return m_index; }
//    std::vector<double> getExternalParameters( std::map<std::string, double>& externalNames ) ; 
    std::shared_ptr<Particle> daughter( const unsigned int& index );
    std::shared_ptr<Particle> daughter( const unsigned int& index ) const ;

    /// these functions are used internally in generating the amplitude 
    Tensor P() const ; /// momentum sum of daughters 
    Tensor FinalStateSpinTensor() const;
    Tensor Q() const ; /// momentum difference between daughters 
    std::vector<std::shared_ptr<Particle>> getFinalStateParticles() ;
    Expression Lineshape(std::vector<DBSYMBOL>* db=0) const;
    Tensor SpinTensor(std::vector<DBSYMBOL>* db=0);
    Expression FormFactor(std::vector<DBSYMBOL>* db=0) const ;
    /// key functions 
    Expression getExpression(std::vector<DBSYMBOL>* db=0 , const unsigned int& index=0);
}; } 

#endif

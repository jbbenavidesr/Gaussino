#ifndef TENSOR_H
#define TENSOR_H
#include <vector>
#include <iostream>
#include <algorithm>
#include "AmpGen/Expression.h"
#include "AmpGen/MsgService.h"
/*
   void printVector( const std::vector<unsigned int>& vect ){
   for( auto& a : vect ) std::cout << "[" << a << "]";
   std::cout << std::endl; 
   }
   */

namespace AmpGen { 
  struct LorentzIndex {
    std::string m_name;
    bool m_isUpper;
    bool operator==( const LorentzIndex& other ) const { return m_name == other.m_name ; } 
    bool operator!=(const  LorentzIndex& other ) const { return m_name != other.m_name ; }
    bool isUpper() const { return m_isUpper; };
    LorentzIndex( const std::string& name="" , bool isUpper=false) : m_name(name), m_isUpper(isUpper) {}
    LorentzIndex operator-() const { return LorentzIndex( m_name , !m_isUpper) ; } /// contravariant to covariant swap operator // 
  }; }

namespace AmpGen { 
  struct Tensor {

    std::string m_name;
    std::vector<unsigned int> m_dim;
    std::vector<Expression> m_elements;
    std::vector<LorentzIndex> m_indices ;

    Tensor ( const std::vector<unsigned int>& _dim) : 
      m_dim(_dim), 
      m_elements(nElements(),Constant(0.)),
      m_indices(_dim.size() ) {};

    bool rankMatches( const Tensor& other ){
      bool success = true;
      if( m_dim.size() != other.m_dim.size() ) return false ; 
      for( unsigned int i = 0 ; i < m_dim.size(); ++i ) 
        success &= m_dim[i] == other.m_dim[i];
      return success; 
    };
    Expression& operator[] ( const unsigned int& i ) { return m_elements[i] ; }
    Expression& operator[] ( const std::vector<unsigned int>& co ) { return (*this)[index(co)] ; }
    Tensor operator()( const LorentzIndex& a){
      if( m_indices.size() != 1 ){
        ERROR("Setting wrong number of indices ! check logic ( indices = 1; this = " << m_indices.size() << ", " << m_dim.size() << " )" );
        return Tensor(std::vector<unsigned int>{0});
      }
      m_indices[0] = a;

      return *this;
    }
    Tensor operator()( const LorentzIndex& a, const LorentzIndex& b ){
      if( m_indices.size() != 2 ){
        ERROR("Setting wrong number of indices ! check logic ( indices = 2; this = " << m_indices.size() << ", " << m_dim.size() << " )" );
        return Tensor(std::vector<unsigned int>{0});
      }
      m_indices[0] = a;
      m_indices[1] = b;
      return *this;
    }
    Tensor Invert() const ; 
    Tensor operator()( const LorentzIndex& a, const LorentzIndex& b, const LorentzIndex& c ){
      if( m_indices.size() != 3 ){
        ERROR("Setting wrong number of indices ! check logic ( indices = 3; this = " << m_indices.size() << ", " << m_dim.size() << " )" );
        return Tensor(std::vector<unsigned int>({0}));
      }
      m_indices[0] = a;
      m_indices[1] = b;
      m_indices[2] = c;
      return *this;
    }

    Tensor operator()( const LorentzIndex& a, const LorentzIndex& b, const LorentzIndex& c, const LorentzIndex& d){
      if( m_indices.size() != 4 ){
        ERROR("Setting wrong number of indices ! check logic ( indices = 4; this = " << m_indices.size() << ", " << m_dim.size() << " )" );
        return Tensor(std::vector<unsigned int>({0}));
      }
      m_indices[0] = a;
      m_indices[1] = b;
      m_indices[2] = c;
      m_indices[3] = d;
      return *this; 
    }

    Tensor operator[]( const std::vector<LorentzIndex>& indices ){ 
      if( indices.size() != m_indices.size() ){
        ERROR("Setting wrong number of indices ! check logic ( indices = " << indices.size() << " this = " << m_indices.size() << ", " << m_dim.size() << " )" );
        return Tensor(std::vector<unsigned int>({0}));
      }
      for( unsigned int i = 0 ; i < indices.size(); ++i ) m_indices[i] = indices[i];
      return *this; 
    }

    Tensor operator | ( Tensor other ) ; 

    Tensor() ; 
    Tensor( const std::vector<double>&      elements, const std::vector<unsigned int>& _dim)  ;
    Tensor( const std::vector<std::string>& elements, const std::vector<unsigned int>& _dim, bool  resolved=false) ;
    Tensor( const std::vector<Expression>&  elements, const std::vector<unsigned int>& _dim ) ;
    Tensor( const std::vector<Expression>&  elements );

    Expression Determinant() const;
    

    /// get an element of the tensor from the index 
    Expression get( const unsigned int& co ) ;

    Expression get( const unsigned int& co ) const ;

    std::string to_string() ;

    unsigned int rank() const ; 
    int metricSgn(const std::vector<unsigned int>& coordinates ) const ; 
    int metricSgn( const unsigned int& index ) const ;
    void append( const Expression& expression ) ;
    void append( const double& value) ;
    void append( const std::string& name , bool resolved=true) ;
    Expression get( const std::vector<unsigned int>& _co ) const ;
    unsigned int size() const ;  
    unsigned int index( const std::vector<unsigned int>& _co ) const ;
    /// get the coordinates of a given index ////
    std::vector<unsigned int> coords( const unsigned int& index ) const ;

    /// get the number of d.o.f.s of a Tensor of given rank and dimension  
    unsigned int nElements() const ; 
    
    bool isScalar() const {
      return size() == 1 && m_dim[0] == 1;
    };
    bool isSpin1() const {
      return size() == 1 && m_dim[0] == 4;
    }
    bool isSpin2() const {
      return size() == 2 && m_dim[0] == 4 && m_dim[1] == 4;
    }
    // eliminate col i in dim d.
    Tensor Eliminate(const unsigned int& i, const unsigned int& d ) const ;
    //// print operator 
    void print() const ;

    //// contraction operator of the form A_{abcd}*B_{defg} 
  //  Tensor operator*( const Tensor& other ); 
  };  

  //// operators ////
  Tensor operator+( Tensor t1, Tensor t2 );
  Tensor operator-( Tensor t1, Tensor t2 );

  Tensor operator/( Tensor t1, const Expression& t2);
  Tensor operator*( const Expression& t1, Tensor t2);
  Tensor operator*( Tensor t1, const Expression& t2);

  Tensor operator/( Tensor t1, const double& t2);
  Tensor operator*( const double& t1, Tensor t2);
  Tensor operator*( Tensor t1, const double& t2);


  static Tensor Metric4x4(
      std::vector<double>({-1,0,0,0,
        0,-1,0,0,
        0,0,-1,0,
        0,0,0,1}), std::vector<unsigned int>({4,4}) );


  Tensor LeviCivita(const unsigned int& rank=4 ); 
  Tensor outer_product( Tensor A, Tensor B, std::vector<unsigned int> orderingA={}, std::vector<unsigned int> orderingB={} ) ; 


  Expression dot( Tensor A, Tensor B );
  Tensor Orbital_PWave( Tensor A, Tensor B);
  Tensor Orbital_DWave( Tensor A, Tensor B);
  Tensor Spin1ProjectionOperator( Tensor A );
  Tensor Spin2ProjectionOperator( Tensor A );
}

#endif

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

  class TensorHelper ; 
  struct LorentzIndex {
    std::string m_name;
    bool m_isUpper;
    bool operator==( const LorentzIndex& other ) const { return m_name == other.m_name ; } 
    bool operator!=(const  LorentzIndex& other ) const { return m_name != other.m_name ; }
    bool isUpper() const { return m_isUpper; };
    LorentzIndex( const std::string& name="" , bool isUpper=false) : 
      m_name(name), m_isUpper(isUpper) {}
    LorentzIndex operator-() const { return LorentzIndex( m_name , !m_isUpper) ; } /// contravariant to covariant swap operator // 
  }; 

  class Tensor {

    private :  
      std::vector<unsigned int> m_dim;
      std::vector<Expression> m_elements;

    public: 
      const std::vector<unsigned int>& dims() const { return m_dim ; }
      unsigned int nDim() const { return m_dim.size() ; } 

      Tensor ( const std::vector<unsigned int>& _dim) : 
        m_dim(_dim), 
        m_elements(nElements(),Constant(0.)) 
        {};

      bool rankMatches( const Tensor& other ){
        bool success = true;
        if( m_dim.size() != other.m_dim.size() ) return false ; 
        for( unsigned int i = 0 ; i < m_dim.size(); ++i ) 
          success &= m_dim[i] == other.m_dim[i];
        return success; 
      };
      /// Low level access of elements, either by coordinates or by index /// 
      Expression& operator[] ( const unsigned int& i ) { return m_elements[i] ; }
      Expression& operator[] ( const std::vector<unsigned int>& co ) { return (*this)[index(co)] ; }
      const Expression& operator[] ( const unsigned int& i ) const { return m_elements[i] ; }
      const Expression& operator[] ( const std::vector<unsigned int>& co ) const { return (*this)[index(co)] ; }

      Expression get( const unsigned int& co ) ;
      Expression get( const unsigned int& co ) const ;

      Tensor Invert() const; 

      /// TensorHelper access to class members 
      /// High level access is done via these commands, i.e. () operators 
      /// low levels access is done via [] operators /// 
      TensorHelper operator()( ) const ; 

      TensorHelper operator()( const LorentzIndex& a) const; 
      TensorHelper operator()( const LorentzIndex& a, const LorentzIndex& b ) const;
      TensorHelper operator()( const LorentzIndex& a, const LorentzIndex& b, const LorentzIndex& c) const;
      TensorHelper operator()( const LorentzIndex& a, const LorentzIndex& b, const LorentzIndex& c, 
                               const LorentzIndex& d) const ; 

      /*
      template < class ... indices > TensorHelper operator() (indices... indexSet ) const {
        return TensorHelper( *this, { indexSet...} );
      }
      */
      TensorHelper operator()( const std::vector<LorentzIndex>& indices ) const ; 

      Tensor() ; 
      Tensor( const std::vector<double>&      elements, 
              const std::vector<unsigned int>& _dim)  ;

      Tensor( const std::vector<std::string>& elements, 
              const std::vector<unsigned int>& _dim, bool  resolved=false) ;

      Tensor( const std::vector<Expression>&  elements, const std::vector<unsigned int>& _dim ) ;
      Tensor( const std::vector<Expression>&  elements );

      Expression Determinant() const;

      /// get an element of the tensor from the index 
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

  Expression dot( Tensor A, Tensor B );
  Tensor Orbital_PWave( Tensor A, Tensor B);
  Tensor Orbital_DWave( Tensor A, Tensor B);
  Tensor Spin1ProjectionOperator( Tensor A );
  Tensor Spin2ProjectionOperator( Tensor A );

  class TensorHelper {
    public: 

    operator Tensor() { return m_tensor ; } 
    TensorHelper( const Tensor& tensor, const std::vector<LorentzIndex>& indices ) : 
      m_tensor(tensor) {
        if( m_tensor.rank() != indices.size() ){
          ERROR("Setting wrong number of indices ! check logic ( this = " 
              << indices.size() << ", " <<  m_tensor.nDim() << " )" );
        }
        else m_indices = indices ; 
      }
    std::vector<LorentzIndex> indices() const { return m_indices ; } 
    const Tensor& tensor() const { return m_tensor ; } 
    private :
    Tensor m_tensor;
    std::vector<LorentzIndex> m_indices;
  };

  TensorHelper operator*( const TensorHelper& t1, const TensorHelper& t2 );
  TensorHelper operator+( const TensorHelper& t1, const TensorHelper& t2 );
  TensorHelper operator-( const TensorHelper& t1, const TensorHelper& t2 );

  TensorHelper operator/( const TensorHelper& t1, const Expression& t2);
  TensorHelper operator*( const Expression& t1, const TensorHelper& t2);
  TensorHelper operator*( const TensorHelper& t1, const Expression& t2);

  TensorHelper operator/( const TensorHelper& t1, const double& t2);
  TensorHelper operator*( const double& t1, const TensorHelper& t2);
  TensorHelper operator*( const TensorHelper& t1, const double& t2);

}

#endif

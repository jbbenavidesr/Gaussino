#ifndef TENSOR_H
#define TENSOR_H
#include <vector>
#include <iostream>
#include <algorithm>
#include "AmpGen/Expression.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/Utilities.h"
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
    Tensor operator | ( Tensor other ){
      //    std::vector<std::pair<LorentzIndex, unsigned int> > allIndices; 
      std::vector<std::pair<unsigned int, unsigned int>> contractions; 
      //  for( unsigned int i = 0 ; i < m_indices.size(); ++i ) 
      //      allIndices.push_back( std::make_pair(  m_indices[i]  , m_dim[i])  );
      std::vector<LorentzIndex> unsummedIndices;
      for( auto& index : m_indices ) 
        if( std::find( other.m_indices.begin(), other.m_indices.end(), index ) == other.m_indices.end() ) 
          unsummedIndices.push_back( index );
      for( auto& index : other.m_indices )
        if( std::find( m_indices.begin(), m_indices.end(), index ) == m_indices.end() ) 
          unsummedIndices.push_back( index );

      for( unsigned int i = 0 ; i < other.m_indices.size(); ++i ){
        std::vector<LorentzIndex>::iterator it = 
          std::find( m_indices.begin(), m_indices.end(), other.m_indices[i] );
        if( it != m_indices.end() ){
          if( other.m_indices[i].isUpper() != it->isUpper() ){
            contractions.push_back( std::make_pair(std::distance(m_indices.begin(),it),i) );
          }
          else ERROR("Contraction makes no sense!");
        }
        // else allIndices.push_back( std::make_pair( other.m_indices[i], other.m_dim[i] ) );
      }
      int size = m_dim.size() + other.m_dim.size() - 2*contractions.size();
      DEBUG(" multiplying " << m_dim.size() << " x "
          << other.m_dim.size() << " with " 
          << contractions.size() 
          << " contractions (dim = " << size << ")" );
      if(size<0) ERROR("Making an object of negative rank , doesn't make sense");
      Tensor value( std::vector<unsigned int>(size,4));
      unsigned nElem = value.nElements();
      value.m_indices = unsummedIndices; 
      DEBUG( "Size of object = " << value.nElements() );
      for( unsigned int elem = 0 ; elem < nElem ; ++elem ){
        auto coords = value.coords(elem); /// coordinates of this part of the expression /// 
        std::vector<unsigned int> thisCoordinates( m_dim.size() , 0 ), 
          otherCoordinates( other.m_dim.size() , 0);
        unsigned int i = 0 ; 
        unsigned int j = 0 ; 
        do {
          DEBUG("Checking : " << i );
          if( isIn( contractions, i  , [](
                  //INFO( "checking : " << i );
                  const std::pair<unsigned int, unsigned int>& a, 
                  const unsigned int & b ){ return a.first == b ; } ) ){
            DEBUG("Skipping coordinate " << i << " as is being contracted " );
            //  thisCoordinates[i] = 999; 
            continue;
          }
          else {
            DEBUG(" Setting coordinate " << i << " = " << j  );
            thisCoordinates[i] = coords[j];
            j++;
          }
          //j++;
        } while( ++i < m_dim.size() ) ; 

        i=0; 
        j = m_dim.size() - contractions.size() ;
        do { 
          DEBUG("Checking : " << i );
          if( isIn( contractions, i, [](
                  const std::pair<unsigned int, unsigned int>& a,
                  const unsigned int & b ){ return a.second == b ; } ) ){
            DEBUG("Skipping coordinate " << i << " as is being contracted " );

          }
          else {
            otherCoordinates[i]=coords[j];
            DEBUG( "Setting i = " << i << ", (" << j << ")"<< coords[j] );
            j++;
          }

        } while( ++i < other.m_dim.size() );
        unsigned int sumElements=1;
        for( unsigned int i=0;i<contractions.size(); ++i) sumElements*=m_dim[i];
        Expression elementExpression=0;
        //std::cout << "Element ( " ;
        //for( unsigned int i = 0 ; i < coords.size() ; ++i ) std::cout << coords[i] << ") = "; 
        for( unsigned int i = 0 ; i < sumElements; ++i ){  
          std::vector<unsigned int> contractedCoordinates(contractions.size());

          unsigned int elemI = i;
          for( unsigned int n = 0 ; n < contractions.size() ; ++n ){
            unsigned int bi = elemI % 4 ;
            elemI = (elemI - bi ) /4;
            contractedCoordinates[n] = bi;
          }
          int sign=1;
          for( unsigned int i = 0 ; i < contractions.size(); ++i ){
            thisCoordinates[ contractions[i].first ] =   contractedCoordinates[i];
            otherCoordinates[ contractions[i].second ] = contractedCoordinates[i];
            sign *= ( contractedCoordinates[i] ) == 3 ? 1 : -1; 
          }
          if( sign == 1 ) 
            elementExpression = elementExpression + get( thisCoordinates ) * other.get( otherCoordinates );  
          else 
            elementExpression = elementExpression - get( thisCoordinates ) * other.get( otherCoordinates ); 
        }
        value.m_elements[elem] = elementExpression; 
      }
      return value; 
    };
    Tensor ( std::vector<double> elements, 
        std::vector<unsigned int> _dim) : 
      m_dim(_dim),
      m_indices(_dim.size() )
    {
      for( auto& x : elements ) append( x );
    }

    Tensor ( const std::vector<std::string>& elements, 
        const std::vector<unsigned int>& _dim, 
        bool  resolved=false) : m_dim(_dim),
    m_indices(_dim.size() ) 
    {
      for( auto& x : elements ) append( x , resolved );
    }
    Expression Determinant() const;
    Tensor(){};
    Tensor( std::vector<Expression> elements, 
        const std::vector<unsigned int>& _m_dim ) : 
      m_dim(_m_dim) ,  
      m_elements(elements),
      m_indices(_m_dim.size()) {
        if( nElements() != m_elements.size() ){
          ERROR("Dimensions do not match number of m_elements!");
        }
      };
    Tensor( const std::vector<Expression>& elements ) : 
      Tensor( elements, std::vector<unsigned int>({(unsigned int)elements.size()})) {} ; 

    void setName( const std::string& name ){ m_name = name ; }
    std::string name() const { return m_name; }
    /// get an element of the tensor from the index 
    Expression get( const unsigned int& co ) { 
      if( co >= m_elements.size() )
        ERROR( "Element (" + std::to_string(co) + " ) out of range (0" 
            << ", " << m_elements.size() << ")" );
      return (m_elements[co]); 
    }
    Expression get( const unsigned int& co ) const {
      if( co >= m_elements.size() )
        ERROR( "Element (" + std::to_string(co) + " ) out of range (0"
            << ", " << m_elements.size() << ")" );
      return (m_elements[co]);
    }
    std::string to_string() { 
      std::string value = "{";
      for( unsigned int i=0; i < m_dim[0] ; ++i ){
        value += "{";
        for( unsigned int j = 0 ; j < m_dim[1] ; ++j ){
          value += (*this)[{i,j}].to_string() + ( i == m_dim[0] -1 && j == m_dim[1] -1 ? "" : ",") ;
        }
      };
      return value + "}"; 
    };

    unsigned int rank() const { return m_dim.size() ; } 
    int metricSgn(const std::vector<unsigned int>& coordinates ) const {
      int sgn=1;
      for( auto& coord : coordinates ) 
        sgn *= ( coord == 3 ) ? 1 : -1; 
      return sgn;
    }
    int metricSgn( const unsigned int& index ) const {
      return metricSgn( coords(index) ); 
    }
    void append( const Expression& expression ) { m_elements.push_back( expression ); }
    void append( const double& value) { m_elements.push_back( Expression( Constant( value ) )); }
    void append( const std::string& name , bool resolved=true) { m_elements.push_back( Expression( Parameter(name , 0 , resolved ) ) ); }
    Expression get( const std::vector<unsigned int>& _co ) const { return (m_elements[index( _co ) ]); }
    unsigned int size() const { return m_elements.size(); } 
    unsigned int index( const std::vector<unsigned int>& _co ) const {
      unsigned int _index = 0 ;
      unsigned int dproduct = 1;
      for( unsigned int i = 0 ; i < _co.size() ; ++i ){
        _index += _co[i] * dproduct;
        dproduct *= m_dim[i];
      }
      if( _index > nElements() )
        ERROR( "Element (" + std::to_string(_index) +") out of range" );
      return _index; 
    }

    /// get the coordinates of a given index ////
    std::vector<unsigned int> coords( const unsigned int& index ) const { 
      std::vector<unsigned int> returnValue; 
      unsigned int index_temp = index; 
      for( unsigned int j=1; j < m_dim.size()+1; ++j ){
        unsigned int dproduct=1;
        for( unsigned int i = 0 ; i < m_dim.size() -j; ++i ){
          dproduct*=m_dim[i];
        }
        unsigned int val = ( index_temp - ( index_temp % dproduct ) ) /dproduct ; 
        index_temp -= dproduct*val;
        returnValue.push_back(val);
      }
      std::reverse( returnValue.begin(), returnValue.end() );   
      return returnValue;
    } 
    /// get the number of d.o.f.s of a Tensor of given rank and dimension  
    unsigned int nElements() const {
      unsigned int dim=1;
      //std::string dString;
      for( auto& d : m_dim ){ 
        dim *= ( d != 0 ) ? d : 1;
        //  dString += "x" + std::to_string(d);
      }
      //INFO( dim << "    " << dString ) ;
      return dim; 
    }
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
    void print() const {
      INFO( "Dimension of object = " << m_dim.size() << " : ");
      for( auto& x : m_dim) std::cout << x << ", ";
      std::cout << std::endl; 
      if ( m_dim.size() == 0 ) std::cout << m_elements[0].to_string() << std::endl; 
      else if( m_dim.size() == 1 ){
        for( unsigned int x=0;x<m_dim[0];++x) 
          std::cout << m_elements[x].to_string() << std::endl;
      }

      else if( m_dim.size() == 2 ){
        for( unsigned int y=0; y < m_dim[1] ; ++y ){
          for( unsigned int x=0; x < m_dim[0] ; ++x ){
            std::cout << 
              m_elements[ index({x,y}) ].to_string() << "     ";
          }
          std::cout << std::endl; 
        }
      }
      else {
        for( unsigned int i = 0 ; i < m_elements.size(); ++i ){
          INFO( "M(" +vectorToString(coords(i)) + ") = " << m_elements[i].to_string() ); 
        }   

      }
    }

    //// contraction operator of the form A_{abcd}*B_{defg} 
    Tensor operator*( const Tensor& other ); 
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

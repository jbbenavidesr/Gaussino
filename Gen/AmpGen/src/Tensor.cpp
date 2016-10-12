#include "AmpGen/Tensor.h"
#include "AmpGen/Utilities.h"

using namespace AmpGen;

int signed_product( const std::vector<unsigned int>& permutation){

  int product = 1;
  for( unsigned int i = 0 ; i < permutation.size() -1 ; ++i ){
    for( unsigned int j = i+1; j < permutation.size() ; ++j ){
      product*=( (int)permutation[i] - (int)permutation[j] );
    }
  }
  return product;
}

/*
 *  Tensor constructors 
 *
 */


Tensor::Tensor(){}

Tensor::Tensor( const std::vector<Expression>& elements, 
    const std::vector<unsigned int>& _dim ) : 
  m_dim(_dim) ,  
  m_elements(elements),
  m_indices(_dim.size()) 
{
  if( nElements() != m_elements.size() )
    ERROR("Dimensions do not match number of m_elements!");
}

Tensor::Tensor( const std::vector<Expression>& elements ) : 
  Tensor( elements, std::vector<unsigned int>({(unsigned int)elements.size()})) 
{
} 


Tensor::Tensor ( const std::vector<double>& elements, 
    const std::vector<unsigned int>& _dim) : 
    m_dim(_dim),
    m_indices(_dim.size() )
{
  for( auto& x : elements ) append( x );
}

Tensor::Tensor ( const std::vector<std::string>& elements, 
    const std::vector<unsigned int>& _dim, 
    bool  resolved) : m_dim(_dim),
  m_indices(_dim.size() ) 
{
  for( auto& x : elements ) append( x , resolved );
}

/// get an element of the tensor from the index 
Expression Tensor::get( const unsigned int& co ) { 
  if( co >= m_elements.size() )
    ERROR( "Element (" + std::to_string(co) + " ) out of range (0" 
        << ", " << m_elements.size() << ")" );
  return (m_elements[co]); 
}
Expression Tensor::get( const unsigned int& co ) const {
  if( co >= m_elements.size() )
    ERROR( "Element (" + std::to_string(co) + " ) out of range (0"
        << ", " << m_elements.size() << ")" );
  return (m_elements[co]);
}

std::string Tensor::to_string() { 
  std::string value = "{";
  for( unsigned int i=0; i < m_dim[0] ; ++i ){
    value += "{";
    for( unsigned int j = 0 ; j < m_dim[1] ; ++j ){
      value += (*this)[{i,j}].to_string() + ( i == m_dim[0] -1 && j == m_dim[1] -1 ? "" : ",") ;
    }
  };
  return value + "}"; 
}

unsigned int Tensor::rank() const 
{ 
  return m_dim.size() ; 
} 

int Tensor::metricSgn(const std::vector<unsigned int>& coordinates ) const {
  int sgn=1;
  for( auto& coord : coordinates ) 
    sgn *= ( coord == 3 ) ? 1 : -1; 
  return sgn;
}

int Tensor::metricSgn( const unsigned int& index ) const {
  return metricSgn( coords(index) ); 
}

void Tensor::append( const Expression& expression ) 
{ 
  m_elements.push_back( expression ); 
}

void Tensor::append( const double& value) 
{ 
  m_elements.push_back( Expression( Constant( value ) )); 
}
void Tensor::append( const std::string& name , bool resolved) 
{ 
  m_elements.push_back( Expression( Parameter(name , 0 , resolved ) ) ); 
}

Expression Tensor::get( const std::vector<unsigned int>& _co ) const 
{ 
  return (m_elements[index( _co ) ]); 
}

unsigned int Tensor::size() const { return m_elements.size(); } 

unsigned int Tensor::index( const std::vector<unsigned int>& _co ) const {
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
std::vector<unsigned int> Tensor::coords( const unsigned int& index ) const { 
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
unsigned int Tensor::nElements() const {
  unsigned int dim=1;
  //std::string dString;
  for( auto& d : m_dim ){ 
    dim *= ( d != 0 ) ? d : 1;
    //  dString += "x" + std::to_string(d);
  }
  //INFO( dim << "    " << dString ) ;
  return dim; 
}



Tensor AmpGen::operator+( Tensor t1, Tensor t2 ){
  Tensor result(t1.dims());

  if( t1.rank() != t2.rank() ){
    ERROR( "Addition between tensors of different rank makes no sense!" );
    t1.print();
    t2.print();
  }
  if( t1.nElements() != t2.nElements() ){
    ERROR( "Addition between tensors with different number of elements "
        << t1.nElements() << " " << t2.nElements() );
  }

  for( unsigned int i = 0 ; i < t1.nElements(); ++i )
    result[i] = t1[i] + t2[i]  ;

  return result;
}

Tensor AmpGen::operator-( Tensor t1, Tensor t2 ){
  Tensor result(t1.dims());
  if( t1.rank() != t2.rank() ){
    ERROR( "Subtraction between tensors of different rank makes no sense!" );
    t1.print();
    t2.print();
  }
  if( t1.nElements() != t2.nElements() ){
    t1.print();
    t2.print();
    ERROR( "Subtraction between tensors with different number of elements " 
        << t1.nElements() << " " << t2.nElements() );
  }
  for( unsigned int i = 0 ; i < t1.nElements() ; ++i )
    result[i] = t1[i] - t2[i];
  if( result.nElements() != t1.nElements() ||
      result.nElements() != t2.nElements()
    ){
    ERROR("What are you saying?");
  }

  return result;
}


Tensor AmpGen::operator/(Tensor t1, const Expression& t2){
  Tensor result(t1.dims());
  result.setIndices(  t1.indices() ) ;
  for( unsigned int i = 0 ; i < t1.nElements(); ++i ) result[i] = t1[i]/t2 ;

  return result;
}

Tensor AmpGen::operator*(const Expression& other , Tensor t1 ){
  Tensor result(t1.dims());
  result.setIndices( t1.indices() );
  for( unsigned int i = 0 ; i < t1.nElements(); ++i ) result[i] = t1[i]*other ;
  return result;
}

Tensor AmpGen::operator*( Tensor t1, const Expression& other){ return other*t1; }

Tensor AmpGen::operator/(Tensor t1, const double& t2){ return t1/Constant(t2); }
Tensor AmpGen::operator*(const double& other , Tensor t1 ){ return Constant(other)*t1; }
Tensor AmpGen::operator*(Tensor t1, const double& other){ return t1*Constant(other) ; }



Tensor AmpGen::outer_product( Tensor A, Tensor B, 
    std::vector<unsigned int> orderingA, 
    std::vector<unsigned int> orderingB){

  std::vector<unsigned int> rank;
  if( orderingA.size() == 0 && orderingB.size() == 0 ){
    for( unsigned int i = 0 ; i != A.rank(); ++i ) orderingA.push_back( i );
    for( unsigned int i = A.rank() ; i != A.rank() + B.rank(); ++i ) orderingB.push_back(i);
  }
  else if ( orderingA.size() != A.rank() || orderingB.size() != B.rank() ){
    ERROR("Set mapping does not match tensor rank");
    return Tensor( std::vector<double>({0.}), {1} );
  }

  for( auto& r : A.dims() ) rank.push_back( r );
  for( auto& r : B.dims() ) rank.push_back( r );

  Tensor ret( rank );
  for( unsigned int i=0; i < ret.nElements() ; ++i){
    std::vector<unsigned int> coords = ret.coords(i); /// get coordinates of this point ///
    std::vector<unsigned int> A_coords; // ( A.rank() , 0 );
    std::vector<unsigned int> B_coords; // ( B.rank() , 0 );
    for( unsigned int i = 0 ; i != A.rank() ; ++i ) A_coords.push_back( coords[ orderingA[i] ] ); /// this is mighty confusing :S ////
    for( unsigned int i = 0 ; i != B.rank() ; ++i ) B_coords.push_back( coords[ orderingB[i] ] ); 
    ret[i] = A.get( A_coords )*B.get(B_coords ) ;
  }
  return ret; 
}


Expression AmpGen::dot( Tensor A, Tensor B ){

  if( A.rank() != B.rank() || A.nElements() != B.nElements() ){
    std::cout << "Rank(A) = " << A.rank() << " ; Rank(B) = " << B.rank() << std::endl; 
    std::cout << "Elem(A) = " << A.nElements() << " ; Elem(B) = " << B.nElements() << std::endl;
    std::cout << "Error - cannot fully contract unmatched Tensors - you nitwit" << std::endl; 
    return Constant(0);
  }
  Expression result;
  for( unsigned int i = 0 ; i < A.nElements(); ++i ){
    int sgn=A.metricSgn(i);
    if( sgn == 1 ) result = result + A.get(i)*B.get(i);
    else if( sgn == -1 ) result = result - A.get(i) * B.get(i);
    else result + sgn*A.get(i)*B.get(i);
  }
  return result;
}

Tensor AmpGen::Orbital_PWave( Tensor P, Tensor Q){
  return Q - P * ( dot(P,Q) / dot(P,P) ) ;
}

Tensor AmpGen::Orbital_DWave( Tensor P, Tensor Q){

  Tensor V = Orbital_PWave(P,Q);
  Tensor PP = outer_product( P, P )   / dot(P,P); 
  Tensor VV = outer_product( V, V );
  if( PP.rank() != VV.rank() )
    ERROR( "Tensor ranks do not match " );
  if( PP.rank() != Metric4x4.rank() )
    ERROR( "Tensor ranks do not match" );

  return VV + (dot(V,V)/3.)*( PP  - Metric4x4 );
}

Tensor AmpGen::Spin1ProjectionOperator( Tensor P ){
  return Metric4x4 - outer_product( P, P ) /dot(P,P); //  - Metric4x4;
}

Tensor AmpGen::Spin2ProjectionOperator( Tensor P ){
  Tensor sp1 = Spin1ProjectionOperator(P);
  return ( outer_product( sp1, sp1, {0,2},{1,3} ) + outer_product( sp1, sp1 , {0,3},{1,2}) )/2. - outer_product(sp1,sp1)/3.;
}

Tensor AmpGen::LeviCivita( const unsigned int & rank  ){

  std::vector<unsigned int> dim( rank, rank ); 
  std::vector<unsigned int> indices( rank );

  std::iota( indices.begin(), indices.end(), 0 );

  int p0 =  signed_product(indices);

  Tensor result( dim ); /// create tensor of rank N ///

  do{
    unsigned int index = result.index( indices );
    result[index] = signed_product( indices ) / p0;
  } while (std::next_permutation(indices.begin(), indices.end() ) );
  return result; 
}

Tensor Tensor::Eliminate( const unsigned int& row, const unsigned int& d) const {
  std::vector<unsigned int> newDimensions( m_dim );
  newDimensions[d]--;
  Tensor f(newDimensions);
  unsigned int n = nElements();
  for( unsigned int i = 0 ; i < n ; ++i ){
    auto co = coords(i);
    //  INFO("Coordinates of " << i << " = " << co[0] << "  " << co[1] );
    if( co[d] == row ) continue;
    if( co[d] > row ) co[d]--;
    f.m_elements[ f.index( co ) ] = m_elements[i];
  }
  return f;
}

Expression Tensor::Determinant() const {
  Expression result = 0;
  if( m_dim.size() != 2 && m_dim[0] != m_dim[1] ){
    ERROR("Trying to make determinant of rank != 2 or not square");
  }
  if( m_dim[0] == 1 ) return m_elements[0];
  Tensor elim = Eliminate(0,0);
  for( unsigned int i = 0 ; i < m_dim[0] ; ++i){
    Tensor sub=elim.Eliminate(i,1);
    result = result + ((i%2 ? m_elements[index({0,i})] : -m_elements[index({0,i})]))*sub.Determinant();
  }
  return result;
}


//// GAUSS-JORDAN matrix inversion //// 
Tensor Tensor::Invert() const {
  int actualsize = m_dim[0];
  if( rank() != 2 ){
    ERROR("Inversion only implementedfor rank 2 objects");
  };
  Tensor data = *this; 
  for (int i=1; i < actualsize; i++) data[i] = data[i] / data[0] ; // normalize row 0
  //data.print();

  for (int i=1; i < actualsize; i++)  { 
    for (int j=i; j < actualsize; j++)  { // do a column of L
      Expression sum = 0 ; 
      for (int k = 0; k < i; k++)  
        sum = sum + data[j*actualsize+k] * data[k*actualsize+i];
      //   INFO("Subtracting " << sum.to_string() << "   " << data[j*actualsize+i].to_string() );
      data[j*actualsize+i] = data[j*actualsize+i] - sum;
    }
    if (i == actualsize-1) continue;
    for (int j=i+1; j < actualsize; j++)  {  // do a row of U
      Expression sum = 0.0;
      for (int k = 0; k < i; k++)
        sum = sum + data[i*actualsize+k]*data[k*actualsize+j];
      data[i*actualsize+j] = 
        (data[i*actualsize+j]-sum) / data[i*actualsize+i];
    }
  }
  // INFO("LU part : " ) ;
  //data.print();
  for ( int i = 0; i < actualsize; i++ )  // invert L
    for ( int j = i; j < actualsize; j++ )  {
      Expression x = 1.0;
      if ( i != j ) {
        x = 0.0;
        for ( int k = i; k < j; k++ ) 
          x = x - data[j*actualsize+k]*data[k*actualsize+i];
      }
      data[j*actualsize+i] = x / data[j*actualsize+j];
    }
  for ( int i = 0; i < actualsize; i++ )   // invert U
    for ( int j = i; j < actualsize; j++ )  {
      if ( i == j ) continue;
      Expression sum = 0.0;
      for ( int k = i; k < j; k++ )
        sum = sum + data[k*actualsize+j]*( (i==k) ? 1.0 : data[i*actualsize+k] );
      data[i*actualsize+j] = -sum;
    }
  for ( int i = 0; i < actualsize; i++ )   // final inversion
    for ( int j = 0; j < actualsize; j++ )  {
      Expression sum = 0.0;
      for ( int k = ((i>j)?i:j); k < actualsize; k++ )  
        sum = sum + ((j==k)?1.0:data[j*actualsize+k])*data[k*actualsize+i];
      data[j*actualsize+i] = sum;
    }
  return data;
}
/// generic contraction operator /// 
AmpGen::Tensor Tensor::operator | ( AmpGen::Tensor other ){
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
  int size =  nDim() + other.nDim() - 2*contractions.size();
  DEBUG(" multiplying " << nDim() << " x "
      << other.nDim() << " with " 
      << contractions.size() 
      << " contractions (dim = " << size << ")" );
  if(size<0) ERROR("Making an object of negative rank , doesn't make sense");
  Tensor value( std::vector<unsigned int>(size,4));
  unsigned nElem = value.nElements();
  value.m_indices = unsummedIndices; 
  DEBUG( "Size of object = " << value.nElements() );
  for( unsigned int elem = 0 ; elem < nElem ; ++elem ){
    auto coords = value.coords(elem); /// coordinates of this part of the expression /// 
    std::vector<unsigned int> thisCoordinates( nDim() , 0 ), 
      otherCoordinates( other.nDim() , 0);
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
    } while( ++i < nDim() ) ; 

    i=0; 
    j = nDim() - contractions.size() ;
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

    } while( ++i < other.nDim() );
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
}

void Tensor::print() const {
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


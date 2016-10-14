#include "AmpGen/Tensor.h"
#include "AmpGen/Utilities.h"

#include <iostream>
#include <iterator>
#include <algorithm>

using namespace AmpGen ;

TensorHelper AmpGen::operator * ( const TensorHelper& t1, const TensorHelper& t2 ){
  std::vector<std::pair<unsigned int, unsigned int>> contractions; 

  std::vector<LorentzIndex> unsummedIndices;

    
  const std::vector<LorentzIndex>& t1_index = t1.indices();
  const std::vector<LorentzIndex>& t2_index = t2.indices();
  unsigned int t1_size  = t1.indices().size();
  unsigned int t2_size  = t2.indices().size();
  
  const Tensor& t1_tensor  = t1.tensor();
  const Tensor& t2_tensor  = t2.tensor();

  /// handle special scalar cases ///
  for( auto& index : t1_index  ) 
    if( std::find( t2_index.begin(), t2_index.end(), index ) == t2_index.end() ) 
      unsummedIndices.push_back( index );

  for( auto& index : t2_index )
    if( std::find( t1_index.begin(), t1_index.end(), index ) == t1_index.end() ) 
      unsummedIndices.push_back( index );

  for( unsigned int i = 0 ; i < t2_size ; ++i ){
    auto it = 
      std::find( t1_index.begin(), t1_index.end(), t2_index[i] );
    if( it != t1_index.end() ){
      if( t2_index[i].isUpper() != it->isUpper() ){
        contractions.push_back( std::make_pair(std::distance( t1_index.begin(),it),i) );
      }
      else ERROR("Contraction makes no sense!");
    }
  }

  int size = t1_size + t2_size - 2*contractions.size();
  DEBUG("Multiplying " << t1_size << " x "
      << t2_size << " with " 
      << contractions.size() 
      << " contractions (dim = " << size << ")" );
  if(size<0) ERROR("Making an object of negative rank , doesn't make sense");
  Tensor value( std::vector<unsigned int>(size,4));

  unsigned nElem = value.nElements();
  DEBUG( "Size of object = " << value.nElements() );
  for( unsigned int elem = 0 ; elem < nElem ; ++elem ){
    auto coords = value.coords(elem); /// coordinates of this part of the expression /// 
    std::vector<unsigned int>  t1_coords( t1_size , 0 ) ; 
    std::vector<unsigned int>  t2_coords( t2_size , 0);
    unsigned int i = 0 ; 
    unsigned int j = 0 ; 
    do {
      DEBUG("Checking : " << i );
      if( isIn( contractions, i  , [](
              //INFO( "checking : " << i );
              const std::pair<unsigned int, unsigned int>& a, 
              const unsigned int & b ){ return a.first == b ; } ) ){
        DEBUG("Skipping coordinate " << i << " as is being contracted " ); 
        continue;
      }
      else {
        DEBUG("Setting coordinate " << i << " = " << j << " coords.size() = " << coords.size()  );
        t1_coords[i] = coords[j];
        j++;
      }
      //j++;
    } while( ++i < t1_size ) ; 

    i=0; 
    j = t1_size - contractions.size() ;
    do { 
      DEBUG("Checking : " << i );
      if( isIn( contractions, i, [](
              const std::pair<unsigned int, unsigned int>& a,
              const unsigned int & b ){ return a.second == b ; } ) ){
        DEBUG("Skipping coordinate " << i << " as is being contracted " );

      }
      else {
        t2_coords[i]=coords[j];
        DEBUG( "Setting i = " << i << ", (" << j << ")"<< coords[j] );
        j++;
      }

    } while( ++i < t2_size );
    unsigned int sumElements=1;

    for( unsigned int i=0;i<contractions.size(); ++i) 
      sumElements*=t1_tensor.dims()[i];
    DEBUG( "Summing " << sumElements );
    Expression elementExpression=0;
    for( unsigned int i = 0 ; i < sumElements; ++i ){  
      std::vector<unsigned int> contractedCoordinates(contractions.size());
      unsigned int elemI = i;
      for( unsigned int n = 0 ; n < contractions.size() ; ++n ){
        /// I don't for the life of me remember what this does 
        /// But it works, so do NOT touch. Unless its to explain or refactor 
        unsigned int bi = elemI % 4 ;
        elemI = (elemI - bi ) /4;
        contractedCoordinates[n] = bi;
      }
      int sign=1;
      for( unsigned int i = 0 ; i < contractions.size(); ++i ){
        t1_coords[ contractions[i].first ] =   contractedCoordinates[i];
        t2_coords[ contractions[i].second ] = contractedCoordinates[i];
        sign *= ( contractedCoordinates[i] ) == 3 ? 1 : -1; 
      }
      elementExpression = elementExpression + 
        sign * t1_tensor[t1_coords] * t2_tensor[ t2_coords ];  
    }
    value[elem] = elementExpression; 
  }
  DEBUG( "Returning object of rank = " << unsummedIndices.size() );
  return TensorHelper( value, unsummedIndices );
}

TensorHelper AmpGen::operator/( const TensorHelper& t1, const Expression& t2){
  return TensorHelper( t1.tensor() / t2 , t1.indices() );
}
TensorHelper AmpGen::operator*( const Expression& t1, const TensorHelper& t2){
  return TensorHelper( t1 * t2.tensor() , t2.indices() );
}
TensorHelper AmpGen::operator*( const TensorHelper& t1, const Expression& t2){
  return TensorHelper( t1.tensor() * t2 , t1.indices() );
}

TensorHelper AmpGen::operator*( const TensorHelper& t1, const double& t2){
  return TensorHelper( t1.tensor() * Constant( t2 ), t1.indices() );
}
TensorHelper AmpGen::operator*( const double& t2, const TensorHelper& t1){
  return TensorHelper( t1.tensor() * Constant( t2 ), t1.indices() );
}

TensorHelper AmpGen::operator/( const TensorHelper& t1, const double& t2){
  return TensorHelper( t1.tensor() / Constant( t2 ), t1.indices() );
}

TensorHelper AmpGen::operator-( const TensorHelper& t1, const TensorHelper& t2 ){
  return t1 + ( -1 ) * t2; 
};

TensorHelper AmpGen::operator+( const TensorHelper& t1, const TensorHelper& t2 ){

  auto indices_t1 = t1.indices();
  auto indices_t2 = t2.indices();
  auto  tensor_t1 = t1.tensor();
  auto  tensor_t2 = t2.tensor();
  std::vector<unsigned int> t1_to_t2_mapping; 
  for( unsigned int i = 0 ; i < indices_t1.size(); ++i ){
    for( unsigned int j = 0 ; j < indices_t2.size() ; ++j ){
      if ( indices_t1[i] == indices_t2[j] ){
        t1_to_t2_mapping.push_back( j );
        break;
      }
    }
  }
  DEBUG("Index 1 size = " << indices_t1.size() << "  " << indices_t2.size() );
  for( unsigned int i = 0 ; i < t1_to_t2_mapping.size(); ++i){
    DEBUG("Mapping = " << i << " -> " << t1_to_t2_mapping[i] );
  }
  if( t1_to_t2_mapping.size() != indices_t1.size() ){
    ERROR("Mapping illformed!");
    return TensorHelper( Tensor(), {} );
  }
  Tensor result(tensor_t1.dims());

  for( unsigned int i = 0 ; i < tensor_t1.nElements(); ++i ){
    auto t1_coords = tensor_t1.coords(i); 
    /// these are the mapspace coordinates of this element 
    auto t2_coords = std::vector<unsigned int>( t1_coords.size(), 0 );

    for( unsigned int j = 0 ; j < t1_coords.size(); ++j ) 
      t2_coords[j] = t1_coords[ t1_to_t2_mapping[ j ] ] ; 

    result[i] = tensor_t1[i] + tensor_t2[t2_coords];
  }
  return TensorHelper( result , indices_t1 );
}

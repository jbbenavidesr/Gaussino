#ifndef AmpGenUTILS_H
#define AmpGenUTILS_H 1 

#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/ParsedParameterFile.h"
#include "AmpGen/ParsedParameterFileList.h"
#include "AmpGen/NamedParameterBase.h"

static AmpGen::MinuitParameter* tryParameter( const std::vector<std::string>& line, AmpGen::MinuitParameterSet& mps ){
  double min,max,step,value;
  int flag;
  if( line.size() == 4 && isInteger( line[1], flag  ) && isNumber( line[2] , value ) && isNumber( line[3],step ) ){
    return new AmpGen::MinuitParameter( line[0], flag,value,step,0.,0., mps );
  }
  if( line.size() == 6 && isInteger( line[1], flag ) && isNumber( line[2], value ) && isNumber(line[3] , step ) && isNumber(line[4], min) && isNumber(line[5],max) )
    return new AmpGen::MinuitParameter( line[0], flag,value,step,min,max , mps);
  return 0;
}


static AmpGen::MinuitParameterSet MPSFromStream(const std::string& fname="" ){
  AmpGen::MinuitParameterSet mps;
  AmpGen::ParsedParameterFile* ppfl =
    AmpGen::ParsedParameterFileList::getMe()->getParsedFile(std::cin);

  for( auto it = ppfl->begin(); it != ppfl->end(); ++it ){
    tryParameter(it->second.parsedStrings() , mps );
  }
  return mps; 
}

template <class TYPE> 
  std::vector<TYPE> getVectorArgument( const std::string& name, const TYPE& default_value ){
  std::vector<TYPE> return_container;
  unsigned int x=0;
  TYPE obj = default_value; 
  do {
    obj = AmpGen::NamedParameter<TYPE>(name+std::to_string(x++), default_value );
    if( obj != TYPE() ) return_container.push_back( obj );
  } while( obj != default_value );
  return return_container; 
}
#endif

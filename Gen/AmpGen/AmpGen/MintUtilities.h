#ifndef AmpGenUTILS_H
#define AmpGenUTILS_H 1 

#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/ParsedParameterFile.h"
#include "AmpGen/ParsedParameterFileList.h"
#include "AmpGen/NamedParameterBase.h"



static bool isNumber( const std::string& word ){
  bool _isNumber = true;
  std::string::const_iterator k = word.begin();
  _isNumber &= isdigit(*k) || (*k == '.' ) || ( *k == '-' ) ;
  for( k = k+1; k <  word.end(); ++k)
    _isNumber &= isdigit(*k) || (*k == '.');
  return _isNumber;
}

static bool isNumber( const std::string& word , double& number){
  char* p;
  number = strtod( word.c_str(), &p );
  return *p==0;
}


static bool isInteger( const std::string& word ){
  bool _isInteger = true;
  for(std::string::const_iterator k = word.begin(); k != word.end(); ++k)
    _isInteger &= isdigit(*k);
  return _isInteger;
}

static bool isInteger( const std::string& word , int& number ){
  char* p;
  number = strtol(word.c_str(),&p,10);
  return *p==0;
}



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


#endif

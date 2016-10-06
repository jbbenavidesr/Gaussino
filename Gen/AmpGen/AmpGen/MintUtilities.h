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

static bool isInteger( const std::string& word ){
  bool _isInteger = true;
  for(std::string::const_iterator k = word.begin(); k != word.end(); ++k)
    _isInteger &= isdigit(*k);
  return _isInteger;
}


static AmpGen::MinuitParameter* tryParameter( const std::vector<std::string>& line, AmpGen::MinuitParameterSet& mps ){
  // the minimum is a name, a fix flag , a mean, and a step;
  //INFO( line[0] <<  isInteger( line[1] ) << "    " <<  isNumber( line[2] ) << "   " <<  isNumber( line[3] ) );
  if( line.size() == 4 && isInteger( line[1] ) && isNumber( line[2] ) && isNumber( line[3] ) ){
    //INFO("Got Parameter : " << line[0] );
    return new AmpGen::MinuitParameter( line[0], stoi( line[1]), stod(line[2]), stod(line[3]) ,0.,0., mps );
  }
  if( line.size() == 6 && isInteger( line[1] ) && isNumber( line[2] ) && isNumber( line[3] ) && isNumber(line[4]) && isNumber(line[5]) )
    return new AmpGen::MinuitParameter( line[0], stoi(line[1]), stod(line[2]), stod(line[3]), stod(line[4]), stod(line[5]) , mps);
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

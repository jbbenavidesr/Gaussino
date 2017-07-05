#include <cmath>

#include "AmpGen/Utilities.h"
#include <fstream>
#include "AmpGen/MsgService.h"

std::vector<std::string> vectorFromFile( const std::string& filename,
    const char ignoreLinesThatBeginWith
    )
{
  std::vector<std::string> output;
  std::string tmp;
  std::ifstream inFile( filename.c_str() );
  while( inFile.good() )
  {
    std::getline( inFile, tmp );
    if( tmp.size() == 0 || tmp[0] == ignoreLinesThatBeginWith ) continue;
    output.push_back( tmp );
  }
  return output;
}


std::vector<std::vector<unsigned int>> nCr( const unsigned int& n, const unsigned int& r){

  std::vector<bool> mask(n);
  std::vector<std::vector<unsigned int>> combinations;

  std::fill( mask.begin() +r, mask.end(), true);
  do{
    std::vector<unsigned int> perm;
    for( unsigned int i = 0 ; i < n; ++i ){
      if( !mask[i] ) perm.push_back(i);
    }
    combinations.push_back(perm);
  } while (std::next_permutation(mask.begin(), mask.end()) );
  return combinations;
}


std::vector<std::string> split(const std::string &s, char delim, bool ignoreWhitespace) {
  std::vector<std::string> elems;
  std::string item;
  std::stringstream ss(s);
  while (std::getline(ss, item, delim)) {
    if( !ignoreWhitespace || (item != " " && item != "" && item != "\n" && item != "\t"))
      elems.push_back(item);
  }
  return elems;
}

std::vector<size_t> findAll( const std::string& input, const std::string& ch){

  std::vector<size_t > output;
  size_t pos =0;
  do {
    pos = input.find(ch,pos+1);
    if( pos != std::string::npos ) output.push_back(pos);
  } while ( pos != std::string::npos );

  return output;
}

std::map<size_t, std::string > vecFindAll( const std::string & input, const std::vector<std::string>& vCh){

  std::map<size_t, std::string > output;

  for( auto& ch : vCh ){
    auto positions=findAll(input, ch );
    for( auto& ip : positions ) output[ip] = ch;
  }
  return output;
}

std::string replaceAll( const std::string& input, 
    const std::string& toReplace, 
    const std::string& replaceWith){
  size_t pos = 0;
  std::string output=input;
  if( toReplace == replaceWith ){
    ERROR("This will lead to infinite loop and not do anything!");
    return input;
  }
  do{
    pos = output.find(toReplace);
    if( pos != std::string::npos ){
      output.replace(pos,toReplace.length(), replaceWith );
      //       std::cout << pos << " : " << output << "  " << toReplace << " " << replaceWith << std::endl; 
    }
  } while( pos != std::string::npos );
  return output;
}

std::string convertTeXtoROOT( std::string input ){
  input = replaceAll( input, "\\", "#" );
  input = replaceAll( input, "#xspace","");
  input = replaceAll( input, "#kern0.2em#overline{#kern-0.2em","#bar{");
  input = replaceAll( input, "^*","^{*}");
  return input;
}

/// extracts tree structures of the form X{Y,Z,A} 
// where Y and Z and A are also tree elements, by finding
// the matching delimiter and the Z, A elements. 

std::vector<std::string> getItems(const std::string& tree ,
    const std::vector<std::string>& brackets, const std::string& seperator
    ){
  auto braces = vecFindAll( tree, brackets ); /// get a vector of positions of the brackets /// 
  if( braces.size() % 2 != 0 ){
    ERROR( "Unmatched brace in expression - " << tree << " check decay string" );
    return std::vector<std::string>();
  }

  if( braces.size() == 0 ) return std::vector<std::string>({tree});
  std::vector<std::string> items = { tree.substr(0,braces.begin()->first) };
  std::vector<std::pair<size_t,size_t>> matched_braces;
  for( std::map<size_t,std::string>::iterator it = braces.begin(); it != braces.end() ; ++it ){
    const std::string& iType = it->second;
    if( iType != brackets[0] ) continue ;
    int diff = 1;
    for( std::map<size_t,std::string>::iterator jt = it; jt != braces.end(); ++jt){
      if( jt == it ) continue;
      const std::string& jType = jt->second;  /// these are the sub-expressions /// 
      diff += jType == brackets[1] ? -1 : +1;
      if( diff == 0 ){
        matched_braces.push_back( std::pair<size_t,size_t>(it->first, jt->first ));
        break;
      }
    }
  }
  auto commas = findAll( tree, seperator) ; //// commas delimit separate decays;
  std::vector<std::string> daughterTrees;
  size_t begin_position = matched_braces.begin()->first+1;
  for( auto comma : commas ){
    auto braces = matched_braces.begin() + 1;
    for( ; braces != matched_braces.end(); ++braces ){
      if( comma > braces->first && comma < braces->second ) break;
    }
    if( braces == matched_braces.end() ){
      items.push_back( tree.substr( begin_position , comma - begin_position ) );
      begin_position = comma+1;
    }
  }
  items.push_back( tree.substr( begin_position, matched_braces.begin()->second - begin_position ) );
  return items;
}


unsigned int edit_distance(const std::string& s1, const std::string& s2)
{
  const std::size_t len1 = s1.size(), len2 = s2.size();
  std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

  d[0][0] = 0;
  for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
  for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

  for(unsigned int i = 1; i <= len1; ++i)
    for(unsigned int j = 1; j <= len2; ++j)
      // note that std::min({arg1, arg2, arg3}) works only in C++11,
      //                       // for C++98 use std::min(std::min(arg1, arg2), arg3)
      d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
  return d[len1][len2];
}

std::string round(const double& number, const unsigned int& nsf ){
  double value = round( number * pow(10,nsf) ) / pow(10,nsf) ;
  char buffer[20];
  sprintf( buffer, ("%." + std::to_string(nsf)+ "f").c_str() , value );
  //return std::to_string( value / pow(10,nsf) ) ;
  std::string returnValue(buffer);
  return returnValue;
}

std::string numberWithError( const double& number, const double& error, const unsigned int& nDigits ){  
  return round( number, nDigits)  + "\\pm" + round( error*pow(10,nDigits), 0 );
}


bool isNumber( const std::string& word ){
  bool _isNumber = true;
  std::string::const_iterator k = word.begin();
  _isNumber &= isdigit(*k) || (*k == '.' ) || ( *k == '-' ) ;
  for( k = k+1; k <  word.end(); ++k)
    _isNumber &= isdigit(*k) || (*k == '.');
  return _isNumber;
}

bool isNumber( const std::string& word , double& number){
  char* p;
  number = strtod( word.c_str(), &p );
  return *p==0;
}


bool isInteger( const std::string& word ){
  bool _isInteger = true;
  for(std::string::const_iterator k = word.begin(); k != word.end(); ++k)
    _isInteger &= isdigit(*k);
  return _isInteger;
}

bool isInteger( const std::string& word , int& number ){
  char* p;
  number = strtol(word.c_str(),&p,10);
  return *p==0;
}

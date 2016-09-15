#ifndef UHTILS_HH
#define UHTILS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <iostream>
#include "TLorentzVector.h"
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "TMatrixT.h"
#include "TMatrixTSym.h"

namespace AmpGen {

  enum OWNERSHIP{CALLER_PASSES_OWNERSHIP, CALLER_KEEPS_OWNERSHIP};

  template <typename Key, typename Val> 
    const Val &  keyFinder( const Key& k
        , const std::map< Key, Val > & m
        , const Val& dummy
        , bool& successFlag
        ){
      typename std::map< Key , Val >::const_iterator it = m.find(k);
      if(it == m.end()){
        successFlag = false;
        return dummy;
      }else{
        successFlag = true;
        return it->second;
      }
    }

  template <typename Key, typename Val> 
    const Val &  keyFinder( const Key& k
        , const std::map< Key, Val > & m
        , const Val& dummy
        ){
      bool successFlag;
      return keyFinder<Key, Val>(k, m, dummy, successFlag);
    }

  template<typename T>
    void stringToAnything(const std::string& str, T& anything){
      std::stringstream strm(str);
      strm >> anything;
    }

  template<typename T>
    std::string anythingToString(const T& anything){
      std::stringstream strm;
      strm << anything;
      std::string ing;
      strm >> ing;
      return ing;
    }

  template<typename T>
    std::string anyVectorToString(const std::vector<T>& anyV){
      std::stringstream strm;
      for(unsigned int i=0; i<anyV.size(); i++){
        strm << anyV[i];
      }
      std::string ing;
      strm >> ing;
      return ing;
    }

  std::string stringtime(double dt);

  int nearestInt(double f);

  bool A_is_in_B(const std::string& a, const std::string& b);

  std::ostream& operator<<(std::ostream& os, const TLorentzVector& v);

  template<typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
      os << "(";
      for(unsigned int i=0; i<v.size(); i++){
        os << v[i];
        if(i + 1 < v.size()) os << ", ";
      }
      os << ")";
      return os;
    }

  static std::vector<std::string> vectorFromFile( const std::string& filename,
      const char ignoreLinesThatBeginWith='#'
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

  template <class T> static bool isIn( const std::vector<T>& container, const T& obj ){
    for( auto& it : container ) if ( obj == it ) return true;
    return false; 
  }

  template <class T, class B, class F> static bool isIn( const std::vector<T>& container, const B& obj , F f ){
    for( auto& it : container ) if( f( it, obj ) ) return true;
    return false; 
  }

  static std::vector<std::vector<unsigned int>> nCr( const unsigned int& n, const unsigned int& r){

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

  template<class T> static std::string vectorToString( const std::vector<T>& obj ){

    std::string returnValue;
    for( auto& it : obj ) returnValue = returnValue + std::to_string(it);
    return returnValue;
  }

  std::vector<std::string> static split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::string item;
    std::stringstream ss(s);
    while (std::getline(ss, item, delim)) {
      if( item != " " && item != "" && item != "\n" && item != "\t")
        elems.push_back(item);
    }
    return elems;
  }

  static std::vector<size_t> findAll( const std::string& input, const std::string& ch){

    std::vector<size_t > output;
    size_t pos =0;
    do {
      pos = input.find(ch,pos+1);
      if( pos != std::string::npos ) output.push_back(pos);
    } while ( pos != std::string::npos );

    return output;
  }

  static std::map<size_t, std::string > vecFindAll( const std::string & input, const std::vector<std::string>& vCh){

    std::map<size_t, std::string > output;

    for( auto& ch : vCh ){
      auto positions=findAll(input, ch );
      for( auto& ip : positions ) output[ip] = ch;
    }
    return output;
  }

  static std::string replaceAll( const std::string& input, 
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

  static std::string RootTeX( const std::string& input ){ return replaceAll( input, "\\", "#" ); }

  /// extracts tree structures of the form X{Y,Z,A} 
  // where Y and Z and A are also tree elements, by finding
  // the matching delimiter and the Z, A elements. 

  static std::vector<std::string> getItems(const std::string& tree ,
      const std::vector<std::string>& brackets={"{","}"}, const std::string& seperator=","
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


  //// EVENT LOOP TOOLS //// 
  template<int N> struct loopUnroller {
    template<typename F, typename container>
      static inline void get(container& obj, 
          const unsigned int& offset, F f) 
      { 
        f(obj[offset+N]) ; 
        loopUnroller<N-1>::get(obj,offset,f); 
      }
  };

  template<> struct loopUnroller<0>
  {
    template<typename F,typename container>
      static inline void get(container& obj, 
          const unsigned int& offset, F f) 
      { 
        f(obj[offset]); 
      }
  };



  /// unrolls operation F on container container with N things.

  template<int N> struct EventLooper {
    template<typename container, typename F>
      static inline void get ( container& cont, F f){

        unsigned int limit = cont.size() - cont.size() % N;
        unsigned int i=0;
        for(; i != limit ; i+=N){
          loopUnroller<N-1>::get( cont, i, f);
        }
        for(; i != cont.size(); ++i )
          f(cont[i]);
      }
  };



  /// calculate the Levenshtein distance between two strings, useful for checking the 

  static unsigned int edit_distance(const std::string& s1, const std::string& s2)
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

  static std::string round(const double& number, const unsigned int& nsf ){
    double value = round( number * pow(10,nsf) ) / pow(10,nsf) ;
    char buffer[20];
    sprintf( buffer, ("%." + std::to_string(nsf)+ "f").c_str() , value );
    //return std::to_string( value / pow(10,nsf) ) ;
    std::string returnValue(buffer);
    return returnValue;
  }

  static std::string numberWithError( const double& number, const double& error, const unsigned int& nDigits ){  
    return round( number, nDigits)  + "(" + round( error*pow(10,nDigits), 0 ) + ")";
  }
#endif 

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


}


//

#ifndef UTILITIES_H
#define UTILITIES_H
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "AmpGen/MsgService.h"

/*

   Generic utility functions for AmpGen library 
   nCr( n , r ) - Gets all unique (1..N) choose r elements 
   vectorToString<T> - concatenates vector into a string
   findAll(input, ch) - Finds all positions of ch input string input
   vecFindAll ( input, strings ) - Finds positions of elements in strings in input, 
   puts them into map
   getItems - Extract vector of head / branches in a persistified decay tree

*/



template <class T> static bool isIn( const std::vector<T>& container, const T& obj ){
  for( auto& it : container ) if ( obj == it ) return true;
  return false; 
}

template <class T, class B, class F> static bool isIn( const std::vector<T>& container, const B& obj , F f ){
  for( auto& it : container ) if( f( it, obj ) ) return true;
  return false; 
}

template<class T> static std::string vectorToString( const std::vector<T>& obj ){

  std::string returnValue;
  for( auto& it : obj ) returnValue = returnValue + std::to_string(it);
  return returnValue;
}

std::vector<std::vector<unsigned int>> nCr( const unsigned int& n, const unsigned int& r) ; 

std::vector<std::string> vectorFromFile( const std::string& filename,
    const char ignoreLinesThatBeginWith='#' );

std::vector<std::string> split(const std::string &s, char delim, bool ignoreWhitespace=true); 

std::vector<size_t> findAll( const std::string& input, const std::string& ch) ;

std::map<size_t, std::string > vecFindAll( const std::string & input, 
    const std::vector<std::string>& vCh) ;

std::string replaceAll( const std::string& input, 
    const std::string& toReplace, 
    const std::string& replaceWith) ;

std::string convertTeXtoROOT( std::string input );

/// extracts tree structures of the form X{Y,Z,A} 
// where Y and Z and A are also tree elements, by finding
// the matching delimiter and the Z, A elements. 

std::vector<std::string> getItems(const std::string& tree ,
    const std::vector<std::string>& brackets={"{","}"}, 
    const std::string& seperator=",") ;

unsigned int edit_distance(const std::string& s1, const std::string& s2) ; 

std::string round(const double& number, const unsigned int& nsf ) ;

std::string numberWithError( const double& number, const double& error, const unsigned int& nDigits );

bool isNumber( const std::string& word);
bool isNumber( const std::string& word, double& number );
bool isInteger( const std::string& word );
bool isInteger( const std::string& word, int& number );


static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}
// Loop unrolling tools are largely not used anymore in favour of OPENMP

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

#endif 

/*

  MsgService Header 
    -Defines coloured and organised output macro streams using __PRETTY_FUNCTION__
    INFO()    - info level messages, always displayed 
    ERROR()   - error level messages, always displayed
    WARNING() - warning level messages, can be switched with the WARNINGLEVEL flag
    DEBUG()   - debug level messages, can be switched with the DEBUGLEVEL flag
*/


#ifndef MSGSERVICE_H
#define MSGSERVICE_H 1
#include <iostream>

#include <iomanip>

#define WARNINGLEVEL 1
//#define DEBUGLEVEL 0
#define FCNNAMELENGTH 45

inline std::string trimmedString( const std::string& thing, const unsigned int& length = FCNNAMELENGTH ){
  size_t pos = thing.find("(");
  if( pos != std::string::npos ){
    return pos < length ? thing.substr(0,pos) : thing.substr(0,length);
  }
  return thing.size() < length ? thing : thing.substr(0,length)+"...";
  if( thing.size() < length ) return thing;
}

#ifdef DEBUGLEVEL
#define  DEBUG( X )   std::cout << "\033[2;32m" << std::left << std::setw(FCNNAMELENGTH)  \
<< trimmedString(__PRETTY_FUNCTION__)  << "  DEBUG        "    << "\033[0m" << X << "\n"
#else
  #define DEBUG( X ) 
#endif
#define  INFO( X )     std::cout << "\033[2;34m" << std::left << std::setw(FCNNAMELENGTH)  \
<< trimmedString(__PRETTY_FUNCTION__)  << "  INFO         "    << "\033[0m" << X << "\n"
#define  ERROR( X )     std::cout << "\033[1;31m" << std::left << std::setw(FCNNAMELENGTH)   \
<<  trimmedString(__PRETTY_FUNCTION__) << "  ERROR        "    << "\033[0m" << X << "\n"
#if WARNINGLEVEL
#define  WARNING( X )     std::cout << "\033[1;35m" << std::left << std::setw(FCNNAMELENGTH) \
<<  trimmedString(__PRETTY_FUNCTION__) << "  WARNING      "    << "\033[0m" << X << "\n"
#else 
 #define WARNING
#endif

#endif

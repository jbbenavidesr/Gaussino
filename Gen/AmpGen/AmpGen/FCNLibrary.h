#ifndef hFCNLIB
#define hFCNLIB

#include "CompiledExpression.h"
#include "MsgService.h"

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <dlfcn.h>

#include <stdlib.h>

//// FIXED : FCNLibrary can now handle multiple types /////

namespace AmpGen { 
  class FCNLibrary  {
    private:
      std::string m_filename; 
      void* m_handle; 
      std::map<unsigned int, ICompiledExpression* > m_objects;
      typedef typename std::map<unsigned int, ICompiledExpression* >::iterator iterator;
      typedef typename std::map<unsigned int, ICompiledExpression* >::const_iterator const_iterator;
      std::vector<std::string> m_includes; 
      std::string generateFilename(){
        char buffer[] = "/tmp/libAmpGen-XXXXXX";
        mkstemp( buffer );
        return buffer; 
      }

    public: 
      enum OPTIONS {
        DEBUG = (1<<0),
        RECOMPILE = (1<<1)
      };

      FCNLibrary() : m_filename(), m_includes({"complex","iostream","math.h"}) {};
      const_iterator cbegin() const { return m_objects.cbegin(); }
      const_iterator cend() const { return m_objects.cend(); }
      iterator begin() { return m_objects.begin(); }
      iterator end() { return m_objects.end(); }

      void add( ICompiledExpression* expression ); 
      void addInclude( const std::string& header );
      template <class RETURN_TYPE> 
        void add ( std::vector<CompiledExpression<RETURN_TYPE>>& expressions ){
          for( auto& expression : expressions ) 
            add( &expression );
        }
      void merge( const FCNLibrary& other ) ;

      virtual void generateSourceCode( const std::string& filename , bool debug=false);
      virtual bool compile(const std::string& name="", bool debug=false) ;

      bool link( const unsigned int& options=FCNLibrary::OPTIONS::RECOMPILE,
          const std::string& name="") ;
      bool isReady() const {
        for( auto& expression : m_objects ){
          if( ! expression.second->isReady() ){
            ERROR( expression.second->name() << " not linked" );
            return false;
          }
        }
        return true;
      }
  };
}
#endif

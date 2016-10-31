#ifndef hFCNLIB
#define hFCNLIB

#include "CompiledExpression.h"
#include "MsgService.h"

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <dlfcn.h>

#include <stdlib.h>

/// at the moment, a FCN library can only return one type, this is a bit daft, will look to improve ///

namespace AmpGen { 
  template <class RETURN_TYPE>
    class FCNLibrary  {
      private:
        std::string m_filename; 
        void* m_handle; 
        std::map<unsigned int, CompiledExpression<RETURN_TYPE>* > m_objects;
        typedef typename std::map<unsigned int, CompiledExpression<RETURN_TYPE>* >::iterator it;
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
        it begin(){ return m_objects.begin(); }
        it end(){ return m_objects.end(); }

        void add( CompiledExpression<RETURN_TYPE>* expression ){
          auto it = m_objects.find(expression->hash() );
          if( it == m_objects.end() ) {
            m_objects[ expression->hash() ] = expression; 
            DEBUG( "Added " << expression->name() << " to FCN library" );
          }
          else if( it->second->name() != expression->name() ){
            WARNING("Hash Collision between " << it->second->name() << " and " << expression->name() );
          }
        }
        void addInclude( const std::string& header ){ m_includes.push_back( header ) ; } 
        void add ( std::vector<CompiledExpression<RETURN_TYPE>>& expressions ){
          for( auto& expression : expressions ) 
            add( &expression );
        }
        void merge( FCNLibrary<RETURN_TYPE>& other ){
          for( auto& x = other.begin(); x != other.end(); ++x ) add( x );
        }

        virtual void generateSourceCode( const std::string& filename , bool debug=false){
          std::ofstream output( (filename+".cpp").c_str() );
          for( auto& include : m_includes )
            output << "#include <" << include << ">" << std::endl;
          for( auto& expression : m_objects )
            expression.second->compile(output, debug );
          output.close();
        };
        virtual bool compile(const std::string& name="", bool debug=false){
          if( m_filename == "" )
            m_filename = name == "" ? generateFilename() : name ;
          generateSourceCode( m_filename , debug ); 
          INFO("Building shared library ... " << m_filename );
          const std::string buildCommand = 
            "g++ -Ofast -shared -rdynamic -fPIC "+m_filename+".cpp -o " + m_filename + ".so";
          if( system(buildCommand.c_str()) != 0){
            ERROR("Compilation failed"); 
            return false;
          }
          return true;  
        } 

        bool link( const unsigned int& options=FCNLibrary<RETURN_TYPE>::OPTIONS::RECOMPILE,
            const std::string& name=""){
          if( m_filename == "" )
            m_filename = name == "" ? generateFilename() : name;

          INFO( "Linking shared library " << m_filename );
          m_handle = dlopen((m_filename+".so").c_str(),RTLD_NOW);
          bool dbThis = options & FCNLibrary<RETURN_TYPE>::OPTIONS::DEBUG ; 
          bool autorebuild = options & FCNLibrary<RETURN_TYPE>::OPTIONS::RECOMPILE ; 
          unsigned int newOptions = dbThis ? FCNLibrary<RETURN_TYPE>::OPTIONS::DEBUG : 0;
          if( !m_handle ){
            WARNING( dlerror() );
            if( autorebuild ){
              return FCNLibrary<RETURN_TYPE>::compile(m_filename, dbThis ) &&  
                link(newOptions,m_filename);
            }
            else return false;
          }
          bool success = true;
          INFO("Got handle - linking " << m_objects.size() << " expressions");
          for( auto& expression : m_objects ){
            bool thisLinked = expression.second->link(m_handle,dbThis);
            success &= thisLinked;
            if( !thisLinked )
              ERROR("Symbol:" << expression.second->name() << " linking failed");
          }
          if( !success && autorebuild ){
            WARNING("Linking fails - recompiling the dynamic library");
            dlclose(m_handle);
            return FCNLibrary<RETURN_TYPE>::compile(m_filename, dbThis ) && link(newOptions,m_filename);
          }
          return success;
        }
    };
} 

#endif

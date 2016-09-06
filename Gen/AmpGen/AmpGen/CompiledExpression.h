#ifndef COMPILED_EXPRESSION_H
#define COMPILED_EXPRESSION_H
/***********************************************************************
  CompiledExpression<TYPE, RETURN_TYPE>

  
  - Takes a class (TYPE) that implements getExpression( ostream* ) and fcnName()
    and generates compiled code with this name from the expresssion tree. 
  - Then, can be dynamically linked and evaluated getVal( event )
  - Can also just link with a dynamic library handle.
  - Also stores "external" parameters to the fcn

  T.Evans 10/15


 ***********************************************************************/


#include "AmpGen/Expression.h"
#include "AmpGen/MsgService.h"

#include <cxxabi.h> /// for unpickling return type names ///
#include <dlfcn.h>

namespace AmpGen { 
template <class RETURN_TYPE> class CompiledExpression {

  private:
  Expression m_obj;
  std::vector<DBSYMBOL> m_db;
  std::string m_name;
  uint64_t m_hash;
  RETURN_TYPE (*m_fcn)(const double*, const double*); /// decay evaluation ///
  void (*m_fdb)(const double*, const double*); /// debug evaluation ///
  std::vector<double> m_externals;
  std::vector<std::string> m_externalNames;
  std::map<std::string, std::pair<unsigned int, double> > m_addressMapping; 
  bool m_hasExternalsChanged;
  public: 
  unsigned int hash() const { 
    return m_hash;
  }
  unsigned int FNV1a_hash() { //// implements FNV-1a hash function //// 
    unsigned int hash=2166136261;
    for( auto& c : m_name ){
      hash ^= c ;
      hash *= 16777619;
    }
    return hash;
  }
  std::map<std::string, std::pair< unsigned int, double> > 
    getAddressMapping() const { return m_addressMapping ; }
  void setExternals(const std::vector<double>& external ){ m_externals = external ; } 
  double getExternal( const std::string& name ){ return m_externals[ m_addressMapping[name].first ] ; } 
  unsigned int getNParams() const { return m_externalNames.size() ; } 
  const std::string& getExternalName( const unsigned int& index ) const { return m_externalNames[index] ; } 
  std::map<std::string, std::pair<unsigned int, double> > addressMapping() const { 
   return m_addressMapping ; 
  } 
  void setToDefaults(){
    for( unsigned int i = 0 ; i < m_externals.size(); ++i) 
      m_externals[i] = m_addressMapping[ m_externalNames[i] ].second;
  };
  void print(){
    INFO( "Name     = " << m_name );
    INFO( "Hash     = " << hash() );
    for( auto& m : m_addressMapping ) 
      INFO( m.first << "  (" << m.second.first << ") = " << m.second.second );
    for(unsigned int i = 0 ; i < m_externals.size(); ++i)
      INFO( "E[" << i << "] (" << m_externalNames[i]  << ") = " << m_externals[i] );
  };
  void setExternal( const double& value, const unsigned int& address){ 
    if( m_externals[address] == value ) return;
    m_externals[address] = value; 
    m_hasExternalsChanged = true ;
  } 
  bool hasExternalsChanged(){
    return m_hasExternalsChanged;
  }
  void resetExternals(){ m_hasExternalsChanged = false ; }

  std::string name() const {return m_name;}
  CompiledExpression( const Expression& expression , 
                      const std::string& name ,
                      const std::map<std::string,unsigned int>& m_evtMapping = std::map<std::string,unsigned int>(), 
                      std::vector<DBSYMBOL>* db=0) : 
    m_obj( expression ) , 
    m_name(name), 
    m_hash(FNV1a_hash()) , 
    m_fcn(NULL),
    m_hasExternalsChanged(false)  {
   
    if( m_evtMapping.size() != 0 ) m_obj.resolveEventMapping( m_evtMapping );
    m_obj.resolveDependencies( m_addressMapping );
    if( db != 0 ) for( auto& sym : *db ){
      m_db.push_back( sym );
      m_db.rbegin()->second.resolveDependencies( m_addressMapping );
      m_db.rbegin()->second.resolveEventMapping( m_evtMapping );
    }
    m_externals.resize( m_addressMapping.size() );
    m_externalNames.resize( m_addressMapping.size());
    for( auto& variable : m_addressMapping ){
      m_externalNames[variable.second.first] = variable.first;
    }
  };
  
  CompiledExpression( const std::string& name ) : m_name(name ), m_hash( FNV1a_hash() ), m_fcn(0) {};

  Expression& expression(){ return m_obj; }
  void compile( std::ostream& stream , bool debug=false ) const {
    int status=0;
    INFO( "Compiling: " << name() << " with " << m_db.size() << " debug symbols");
    const std::string retName = abi::__cxa_demangle(typeid(RETURN_TYPE).name(), 0, 0, &status);
    m_obj.compile( stream, "p"+std::to_string(hash()) , retName , "d" );
    if( m_db.size() != 0 && debug ) DEBUG_SYMBOLS( "p"+std::to_string(hash()) , m_db, stream );
  }

  void compileWithParameters( std::ostream& stream ) const {
    int status=0;
    const std::string retName = abi::__cxa_demangle(typeid(RETURN_TYPE).name() , 0, 0, &status );
   // compile(stream, false );
    INFO("Compiling " << name() << " = " << hash() );
    stream << "extern \"C\" " << retName.c_str() << " r"+std::to_string(hash()) << "( double* E ){" << std::endl; 
    stream << "double externalParameters [] = {";
    if( m_externals.size() != 0 ){
    for( unsigned int i = 0 ; i < m_externals.size() - 1 ; ++i){
   
      stream << m_externals[i] << ", // " << m_externalNames[i] << std::endl;
    }; 
    
      stream << m_externals[m_externals.size()-1] << " }; // " << m_externalNames[m_externalNames.size()-1] << std::endl; 
    }
    else stream << "0};" << std::endl; 
    stream << " return p"<<hash() << "( E , externalParameters ); }" << std::endl;
  };

  bool isReady() const { return m_fcn != 0 ; } 
  RETURN_TYPE getVal( const double* event) const {
    return (*m_fcn)( event, &(m_externals[0]) );
  }
  void debug( const double* event) const {
    if( !isReady() ){
      ERROR("Function " << name() << " not linked");
    }
    if( m_fdb == 0 ){
      ERROR("Function" << name() << " debugging symbols not linked");
    }
    return (*m_fdb)( event, &(m_externals[0]));
  }
  bool link( void* handle , bool dbThis=false ){
    INFO( "Linking " << m_name << (dbThis?" (debugging)":"") );
    const std::string symbol = "p"+std::to_string(hash());
    dlerror();
    m_fcn = (RETURN_TYPE(*)(const double*, const double* ))
      dlsym( handle, symbol.c_str() );
    if( m_fcn == NULL ){
      ERROR( dlerror() ); 
      ERROR( m_name << " (symbol = " << symbol <<") linking fails");
      return false;
    }
    if( dbThis ){
      const std::string dbsymbol = symbol+"_DB";
      m_fdb = (void(*)(const double*, const double*))
        dlsym( handle, dbsymbol.c_str() );
      if( m_fdb == NULL ){
        ERROR("Linking of " << m_name << " symbol = " << dbsymbol << ")");
        return false;
      }
    }
    if( m_fcn == NULL ){
      ERROR( "Linking of " << m_name << " failed (hash = " << hash() << ")" );
      return false;
    } 
    return true;
  }
}; } 

#endif 

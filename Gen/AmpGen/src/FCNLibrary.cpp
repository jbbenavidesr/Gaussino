#include "AmpGen/FCNLibrary.h"
#include "AmpGen/MsgService.h"

using namespace AmpGen;



void FCNLibrary::add( ICompiledExpression* expression ){
  auto it = m_objects.find(expression->hash() );
  if( it == m_objects.end() ) {
    m_objects[ expression->hash() ] = expression; 
    DEBUG( "Added " << expression->name() << " to FCN library" );
  }
  else if( it->second->name() != expression->name() ){
    WARNING("Hash Collision between " << it->second->name() << " and " << expression->name() );
  }
}

void FCNLibrary::addInclude( const std::string& header ){ 
  m_includes.push_back( header ) ; 
} 

void FCNLibrary::merge( const FCNLibrary& other ){
  for( auto x = other.cbegin(); x != other.cend(); ++x ) add( x->second );
}

void FCNLibrary::generateSourceCode( const std::string& filename, bool debug){
  std::ofstream output( (filename+".cpp").c_str() );
  for( auto& include : m_includes )
    output << "#include <" << include << ">" << std::endl;
  for( auto& expression : m_objects )
    expression.second->compile(output, debug );
  output.close();
}

bool FCNLibrary::compile(const std::string& name, bool debug){
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

bool FCNLibrary::link( const unsigned int& options,const std::string& name){
  if( m_filename == "" )
    m_filename = name == "" ? generateFilename() : name;

  INFO( "Linking shared library " << m_filename );
  m_handle = dlopen((m_filename+".so").c_str(),RTLD_NOW);
  bool dbThis = options & FCNLibrary::OPTIONS::DEBUG ; 
  bool autorebuild = options & FCNLibrary::OPTIONS::RECOMPILE ; 
  unsigned int newOptions = dbThis ? FCNLibrary::OPTIONS::DEBUG : 0;
  if( !m_handle ){
    WARNING( dlerror() );
    if( autorebuild ){
      return FCNLibrary::compile(m_filename, dbThis ) &&  
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
    return FCNLibrary::compile(m_filename, dbThis ) && link(newOptions,m_filename);
  }
  return success;
}


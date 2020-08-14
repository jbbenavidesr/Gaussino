#pragma once

#include <sstream>
#include <string>
#include <thread>
#include <map>

class GiGaMessage;
// Basic interface for wrapping some external messaging service
// into the G4 parts of GiGa without depending on it directly,
// e.g. Gaudi MsgSvc.

class IGiGaMessage
{
  friend class GiGaMessage;

protected:
  IGiGaMessage()          = default;
  virtual ~IGiGaMessage() = default;

  virtual void debug( std::string message ) const   = 0;
  virtual void verbose( std::string message ) const = 0;
  virtual void error( std::string message ) const   = 0;
  virtual void warning( std::string message ) const = 0;
  virtual void info( std::string message ) const = 0;
  virtual void always( std::string message ) const = 0;
  virtual int level( ) const {return -1;}
};

// Baseclass enabling the use of the messaging interface within implementation classes
// of G4 objects

class GiGaMessage
{
public:
  GiGaMessage() = default;
  virtual ~GiGaMessage()
  {
    if ( m_msg ) {
      delete m_msg;
    }
  }
  void SetMessageInterface( const IGiGaMessage* msg ) { m_msg = msg; }
  GiGaMessage( GiGaMessage&& right )
  {
    m_msg       = right.m_msg;
    right.m_msg = nullptr;
  }
  static thread_local std::string NameTag;

protected:

  void debug( std::string message ) const
  {
    if ( !m_msg || !printDebug()) return;
    auto toprint = std::string{"[ "}.append(NameTag).append(" ] ");
    m_msg->debug(toprint.append( message));
  }
  void verbose( std::string message ) const
  {
    if ( !m_msg || !printVerbose()) return;
    auto toprint = std::string{"[ "}.append(NameTag).append(" ] ");
    m_msg->verbose(toprint.append( message));
  }
  void error( std::string message, unsigned int mx = 0 ) const
  {
    if ( !m_msg ) return;
    auto toprint = std::string{"[ "}.append(NameTag).append(" ] ");
    if(mx > 0){
      const size_t num = increment( m_errors, message );
      if(num > mx){
        return;
      } else if (num == mx){
        m_msg->error(toprint.append("The ERROR message is suppressed : '").append( message).append( "'" ));
        return;
      }
    }
    m_msg->error(toprint.append( message));
  }
  void warning( std::string message, unsigned int mx = 0 ) const
  {
    if ( !m_msg ) return;
    auto toprint = std::string{"[ "}.append(NameTag).append(" ] ");
    if(mx > 0){
      const size_t num = increment( m_warnings, message );
      if(num > mx){
        return;
      } else if (num == mx){
        m_msg->warning(toprint.append("The WARNING message is suppressed : '").append( message).append( "'" ));
        return;
      }
    }
    m_msg->warning(toprint.append( message));
  }
  void info( std::string message, unsigned int mx = 0 ) const
  {
    if ( !m_msg ) return;
    auto toprint = std::string{"[ "}.append(NameTag).append(" ] ");
    if(mx > 0){
      const size_t num = increment( m_infos, message );
      if(num > mx){
        return;
      } else if (num == mx){
        m_msg->info(toprint.append("The INFO message is suppressed : '").append( message).append( "'" ));
        return;
      }
    }
    m_msg->info(toprint.append( message));
  }
  void always( std::string message ) const
  {
    if ( !m_msg ) return;
    auto toprint = std::string{"[ "}.append(NameTag).append(" ] ");
    m_msg->always(toprint.append( message));
  }
  int MessageInterfacelevel() const {return m_msg->level();}
  bool printVerbose() const {return m_msg->level() <= 1;}
  bool printDebug() const {return m_msg->level() <= 2;}
  bool printInfo() const {return m_msg->level() <= 3;}

private:
  typedef std::map<std::string, unsigned int, std::less<>> Counter;
  static unsigned int increment( Counter& c, std::string_view which ) {
    auto i = c.find( which );
    return i != c.end() ? ++( i->second ) : c.emplace( which, 1 ).first->second;
  }

  /// Counter of errors
  mutable Counter m_errors;
  /// counter of warnings
  mutable Counter m_warnings;
  /// counter of infos
  mutable Counter m_infos;

  const IGiGaMessage* m_msg = nullptr;
};

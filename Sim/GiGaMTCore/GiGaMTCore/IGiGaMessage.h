#pragma once

#include <sstream>
#include <string>
#include <thread>

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

protected:
  void debug( std::string message ) const
  {
    if ( !m_msg || !printDebug()) return;
    std::stringstream ss;
    ss << "[ Thread " << std::this_thread::get_id() << " ] " << message;
    m_msg->debug( ss.str() );
  }
  void verbose( std::string message ) const
  {
    if ( !m_msg || !printVerbose()) return;
    std::stringstream ss;
    ss << "[ Thread " << std::this_thread::get_id() << " ] " << message;
    m_msg->verbose( ss.str() );
  }
  void error( std::string message ) const
  {
    if ( !m_msg ) return;
    std::stringstream ss;
    ss << "[ Thread " << std::this_thread::get_id() << " ] " << message;
    m_msg->error( ss.str() );
  }
  void warning( std::string message ) const
  {
    if ( !m_msg ) return;
    std::stringstream ss;
    ss << "[ Thread " << std::this_thread::get_id() << " ] " << message;
    m_msg->warning( ss.str() );
  }
  void info( std::string message ) const
  {
    if ( !m_msg ) return;
    std::stringstream ss;
    ss << "[ Thread " << std::this_thread::get_id() << " ] " << message;
    m_msg->info( ss.str() );
  }
  int MessageInterfacelevel() const {return m_msg->level();}
  bool printVerbose() const {return m_msg->level() <= 1;}
  bool printDebug() const {return m_msg->level() <= 2;}
  bool printInfo() const {return m_msg->level() <= 3;}

private:
  const IGiGaMessage* m_msg = nullptr;
};

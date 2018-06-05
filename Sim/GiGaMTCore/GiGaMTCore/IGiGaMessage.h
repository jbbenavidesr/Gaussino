#pragma once

#include <string>

class GiGaMessage;
// Basic interface for wrapping some external messaging service
// into the G4 parts of GiGa without depending on it directly,
// e.g. Gaudi MsgSvc.

class IGiGaMessage
{
  friend class GiGaMessage;

protected:
  IGiGaMessage() = default;

  virtual void debug( std::string message )   const = 0;
  virtual void verbose( std::string message ) const = 0;
  virtual void error( std::string message )   const = 0;
  virtual void warning( std::string message ) const = 0;
};

// Baseclass enabling the use of the messaging interface within implementation classes
// of G4 objects

class GiGaMessage
{
public:
  GiGaMessage() = default;
  virtual ~GiGaMessage() = default;
  void SetMessageInterface( const IGiGaMessage* msg ) { m_msg = msg; }

protected:
  void debug( std::string message )
  {
    if ( m_msg ) m_msg->debug( message );
  }
  void verbose( std::string message )
  {
    if ( m_msg ) m_msg->verbose( message );
  }
  void error( std::string message )
  {
    if ( m_msg ) m_msg->error( message );
  }
  void warning( std::string message )
  {
    if ( m_msg ) m_msg->warning( message );
  }

private:
  const IGiGaMessage* m_msg = nullptr;
};

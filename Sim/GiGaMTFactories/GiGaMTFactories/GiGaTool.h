#pragma once

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GiGaMTCore/IGiGaMessage.h"

/** GiGaFactoryBase
 *
 *  Base class for all factories of G4 objects which implements
 *  the IGiGaMessage interface to allow the use of the Gaudi
 *  messaging facilities from the factory within the G4 objects.
 *  TODO: That might actually be stupid but whatever ...
 *
 *  @author Dominik Muller
 *  @date   2018-06-04
 */

class GiGaMessageImpl : public IGiGaMessage
{
  friend class GiGaTool;

  GiGaMessageImpl() = delete;
  GiGaMessageImpl( MsgStream _stream ) : msg( _stream ) {}

public:
  void debug( std::string message ) const override { msg << MSG::DEBUG << message << endmsg; }
  void verbose( std::string message ) const override { msg << MSG::VERBOSE << message << endmsg; }
  void error( std::string message ) const override { msg << MSG::ERROR << message << endmsg; }
  void warning( std::string message ) const override { msg << message << endmsg; }

private:
  mutable MsgStream msg;
};

class GiGaTool : public GaudiTool
{

public:
  using GaudiTool::GaudiTool;
  virtual ~GiGaTool(){};

  GiGaMessageImpl* message_interface() const
  {
    MsgStream msg( msgSvc(), name() );
    msg.setLevel( msgLevel() );
    auto msgwrapper = new GiGaMessageImpl( msg );
    return msgwrapper;
  }
};

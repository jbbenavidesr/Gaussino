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

template <typename T>
class GiGaFactoryBase : public GaudiTool, public IGiGaMessage
{
public:
  using GaudiTool::GaudiTool;

  virtual T* construct() const = 0;

protected:
  // GiGaFactoryBase( const std::string & type , const std::string & name ,
  // const IInterface * parent ) ;

  // Get in the normal messaging things
  using GaudiTool::debug;
  using GaudiTool::error;
  using GaudiTool::verbose;
  using GaudiTool::warning;

  virtual ~GiGaFactoryBase();

  void debug( std::string message ) const override
  {
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << message << endmsg;
    }
  }
  void verbose( std::string message ) const override
  {
    if ( msgLevel( MSG::VERBOSE ) ) {
      verbose() << message << endmsg;
    }
  }
  void error( std::string message ) const override { error() << message << endmsg; }
  void warning( std::string message ) const override { warning() << message << endmsg; }
};

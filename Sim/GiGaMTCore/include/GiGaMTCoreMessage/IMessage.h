/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include <map>
#include <sstream>
#include <string>
#include <thread>

namespace Gsino {
  // Basic interface for wrapping some external messaging service
  // into the G4 parts of GiGa without depending on it directly,
  // e.g. Gaudi MsgSvc.
  class IMessage {

  public:
    virtual ~IMessage()                                     = default;
    virtual void       debug( std::string message ) const   = 0;
    virtual void       verbose( std::string message ) const = 0;
    virtual void       error( std::string message ) const   = 0;
    virtual void       warning( std::string message ) const = 0;
    virtual void       info( std::string message ) const    = 0;
    virtual void       always( std::string message ) const  = 0;
    inline virtual int level() const { return -1; }
  };

  // Baseclass enabling the use of the messaging interface within implementation classes
  // of G4 objects
  class Message {
  public:
    Message() = default;
    ~Message();
    Message( Message&& );
    inline void SetMessageInterface( const IMessage* msg ) { m_msg = msg; };

    static thread_local std::string NameTag;

    void debug( std::string ) const;
    void verbose( std::string ) const;
    void error( std::string, unsigned int mx = 0 ) const;
    void warning( std::string, unsigned int mx = 0 ) const;
    void info( std::string, unsigned int mx = 0 ) const;
    void always( std::string ) const;

    inline int MessageInterfacelevel() const { return m_msg->level(); }

    inline bool printVerbose() const { return m_msg->level() <= 1; }
    inline bool printDebug() const { return m_msg->level() <= 2; }
    inline bool printInfo() const { return m_msg->level() <= 3; }

  private:
    using Counter = std::map<std::string, unsigned int, std::less<>>;
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

    const IMessage* m_msg = nullptr;
  };
} // namespace Gsino

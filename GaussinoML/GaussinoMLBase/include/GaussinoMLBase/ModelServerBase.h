/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include <mutex>

// Gaudi
#include "GaudiKernel/MsgStream.h"

namespace Gsino::ML {

  template <typename Test, template <typename...> class Ref>
  struct is_specialization : std::false_type {};

  template <template <typename...> class Ref, typename... Args>
  struct is_specialization<Ref<Args...>, Ref> : std::true_type {};
  struct ModelServerBase {
    ModelServerBase( const std::string& name = "Unspecified" );
    ModelServerBase( const ModelServerBase& ) = delete;

    void print( const std::string&, MSG::Level ) const;

    void       setLevel( MSG::Level );
    MSG::Level level() const;

    void       setMsgStream( std::unique_ptr<MsgStream> );
    MsgStream* msgStream();

  protected:
    MSG::Level                 m_lvl = MSG::Level::INFO;
    std::string                m_name;
    std::unique_ptr<MsgStream> m_stream;

  private:
    const char*        levelName( MSG::Level ) const;
    mutable std::mutex m_printingLock;
  };
} // namespace Gsino::ML

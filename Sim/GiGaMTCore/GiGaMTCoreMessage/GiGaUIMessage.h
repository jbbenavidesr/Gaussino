/*****************************************************************************\
* (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#pragma once

// Gaussino
#include "GiGaMTCoreMessage/IGiGaMessage.h"

/**
 * @class GiGa::UIMessage
 *
 * Extends standard GiGaMessage with additional method that calls the UI
 * manager and parses the status of the command correctly.
 *
 *  @author Michał Mazurek
 *  @date   28/07/2022
 */

namespace GiGa {
  class UIMessage : public Message {
  public:
    using Message::Message;
    void applyUIcommand( std::string command ) const;
  };
} // namespace GiGa

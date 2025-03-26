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

// G4
#include "G4UIcommandStatus.hh"
#include "G4UImanager.hh"

// Gaussino
#include "GiGaMTCoreMessage/UIMessage.h"

void Gsino::UIMessage::applyUIcommand( std::string command ) const {
  auto ui = G4UImanager::GetUIpointer();

  if ( !ui ) {
    std::string msg = "G4UImanager* points to NULL!";
    error( msg );
    throw std::runtime_error( msg );
  }

  auto status = ui->ApplyCommand( command );
  switch ( status ) {
  case G4UIcommandStatus::fCommandSucceeded:
    debug( "UICommand succeeded." );
    return;
  case G4UIcommandStatus::fCommandNotFound:
    error( "UICommand not found." );
    return;
  case fIllegalApplicationState:
    error( "Illegal application state." );
    return;
  default:
    error( "Illegal parmeter (" + std::to_string( status % 100 ) + ")" );
  }
}

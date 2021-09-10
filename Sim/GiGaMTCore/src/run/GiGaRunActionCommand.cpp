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
#include "GiGaMTCoreRun/GiGaRunActionCommand.h"
#include "Geant4/G4UImanager.hh"

// ============================================================================
/** performe the action at the begin of each run
 *  @param run pointer to Geant4 run object
 */
// ============================================================================
void GiGaRunActionCommand::BeginOfRunAction( const G4Run* run )
{
  if ( 0 == run ) {
    warning( "BeginOfRunAction:: G4Run* points to NULL!" );
  }
  /// get Geant4 UI manager
  G4UImanager* ui = G4UImanager::GetUIpointer();
  if ( 0 == ui ) {
    error( "BeginOfRunAction:: G4UImanager* points to NULL!" );
    return;
  } else {
    for ( auto& command : m_beginCmds ) {
      info( "BeginOfRunAction(): execute '" + command + "'" );
      ui->ApplyCommand( command );
    }
  }
}

// ============================================================================
/** performe the action at the end of each run
 *  @param run pointer to Geant4 run object
 */
// ============================================================================
void GiGaRunActionCommand::EndOfRunAction( const G4Run* run )
{
  if ( 0 == run ) {
    warning( "EndOfRunAction:: G4Run* points to NULL!" );
  }
  /// get Geant4 UI manager
  G4UImanager* ui = G4UImanager::GetUIpointer();
  if ( 0 == ui ) {
    error( "EndOfRunAction:: G4UImanager* points to NULL!" );
  } else {
    for ( auto& command : m_endCmds ) {
      info( "EndOfRunAction(): execute '" + command + "'" );

      ui->ApplyCommand( command );
    }
  }
}

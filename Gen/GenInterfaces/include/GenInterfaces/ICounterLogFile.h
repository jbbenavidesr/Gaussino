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
#ifndef GENERATORS_ICOUNTERLOGFILE_H 
#define GENERATORS_ICOUNTERLOGFILE_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ICounterLogFile ( "ICounterLogFile", 1, 0 );

/** @class ICounterLogFile ICounterLogFile.h GenInterfaces/ICounterLogFile.h
 *  
 *
 *  @author Patrick Robbe
 *  @date   2013-01-15
 */
class ICounterLogFile : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICounterLogFile; }

  virtual void addEfficiency( const std::string & name , const unsigned int after , 
                              const unsigned int before , const double fraction , 
                              const double err_fraction ) = 0 ;

  virtual void addCounter( const std::string & name , const unsigned int value ) = 0 ; 

  virtual void addFraction( const std::string & name , const unsigned int number , 
                            const double fraction , const double err_fraction ) = 0 ;

  /// Add cross section value, in mb
  virtual void addCrossSection( const std::string & name , 
                                const unsigned int processId , 
                                const unsigned int number , 
                                const double value ) = 0 ;

  /// Add the event type number
  virtual void addEventType( const unsigned int evType ) = 0 ;

  /// Add the Gauss version
  virtual void addGaussVersion( const std::string & version ) = 0 ;

  /// Add the method type
  virtual void addMethod( const std::string & type ) = 0 ;

  /// Add the generator
  virtual void addGenerator( const std::string & generator ) = 0 ;
  
protected:

private:

};
#endif // GENERATORS_ICOUNTERLOGFILE_H

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
// $Id: FixedLuminosityWithSvc.h,v 1.4 2009-04-07 16:11:21 gcorti Exp $
#ifndef GENERATORS_FIXEDLUMINOSITY_H 
#define GENERATORS_FIXEDLUMINOSITY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IPileUpTool.h"

// forward declaration
class ICounterLogFile ;
class IBeamInfoSvc;

/** @class FixedLuminosityWithSvc FixedLuminosityWithSvc.h "FixedLuminosityWithSvc.h"
 *  
 *  Tool to compute variable number of pile up events
 *  depending on beam parameters
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class FixedLuminosityWithSvc : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FixedLuminosityWithSvc( const std::string& type, const std::string& name,
                   const IInterface* parent) ;

  virtual ~FixedLuminosityWithSvc( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) override;
  
  /// Finalize method
  virtual StatusCode finalize( ) override;

  /** Implements IPileUpTool::numberOfPileUp
   *  Returns the number of pile-up interactions in one event. It follows
   *  a Poisson distribution with 
   *  mean = Luminosity * cross_section / crossing_rate.
   *  The fixed luminosity is returned in the GenHeader.
   */
  virtual unsigned int numberOfPileUp( HepRandomEnginePtr & engine ) override;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) override;

protected:

private:
  /// Location where to store FSR counters (set by options)
  std::string  m_FSRName;

  IBeamInfoSvc *m_beaminfosvc;
  ICounterLogFile * m_xmlLogTool ; ///< XML File for generator counters

  int    m_numberOfZeroInteraction ; ///< Counter of empty events

  int    m_nEvents ; ///< Counter of events (including empty events)
};
#endif // GENERATORS_FIXEDLUMINOSITY_H

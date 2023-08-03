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
// $Id: FixedLuminosityForRareProcess.h,v 1.2 2009-04-07 16:11:21 gcorti Exp $
#ifndef GENERATORS_FIXEDLUMINOSITYFORRAREPROCESS_H
#define GENERATORS_FIXEDLUMINOSITYFORRAREPROCESS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "GenInterfaces/IPileUpTool.h"

// forward declaration
class ICounterLogFile;

/** @class FixedLuminosityForRareProcess FixedLuminosityForRareProcess.h "FixedLuminosityForRareProcess.h"
 *
 *  Tool to compute variable number of pile up events
 *  depending on beam parameters
 *
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class FixedLuminosityForRareProcess : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FixedLuminosityForRareProcess( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~FixedLuminosityForRareProcess(); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize() override;

  /** Implements IPileUpTool::numberOfPileUp
   *  Returns the number of pile-up interactions in one event. It follows
   *  a Poisson distribution with
   *  mean = Luminosity * cross_section / crossing_rate.
   *  The fixed luminosity is returned as the currentLuminosity.
   */
  virtual unsigned int numberOfPileUp( HepRandomEnginePtr& engine ) override;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters() override;

protected:
private:
  ICounterLogFile* m_xmlLogTool; ///< XML File for generator statistics

  std::string m_beamParameters; ///< Location of beam parameters (set by options)

  /// Location where to store FSR counters (set by options)
  std::string m_FSRName;

  int m_nEvents; ///< Counter of events (including empty events)
};
#endif // GENERATORS_FIXEDLUMINOSITYFORRAREPROCESS_H

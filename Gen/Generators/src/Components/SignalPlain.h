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
// $Id: SignalPlain.h,v 1.5 2006-10-01 22:43:40 robbep Exp $
#ifndef GENERATORS_SIGNALPLAIN_H 
#define GENERATORS_SIGNALPLAIN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/Signal.h" 
#include "HepMC3/GenEvent.h"
#include "Event/GenCollision.h"

/** @class SignalPlain SignalPlain.h 
 *  
 *  Tool for signal generation with plain method. Concrete 
 *  implementation of ISampleGenerationTool using the Signal
 *  base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalPlain : public Signal {
 public:
  /// Standard constructor
  SignalPlain( const std::string & type , const std::string & name ,
               const IInterface * parent ) ;
  
  virtual ~SignalPlain( ); ///< Destructor
  
  /** Generates events containing a signal particle.
   *  Implements ISampleProductionTool::generate.
   *  Loop over events until it contains a signal particle
   *  without using any trick (just like Inclusive).
   */
  virtual bool generate( const unsigned int nPileUp , 
                         HepMC3::GenEventPtrs & theEvents ,
                         LHCb::GenCollisions & theCollisions ,
                         HepRandomEnginePtr & engine ) const override;  

 private:
};
#endif // GENERATORS_SIGNALPLAIN_H

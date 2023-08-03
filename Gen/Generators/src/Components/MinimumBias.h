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
// $Id: MinimumBias.h,v 1.3 2005-12-31 17:33:12 robbep Exp $
#ifndef GENERATORS_MINIMUMBIAS_H
#define GENERATORS_MINIMUMBIAS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ExternalGenerator.h"

/** @class MinimumBias MinimumBias.h "MinimumBias.h"
 *
 *  Tool for minimum bias generation. Concrete implementation
 *  of ISampleGenerationTool using the base class ExternalGenerator.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class MinimumBias : public ExternalGenerator {
public:
  /// Standard constructor
  MinimumBias( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~MinimumBias(); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize() override;

  /** Generate minimum bias interactions without any cut.
   *  Implements ISampleGeneratorTool::generate.
   */
  virtual bool generate( const unsigned int nPileUp, HepMC3::GenEventPtrs& theEvents,
                         LHCb::GenCollisions& theCollisions, HepRandomEnginePtr& ) const override;

  /// Dummy implementation for ISampleGeneratorTool::printCounters
  virtual void printCounters() const override { ; }
};
#endif // GENERATORS_MINIMUMBIAS_H

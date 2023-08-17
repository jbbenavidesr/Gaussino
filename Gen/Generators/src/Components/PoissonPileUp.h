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

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IPileUpTool.h"

// forward declaration
class ICounterLogFile;

/** @class PoissonPileUp PoissonPileUp.h "PoissonPileUp.h"
 *
 *  Simple tool to generate pile-up numbers from a Poisson with
 *  user-provided mean.
 *
 *  @author Dominik Muller
 *  @date   2018-02-04
 */
class PoissonPileUp : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  PoissonPileUp( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~PoissonPileUp() = default;

  /// Initialize method
  virtual StatusCode initialize() override;

  virtual unsigned int numberOfPileUp( HepRandomEnginePtr& engine ) override;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters() override;

protected:
private:
  ICounterLogFile* m_xmlLogTool = nullptr;

  std::atomic_long m_numberOfZeroInteraction{ 0 };
  std::atomic_long m_nEvents{ 0 };

  Gaudi::Property<double> m_mean{ this, "PileUpNu", 1, "Pile-up nu" };
};

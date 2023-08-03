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
// $Id: FlatPtRapidity.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_MOMENTUMRANGE_H
#define PARTICLEGUNS_MOMENTUMRANGE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

/** @class FlatPtRapidity FlatPtRapidity.h "FlatPtRapidity.h"
 *
 *  Particle gun with given momentum range
 *
 *  @author Dan Johnson
 *  @date   2016-02-19
 */
class FlatPtRapidity : public GaudiTool, virtual public IParticleGunTool {
public:
  /// Constructor
  FlatPtRapidity( const std::string& type, const std::string& name, const IInterface* parent );

  /// Destructor
  virtual ~FlatPtRapidity();

  /// Initialize particle gun parameters
  virtual StatusCode initialize() override;

  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector& momentum, Gaudi::LorentzVector& origin, int& pdgId,
                                 HepRandomEnginePtr& engine ) override;

  /// Print counters
  virtual void printCounters() override{};

private:
  double m_minPt;       ///< Minimum pT (Set by options)
  double m_minRapidity; ///< Minimum rapidity (Set by options)

  double m_maxPt;       ///< Maximum pT (Set by options)
  double m_maxRapidity; ///< Maximum rapidity (Set by options)

  /// Pdg Codes of particles to generate (Set by options)
  std::vector<int> m_pdgCodes;

  /// Names of particles to generate
  std::vector<std::string> m_names;
};

#endif // PARTICLEGUNS_MOMENTUMRANGE_H

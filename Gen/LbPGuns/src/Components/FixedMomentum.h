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
// $Id: FixedMomentum.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_FIXEDMOMENTUM_H
#define PARTICLEGUNS_FIXEDMOMENTUM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

/** @class FixedMomentum FixedMomentum.h "FixedMomentum.h"
 *
 *  Particle gun with fixed momentum
 *  
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */
class FixedMomentum : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  FixedMomentum( const std::string & type , const std::string& name, 
                 const IInterface * parent ) ;
  
  /// Destructor
  virtual ~FixedMomentum();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize() override;

  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector & momentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId , HepRandomEnginePtr& engine ) override;

  /// Print counters
  virtual void printCounters( ) override {};
                                 
 private:
  double m_px; ///< px (Set by options)
  double m_py; ///< py (Set by options)
  double m_pz; ///< pz (Set by options)

  /// Pdg Codes of particles to generate (Set by options)
  std::vector<int>         m_pdgCodes;

  /// Masses of particles to generate
  std::vector<double>      m_masses;

  /// Names of particles to generate
  std::vector<std::string> m_names;
};

#endif // PARTICLEGUNS_FIXEDMOMENTUM_H

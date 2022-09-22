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
// $Id: GaussianTheta.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_GAUSSIANTHETA_H
#define PARTICLEGUNS_GAUSSIANTHETA_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

/** @class GaussianTheta GaussianTheta.h "GaussianTheta.h"
 *
 *  This code is used to generate particles with Gaussian theta distribution,
 *  and flat phi distribution, and flat momentum distribution
 *
 *  @author P. Robbe (adaptation to new structure)
 *  @date 2008-06-09
 */
class GaussianTheta : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  GaussianTheta( const std::string & type , const std::string & name , 
                 const IInterface * parent ) ;
  
  virtual ~GaussianTheta() ; ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize() override;
  
  /// Generate the particle
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId , HepRandomEnginePtr & engine ) override;
                                 
  /// Print counters
  virtual void printCounters( ) override {};
  
 private:
  double m_mass ;
  int m_pdgCode;  
  double m_minMom , m_maxMom ; ///< Max and min momentum
  double m_meanTheta ; ///< Mean value of theta
  double m_sigmaTheta ; ///< Sigma of Theta Gaussian
  double m_x_axis ;
  double m_y_axis ;
} ;

#endif // PARTICLEGUNS_GENERICGUN_H

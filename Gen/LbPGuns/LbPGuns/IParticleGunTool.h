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
// $Id: IParticleGunTool.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_IPARTICLEGUNTOOL_H 
#define PARTICLEGUNS_IPARTICLEGUNTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "NewRnd/RndCommon.h"


/** @class IParticleGunTool IParticleGunTool.h "ParticleGuns/IParticleGunTool.h"
 *  
 *  Abstract interface to particle gun tool. Generates a single particle.
 * 
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */

namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_IParticleGunTool( "IParticleGunTool" , 1 , 0 ) ;

class IParticleGunTool : virtual public IAlgTool {
public:
  typedef std::vector< int > PIDs ;
  static const InterfaceID& interfaceID() { return IID_IParticleGunTool ; }

  /** Generates one particle.
   *  @param[out] fourMomentum  four-momentum of the generated particle gun
   *  @param[out] origin        four-momentum of the origin vertex of the particle gun 
   *  @param[out] pdgId         pdgId of the generated particle
   */
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId , HepRandomEnginePtr & engine ) = 0 ;

  /// Print various counters at the end of the job
  virtual void printCounters( ) = 0 ;
};
#endif // PARTICLEGUNS_IPARTICLEGUNTOOL_H

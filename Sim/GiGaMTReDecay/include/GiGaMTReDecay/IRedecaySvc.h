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

#include "GaudiKernel/System.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GiGaMTReDecay/Token.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMCUser/typedefs.h"
#include "NewRnd/RndAlgSeeder.h"
#include "Utils/LocalTL.h"

#include "Event/GenCollision.h"

#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "GiGaMTCoreRun/SimResults.h"
// GiGa

namespace Gaussino::ReDecay {
  class TokenGuard;
}
typedef std::tuple<HepMC3::GenEventPtr, int, std::vector<int>, std::shared_ptr<LHCb::GenCollision>> HepMCData;

/** @class IReDecaySvc IReDecaySvc.h GiGaMTReDecay/IReDecaySvc.h
 *
 * Definition of the abstract interface to the ReDecay service.
 *
 *  @author Dominik Muller
 */

class GAUDI_API IReDecaySvc : public extend_interfaces<IInterface> {
public:
  friend Gaussino::ReDecay::TokenGuard;
  /// Retrieve interface ID
  DeclareInterfaceID( IReDecaySvc, 1, 0 );

  // Basic function to obtain a Token given a specific seed pair. Only one token can be requested for a given
  virtual Gaussino::ReDecay::Token obtainToken( const Random::SeedPair& seedpair ) = 0;
  // Function for internal use to signal the service that a token has been used up. This will ultimately trigger the
  // deletion of any stored event once all redecays have been done.
  virtual void                  removeToken( Gaussino::ReDecay::Token& token ) = 0;
  Gaussino::ReDecay::TokenGuard setCurrentToken( const Gaussino::ReDecay::Token& token ) {
    m_currentToken = &token;
    return Gaussino::ReDecay::TokenGuard( this );
  }
  bool isCurrentOriginal() { return m_currentToken->m_original; }

  virtual void storeOriginalHepMC( const Gaussino::ReDecay::Token&, std::vector<HepMC3::GenEventPtr>&,
                                   LHCb::GenCollisions& )                                                 = 0;
  virtual void storeOriginalSimResult( const Gaussino::ReDecay::Token&, const Gaussino::GiGaSimReturns& ) = 0;

  virtual std::vector<HepMCData>& getOriginalHepMCData( const Gaussino::ReDecay::Token& ) = 0;
  virtual std::vector<HepMCData>& getOriginalHepMCData() { return getOriginalHepMCData( *m_currentToken.get() ); };

  virtual Gaussino::GiGaSimReturns getOriginalSimResult( const Gaussino::ReDecay::Token& ) = 0;
  virtual Gaussino::GiGaSimReturns getOriginalSimResult() { return getOriginalSimResult( *m_currentToken.get() ); };

  virtual unsigned int getNPileUp( const Gaussino::ReDecay::Token& ) = 0;
  virtual unsigned int getNPileUp() { return getNPileUp( *m_currentToken.get() ); };
  // Functions to return a specific pileup HepMC data.
  virtual HepMCData getHepMCDataIterated( const Gaussino::ReDecay::Token& ) = 0;
  virtual HepMCData getHepMCDataIterated() { return getHepMCDataIterated( *m_currentToken.get() ); };

  virtual unsigned long long getEncodedOriginalEvtInfo( const Gaussino::ReDecay::Token& ) = 0;
  virtual unsigned long long getEncodedOriginalEvtInfo() { return getEncodedOriginalEvtInfo( *m_currentToken.get() ); }

public:
protected:
  /// virtual destructor
  virtual ~IReDecaySvc() = default;

protected:
  LocalTL<const Gaussino::ReDecay::Token*> m_currentToken{ nullptr };
};

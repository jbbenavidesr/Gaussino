#pragma once

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GiGaMTReDecay/Token.h"
#include "NewRnd/RndAlgSeeder.h"
#include "Utils/LocalTL.h"
#include "HepMCUser/typedefs.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenEvent.h"
// GiGa

namespace Gaussino::ReDecay {
  class TokenGuard;
}
typedef std::tuple<HepMC3::GenEventPtr, int, std::vector<int>> HepMCData;

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
  virtual void removeToken( Gaussino::ReDecay::Token& token ) = 0;
  Gaussino::ReDecay::TokenGuard setCurrentToken( const Gaussino::ReDecay::Token& token ){
    m_currentToken = &token;
    return Gaussino::ReDecay::TokenGuard(this);
  }
  bool isCurrentOriginal(){
    return m_currentToken->m_original;
  }

  virtual void storeOriginalHepMC(const Gaussino::ReDecay::Token &, std::vector<HepMC3::GenEventPtr> &) = 0;
  virtual std::vector<HepMCData> getOriginalHepMCData(const Gaussino::ReDecay::Token &) = 0;

public:
protected:
  /// virtual destructor
  virtual ~IReDecaySvc() = default;
protected:
  LocalTL<const Gaussino::ReDecay::Token*> m_currentToken{nullptr};
};

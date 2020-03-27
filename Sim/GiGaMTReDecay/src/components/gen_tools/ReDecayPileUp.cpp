// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IPileUpTool.h"
#include "GiGaMTReDecay/IRedecaySvc.h"

/** @class ReDecayPileUp ReDecayPileUp.h
 * "ReDecayPileUp.h"
 *
 *  Tool to get the number of pileup events to redecay.
 *  Basically just a getter to have Generation get the pileup from the
 *  service without code changes to Generation
 *
 *  @author Dominik Muller
 *  @date   2020-03-11
 */
class ReDecayPileUp : public GaudiTool, virtual public IPileUpTool {
public:
  public:
  /// Standard constructor
  ReDecayPileUp( const std::string& type, const std::string& name,
                   const IInterface* parent) ;

  unsigned int numberOfPileUp( HepRandomEnginePtr & ) override;

  void printPileUpCounters() override {};

  private:
    ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
};

DECLARE_COMPONENT( ReDecayPileUp )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReDecayPileUp::ReDecayPileUp( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    {
    declareInterface< IPileUpTool >( this ) ;
}

unsigned int ReDecayPileUp::numberOfPileUp( HepRandomEnginePtr & ) {
  return m_redecaysvc->getNPileUp();
}

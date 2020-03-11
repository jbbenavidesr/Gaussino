// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/IPileUpTool.h"
#include "GiGaMTReDecay/IRedecaySvc.h"

/** @class GaussRedecayFakePileUp GaussRedecayFakePileUp.h
 * "GaussRedecayFakePileUp.h"
 *
 *  Tool to get the number of pileup events to redecay.
 *  Basically just a getter to have Generation get the pileup from the
 *  service without code changes to Generation
 *
 *  @author Dominik Muller
 *  @date   2020-03-11
 */
class GaussRedecayFakePileUp : public extends<GaudiTool, IPileUpTool> {
  public:
  /// Standard constructor
  using extends::extends;

  unsigned int numberOfPileUp( HepRandomEnginePtr & ) override;

  void printPileUpCounters() override {};

  private:
    ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};
};

DECLARE_COMPONENT( GaussRedecayFakePileUp )

unsigned int GaussRedecayFakePileUp::numberOfPileUp( HepRandomEnginePtr & ) {
  return m_redecaysvc->getNPileUp();
}

#ifndef LBCRMC_BOOSTFOREPOS_H 
#define LBCRMC_BOOSTFOREPOS_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"

/** @class BoostForEpos BoostForEpos.h 
 *  
 *
 *  @author Patrick Robbe
 *  @date   2016-10-21
 */
class BoostForEpos : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BoostForEpos( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BoostForEpos( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputHepMCEvent  ;
  double m_px, m_py, m_pz ;
  ROOT::Math::Boost m_boost ;
};
#endif // LBCRMC_BOOSTFOREPOS_H

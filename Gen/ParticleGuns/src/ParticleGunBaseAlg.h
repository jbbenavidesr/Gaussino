// $Id: ParticleGunBaseAlg.h,v 1.4 2006-03-22 23:03:05 robbep Exp $
// --------------------------------------------------
//
// Initial code by M. Shapiro
//
// LHCb version: W. Pokorski
//


#ifndef PARTICLEGUNS_PARTICLEGUNBASEALG_H
#define PARTICLEGUNS_PARTICLEGUNBASEALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

namespace HepMC { class GenEvent ; }

/** @class ParticleGunBaseAlg ParticleGunBaseAlg.h 
 *  
 *  Base class to use to interface particle guns
 *
 *  @author
 *  @date   2003-10-10
 */
class ParticleGunBaseAlg : public GaudiAlgorithm {
public:
  typedef std::vector< int >                            PIDs             ;
  
  /// Standard constructor
  ParticleGunBaseAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~ParticleGunBaseAlg();

  /** Initialize method (implemented in ParticleGunBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode initialize();

  /** Execute method (implemented in ParticleGunBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode execute();

  /// Call to the particle gun generator
  virtual StatusCode callParticleGun( HepMC::GenEvent * evt ) = 0 ;
  
 protected:
  
 private:

	/// Event Type
  int m_evnType;

  std::string m_eventLoc;
  std::string m_headerLoc;
  std::string m_collLoc;
  
};   

#endif  // PARTICLEGUNS_PARTICLEGUNBASEALG_H


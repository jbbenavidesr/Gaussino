// $Id: BcChargedNumInLHCb.h,v 1.0 2017-04-11 22:24:49 $
#ifndef GENERATORS_BCCHARGEDNUMINLHCB_H 
#define GENERATORS_BCCHARGEDNUMINLHCB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "MCInterfaces/IGenCutTool.h"

// Forward declaration
class IDecayTool ;

/** @class BcChargedNumInLHCb BcChargedNumInLHCb.h 
 *  
 *  Tool to select events with enough charged stable particles  
 *  which are in LHCb acceptance and from B_c.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Wenqian Huang
 *  @date   2017-05-04
 */

class BcChargedNumInLHCb : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  BcChargedNumInLHCb( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~BcChargedNumInLHCb( ); ///< Destructor


  virtual StatusCode initialize( );   ///< Initialize method

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

  virtual StatusCode finalize( );   ///< Finalize method
  
protected:
  
  
  
private:
  /// Decay tool
  IDecayTool*  m_decayTool;

  /** Study a particle a returns true when all stable daughters
   *  are in LHCb acceptance
   */
  bool passCuts( const HepMC::GenParticle * theSignal ) const ;  
  
  /** Minimum value of angle around z-axis for charged daughters
   * (set by options) 
   */
  double m_chargedThetaMin ;

  /** Maximum value of angle around z-axis for charged daughters
   * (set by options) 
   */
  double m_chargedThetaMax ;

  /** Minimum number of electron in LHCb charged acceptance
   * (set by options) 
   */
  int m_enuminlhcb ;

  /** Minimum number of muon in LHCb charged acceptance
   * (set by options)
   */
  int m_munuminlhcb ;

  /** Minimum number of kaon in LHCb charged acceptance
   * (set by options)
   */
  int m_knuminlhcb ;

  /** Minimum number of pion in LHCb charged acceptance
   * (set by options)
   */
  int m_pinuminlhcb ;

  /** Minimum number of proton in LHCb charged acceptance
   * (set by options)
   */
  int m_pnuminlhcb ;

  /** Minimum number of proton in LHCb charged acceptance
    * (set by options)
    */
  int m_hadnuminlhcb ; 

  double m_mupt ;

  double m_hadpt ;
 
  /// Name of the decay tool to use
  std::string m_decayToolName ;


  int  m_sigBcPID        ;  ///< PDG Id of the B_c 

};
#endif // GENERATORS_BCCHARGEDNUMINLHCB_H

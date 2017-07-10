#ifndef GENCUTS_BEAUTYTOCHARMTOMU3H_H 
#define GENCUTS_BEAUTYTOCHARMTOMU3H_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IFullGenEventCutTool.h"
#include "MCInterfaces/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class BeautyTo2CharmTomu3h BeautyTo2CharmTomu3h.h
 *  
 *  Tool to filter generic Xb->Xc(->KKPi)Xc(->mu X) events and keep the particles in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Stephen Ogilvy
 *  @date   2017-03-10
 */

class BeautyTo2CharmTomu3h: public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  BeautyTo2CharmTomu3h( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);
  
  virtual ~BeautyTo2CharmTomu3h( ); // Destructor
  
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

private:
  /** Check the correct particles are in LHCb acceptance. 
   */
  void passCuts( const HepMC::GenParticle * theSignal
                 , bool &hasMuon, bool &hasHadrons ) const;
  
  double m_chargedThetaMin ;
  double m_chargedThetaMax ;
  double m_muonptmin; 
  double m_muonpmin; 
  double m_hadronptmin;
  double m_hadronpmin; 
  
};
#endif // GENCUTS_BEAUTYTOCHARMTOMU3H_H

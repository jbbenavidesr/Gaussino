#ifndef GENCUTS_BEAUTYTOMUCHARMTO3H_H
#define GENCUTS_BEAUTYTOMUCHARMTO3H_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IFullGenEventCutTool.h"
#include "MCInterfaces/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class BeautyTomuCharmTo3h BeautyTomuCharmTo3h
 *  
 *  Tool to filter SL decays of H_b->H_c(*)(3h)munu and H_b->H_c(*)(3h)tau(mununu)nu decays
 *  while only enforcing that the 3h from the H_c decay and the mu from the H_b or tau 
 *  are in the acceptance.
 *  Allows for the use of kinematic requirements (p/pT) on the muon and hadrons.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Stephen Ogilvy
 *  @date   2017-03-17
 */

class BeautyTomuCharmTo3h: public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  BeautyTomuCharmTo3h( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
  virtual ~BeautyTomuCharmTo3h( ); // Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:

  // Function to check the muon passes the acceptance cuts.
  void passMuonKinematics(const HepMC::GenParticle * theMuon,
                          bool &hasMuon) const ;

  // Function to check the muon passes the acceptance cuts.
  void passMuonFromTauonKinematics(const HepMC::GenParticle * theTauon,
                                   bool &hasMuon) const ;

  // Function to check a Ds has all daughters accepted.
  void passCharmTo3hKinematics(const HepMC::GenParticle * theDs,
                               bool &hasHadrons) const ;
  
  double m_chargedThetaMin ;
  double m_chargedThetaMax ;
  double m_muonptmin; 
  double m_muonpmin; 
  double m_hadronptmin;
  double m_hadronpmin; 
  
};
#endif // GENCUTS_BEAUTYTOMUCHARMTO3H_H

// $Id: ParticleGun.h,v 1.2 2006-03-22 23:03:05 robbep Exp $
#ifndef PARTICLEGUNS_PARTICLEGUN_H
#define PARTICLEGUNS_PARTICLEGUN_H 1

#include "ParticleGunBaseAlg.h"
#include "GaudiKernel/RndmGenerators.h"

class ParticleGun : public ParticleGunBaseAlg {
 public:
  
  /// Constructor
  ParticleGun(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Destructor
  virtual ~ParticleGun();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual StatusCode callParticleGun( HepMC::GenEvent * evt ) ;

 private:
  
  /// Setable Properties:-
  
  /// Minimum momentum
  double m_minMom;
  
  /// Minimum theta angle
  double m_minTheta;

  /// Minimum phi angle
  double m_minPhi;

  /// Maximum momentum
  double m_maxMom;

  /// Maximum theta Angle
  double m_maxTheta;

  /// Maximum phi angle
  double m_maxPhi;

  /// Minimum x position of vertex
  double m_minxvtx;

  /// Maximum x position of vertex
  double m_maxxvtx;

  /// Minimum y position of vertex
  double m_minyvtx;

  /// Maximum y position of vertex
  double m_maxyvtx;

  /// Minimum z position of vertex
  double m_minzvtx;

  /// Maximum z position of vertex
  double m_maxzvtx;

  /// px
  double m_px;

  /// py
  double m_py;

  /// pz
  double m_pz;

  /// Gun mode
  bool m_gmode;

  /// Minimum number of particles
  unsigned int m_minParts;

  /// Maximum number of particles
  unsigned int m_maxParts;

  /// Pdg Codes of particles to generate
  std::vector<int>         m_pdgCodes;

  /// Masses of particles to generate
  std::vector<double>      m_masses;

  /// Names of particles to generate
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;
};

#endif // PARTICLEGUNS_PARTICLEGUN_H

// $Id: CosmicsGun.h,v 1.1 2008-01-24 20:48:16 manca Exp $
#ifndef PARTICLEGUNS_COSMICSGUN_H
#define PARTICLEGUNS_COSMICSGUN_H 1

#include "ParticleGunBaseAlg.h"
#include "GaudiKernel/RndmGenerators.h"

class CosmicsGun : public ParticleGunBaseAlg {
 public:
  
  /// Constructor
  CosmicsGun(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Destructor
  virtual ~CosmicsGun();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual StatusCode callParticleGun( HepMC::GenEvent * evt ) ;

  /// Flux function 
  double muonSpec(double muMomentum, double Theta, double vDepth, 
                              double overAngle);
  
 private:
  
  /// Setable Properties:-
  
  /// Minimum momentum of the mu at detection
  double m_minMom;
  
  /// Minimum zenith angle of the cosmic ray
  double m_minTheta;

  /// Minimum phi angle
  double m_minPhi;

  /// Maximum momentum of the mu at detection
  double m_maxMom;

  /// Maximum zenith Angle of the muon
  double m_maxTheta;

  /// Maximum phi angle
  double m_maxPhi;

  /// Minimum x position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_minxvtx;

  /// Maximum x position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_maxxvtx;

  /// Minimum y position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_minyvtx;


  /// Maximum y position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_maxyvtx;

  /// Minimum z position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_minzvtx;

  /// Maximum z position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_maxzvtx;

  ///Time I want the mu to interact; then the real 
  ///time of the vertex is extrapolated
  double m_time;

  /// Theory model 
  int m_model;  

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

#endif // PARTICLEGUNS_COSMICSGUN_H

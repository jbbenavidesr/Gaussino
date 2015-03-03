// $Id: MaterialEvalGun.h,v 1.2 2006-03-22 23:03:05 robbep Exp $
#ifndef PARTICLEGUNS_MATERIALEVALGUN_H
#define PARTICLEGUNS_MATERIALEVALGUN_H 1

// Include files
// from Gaudi
#include "ParticleGunBaseAlg.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class MaterialEvalGun MaterialEvalGun.h
 * 
 *  Particle gun to evalute material interactions. 
 *  Generate flat distributions of particles in a rectangle area from 
 *  a fix point, with fix momentum.
 */
class MaterialEvalGun : public ParticleGunBaseAlg {
 public:
  /// Standard constructor
  MaterialEvalGun(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~MaterialEvalGun();

  /// Initialization method
  virtual StatusCode initialize();

  /// Execute method: generate events
  virtual StatusCode callParticleGun( HepMC::GenEvent * evt ) ;
  
 private:
  double m_xVtx ; ///< x position of the vertex
  double m_yVtx ; ///< y position of the vertex
  double m_zVtx ; ///< z position of the vertex
         
  double m_zplane ; ///< z position of the rectangular target
  double m_xmin   ; ///< x position of the left side of the rectangular target
  double m_xmax   ; ///< x position of the right side of the rectangular target
  double m_ymin   ; ///< y position of the bottom side of the target
  double m_ymax   ; ///< y position of the top side of the target
  double m_ptotal ; ///< Momentum of the particles
 
  int m_partNumber ; ///< Number of particles to generate per event
  int m_pdgCode    ; ///< PDG Code of the generated particles
  
  Rndm::Numbers m_flatGenerator ; ///< Random number generator
  double m_mass ; ///< Mass of the particle to generate
};

#endif // PARTICLEGUNS_MATERIALEVALGUN_H

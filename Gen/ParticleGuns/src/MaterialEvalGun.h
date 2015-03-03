// $Id: MaterialEvalGun.h,v 1.3 2007-08-24 17:28:04 gcorti Exp $
#ifndef MATERIALEVALGUN_H
#define MATERIALEVALGUN_H 1

// Include files
// from Gaudi
#include "ParticleGunBaseAlg.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class MaterialEvalGun MaterialEvalGun.h
 * 
 *  Particle gun to evalute material.
 *  Generate either random flat distributions or regular scan with a given
 *  step of particles. The homogeneus distribution can be in an x-y rectangular
 *  area or in eta-phi. The origin is a fixed point with fix momentum.
 *
 *  @author Witek Pokorski
 *  @author Gloria Corti, port code from Yasmine Amhis
 *  @date   last modified on 2007-08-21
 *  
 */
class MaterialEvalGun : public ParticleGunBaseAlg {

public:

  /// Standard constructor
  MaterialEvalGun(const std::string& name, ISvcLocator* pSvcLocator);

  
  virtual ~MaterialEvalGun();         ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
 
  /// Specialized method called by base class in execute
  virtual StatusCode callParticleGun( HepMC::GenEvent * evt );

protected:
  
  /// Generate 3-momentum for a uniformly flat distribution in x-y plane
  void generateUniformXY(double& px, double& py, double& pz);
  
  /// Generate 3-momentum for a regular grid in x-y plane
  StatusCode generateGridXY(double& px, double& py, double& pz);

  /// Generate 3-momentum for a uniformly flat distribution in eta-phi plane
  void generateUniformEtaPhi(double& px, double& py, double& pz);

  /// Generate 3-momentum for a regular grid in eta-phi plane
  StatusCode generateGridEtaPhi(double& px, double& py, double& pz);


private:

  double m_xVtx;   ///< x position of the vertex
  double m_yVtx;   ///< y position of the vertex
  double m_zVtx;   ///< z position of the vertex
         
  double m_zplane; ///< z position of the rectangular target
  double m_xmin;   ///< x position of the left side of the rectangular target
  double m_xmax;   ///< x position of the right side of the rectangular target
  double m_ymin;   ///< y position of the bottom side of the target
  double m_ymax;   ///< y position of the top side of the target
  double m_ptotal; ///< Momentum of the particles

  double m_minEta; ///< minimum value of eta
  double m_maxEta; ///< maximum value of eta
  double m_minPhi; ///< minimum value of phi
  double m_maxPhi; ///< maximum value of phi

  bool m_useGrid;  ///< Flag to generate in a regular grid or randomly flat
  bool m_etaPhi;   ///< Flag to generate in eta-phi plane or x-y

  double m_nXstep;   ///< Number of x steps in the grid 
  double m_nYstep;   ///< Number of y steps in the grid
  double m_nEtastep; ///< Number of eta steps in the grid
  double m_nPhistep; ///< Number of phi steps in the grid
 
  int m_partNumber;  ///< Number of particles to generate per event
  int m_pdgCode;     ///< PDG Code of the generated particles

  double m_mass;     ///< Mass of the particle to generate

  double m_dXstep;   ///< Size of step in x
  double m_dYstep;   ///< Size of step in y
  double m_dEtastep; ///< Size of step in eta
  double m_dPhistep; ///< Size of step in phi

  int m_counterX;    ///< Size of step in x
  int m_counterY;    ///< Size of step in y
  int m_counterEta;  ///< Size of step in eta
  int m_counterPhi;  ///< Size of step in phi

  Rndm::Numbers m_flatGenerator; ///< Random number generator
};

#endif // MATERIALEVALGUN_H

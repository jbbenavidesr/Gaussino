// --------------------------------------------------
// 
// File:  CosmicGenerator/CosmicGenerator.h
// Description:

//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
// 
// AuthorList:
//         W. Seligman: Initial Code 08-Nov-2002,
//         based on work by M. Shapiro and I. Hinchliffe
//         adapted for the CosmicGun by S Bentvelsen


#ifndef GENERATORMODULESOTCOSMICGEN_H
#define GENERATORMODULESOTCOSMICGEN_H

#include "ParticleGunBaseAlg.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Transform3DTypes.h"

class SolidBox ;
class CosmicGun;
class ParticleProperty ;

class OTCosmicGenerator : public ParticleGunBaseAlg 
{
public:
  OTCosmicGenerator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~OTCosmicGenerator();
  virtual StatusCode initialize();
  virtual StatusCode callParticleGun( HepMC::GenEvent * evt );
  virtual StatusCode finalize();

private:
  
  // event counter, used for event ID
  int m_events, m_generated, m_rejectedbyenergycut, m_rejectedbyscintacceptance;
  float m_emin, m_emax;
  float m_ctcut;
  float m_tmin, m_tmax;
  int m_printEvent, m_printMod;
  float m_thetamin, m_thetamax, m_phimin, m_phimax;

  /// Flat random number generator
  Rndm::Numbers m_flatgenerator ;
  std::auto_ptr<SolidBox> m_scintsolid ;
  Gaudi::Transform3D m_toptransform ;
  Gaudi::Transform3D m_bottransform ;
  const ParticleProperty* m_muplus  ;
  const ParticleProperty* m_muminus ;
};

#endif

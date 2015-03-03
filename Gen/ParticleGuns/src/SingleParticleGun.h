// $Id: SingleParticleGun.h,v 1.2 2006-03-22 23:03:05 robbep Exp $
// --------------------------------------------------
// 
// File:  GeneratorModules/SingleParticleGun.h
// Description:
//    This code is used to generate a single particle or a poissonly
//    distributed set of particles with pt, eta and phi selected as
//    specified by the JobOptionsService.  Three modes of generation are
//    possible:
//           FixedMode:  Generation with a fixed value
//           GaussianMode: Generation with a gaussian of specified mean and
//                         sigma
//           FlatMode: Generation of a flat distribution between a specified
//                     minimum and maximum value
//    The mode can be separately selected for pt, eta and phi.
//    The particle species can also be specified
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
//
// AuthorList:
//         M. Shapiro:  Initial Code March 2000
//         W. Pokorski: LHCb modifications: all the time...

#ifndef PARTICLEGUNS_SINGLEPARTICLEGUN_H
#define PARTICLEGUNS_SINGLEPARTICLEGUN_H 1

#include "ParticleGunBaseAlg.h"
#include "GaudiKernel/RndmGenerators.h"

namespace SPGGenMode {
  enum Mode { FixedMode = 1, GaussMode , FlatMode } ;
};

class SingleParticleGun : public ParticleGunBaseAlg {
 public:
  
  SingleParticleGun(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SingleParticleGun();
  virtual StatusCode initialize();
  virtual StatusCode callParticleGun( HepMC::GenEvent* evt ) ;
  
 private:
	// Setable Properties:-
  double m_requestedPt;
  double m_requestedEta;
  double m_requestedPhi;
  double m_minPt;
  double m_minEta;
  double m_minPhi;
  double m_maxPt;
  double m_maxEta;
  double m_maxPhi;
  double m_sigmaPt;
  double m_sigmaEta;
  double m_sigmaPhi;
  int m_PtGenMode , m_EtaGenMode , m_PhiGenMode ; 
  int m_pdgCode;  

	// Local Member Data:
  double m_mass ;

	// Private Methods:
  double generateValue( const int mode , const double val , 
                        const double sigma , const double min , 
                        const double max ) ;

  // Random generators:
  Rndm::Numbers m_flatGenerator ;
  Rndm::Numbers m_gaussGenerator ;
} ;

#endif // PARTICLEGUNS_SINGLEPARTICLEGUN_H

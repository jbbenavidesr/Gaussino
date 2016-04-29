#ifndef GaussRedecaySorter_H
#define GaussRedecaySorter_H 1

// Include files
// from Gaudi
#include "Event/HepMCEvent.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGaussRedecayStr;  ///< GaussRedecay storage service
class IGaussRedecayCtr;  ///< GaussRedecay counter service

namespace HepMC {
class GenParticle;
}

namespace LHCb {
class IParticlePropertySvc;
}

/** @class GaussRedecaySorter GaussRedecaySorter.h
 *
 * Algorithm running after the nominal generation to split off parts of the
 * HepMC
 * if those should be redecayed.
 *
 *  @author Dominik Muller
 *  @date   2016-4-26
 */
class GaussRedecaySorter : public GaudiAlgorithm {
  public:
  GaussRedecaySorter(const std::string& Name, ISvcLocator* SvcLoc);

  virtual ~GaussRedecaySorter();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

  private:
  std::string m_gaussRDSvcName;
  IGaussRedecayStr* m_gaussRDStrSvc = nullptr;
  IGaussRedecayCtr* m_gaussRDCtrSvc = nullptr;
  LHCb::IParticlePropertySvc* m_ppSvc = nullptr;
  HepMC::GenParticle* m_theSignal = nullptr;

  std::string m_generationLocation;

  HepMC::GenParticle* find_signal(LHCb::HepMCEvents* evts);
  void store_particle(HepMC::GenParticle*);
  /*Decay everything that is heavier than the signal (Only in the signal event
   * for now.)*/
  void store_heavier_than_signal(LHCb::HepMCEvents* evts);
  /*Helper to tag particles that have to be deleted. Used to identify the head
   * of a tree that should be redecayed. Changes status of the particle to
   * 1042 if it should be redecayed and to
   * 1043 if it is the daugther of a particle to be redecayed.
   * 1042 will not override 1043.*/
  void recursive_tagger(HepMC::GenParticle*);
  void printChildren(HepMC::GenParticle*, int level=0);
};

#endif  // GaussRedecaySorter_H

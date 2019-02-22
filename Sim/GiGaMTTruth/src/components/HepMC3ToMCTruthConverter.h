#pragma once

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Geant4/G4SystemOfUnits.hh"

#include "GiGaMTTruth/IHepMC3ToMCTruthTracker.h"
#include "HepMC/GenParticle.h"

class G4PrimaryParticle;
class G4PrimaryVertex;
namespace LHCb
{
  class IParticlePropertySvc;
}

/** @class HepMC3ToMCTruthTracker HepMC3ToMCTruthTracker.h "HepMC3ToMCTruthTracker.h"
 *
 *  Tool to loop over the HepMC3 structure and fill an MCTruthConverter object that hols
 *  the information on which particles to keep and which ones are supposed to be treated
 *  by Geant4
 *
 *  @author Dominik Muller
 *  @date   15.2.2019
 *
 */
class HepMC3ToMCTruthTracker : public extends<GaudiTool, IHepMC3ToMCTruthTracker>
{
public:
  Gaudi::Property<double> m_travelLimit{this, "TravelLimit", 1e-10 * m};
  using extends::extends;

  virtual ~HepMC3ToMCTruthTracker() = default;

  virtual Gaussino::MCTruthTracker BuildConverter( const std::vector<const HepMC::GenEvent*>& ) const override;

private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
  void convert( const HepMC::GenParticlePtr& hepmc, G4PrimaryParticle* g4parent = nullptr,
                G4PrimaryVertex* g4vertex = nullptr );
  bool IsTraveling( const HepMC::GenParticlePtr& part );
  /// Decide if a particle has to be kept or not.
  static bool keep( const HepMC::GenParticlePtr & particle );
};

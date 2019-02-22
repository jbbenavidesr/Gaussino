#pragma once

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Geant4/G4SystemOfUnits.hh"

#include "GiGaMTTruth/IHepMC3ToMCTruthConverter.h"
#include "HepMC/GenParticle.h"

class G4PrimaryParticle;
class G4PrimaryVertex;
namespace LHCb
{
  class IParticlePropertySvc;
}

/** @class HepMC3ToMCTruthConverter HepMC3ToMCTruthConverter.h "HepMC3ToMCTruthConverter.h"
 *
 *  Tool to loop over the HepMC3 structure and fill an MCTruthConverter object that hols
 *  the information on which particles to keep and which ones are supposed to be treated
 *  by Geant4
 *
 *  @author Dominik Muller
 *  @date   15.2.2019
 *
 */
class HepMC3ToMCTruthConverter : public extends<GaudiTool, IHepMC3ToMCTruthConverter>
{
public:
  Gaudi::Property<double> m_travelLimit{this, "TravelLimit", 1e-10 * m};
  using extends::extends;

  virtual ~HepMC3ToMCTruthConverter() = default;

  virtual Gaussino::MCTruthConverterPtrs BuildConverter( const std::vector<HepMC::GenEvent>& ) const override;

private:
  ServiceHandle<LHCb::IParticlePropertySvc> m_ppSvc{this, "PropertyService", "LHCb::ParticlePropertySvc"};
  bool IsTraveling( const HepMC::GenParticlePtr& part ) const;
  /// Decide if a particle has to be kept or not.
  bool keep( const HepMC::GenParticlePtr & particle ) const;
};

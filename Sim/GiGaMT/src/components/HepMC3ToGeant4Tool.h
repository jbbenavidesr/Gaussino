#pragma once

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Geant4/G4SystemOfUnits.hh"

#include "HepMC/GenParticle.h"
#include "SimInterfaces/IHepMC3ToGeant4Tool.h"

class G4PrimaryParticle;
class G4PrimaryVertex;
namespace LHCb
{
  class IParticlePropertySvc;
}

/** @class HepMC3ToGeant4Tool HepMC3ToGeant4Tool.h "HepMC3ToGeant4Tool.h"
 *
 *  Basic tool to translate an HepMC::GenEvent into a G4Event
 *  Only stable particles are converted, i.e. those without
 *  and end vertex in the HepMC record
 *
 *  @author Dominik Muller
 *  @date   22.6.2018
 *
 */
class HepMC3ToGeant4Tool : public extends<GaudiTool, IHepMC3ToGeant4Tool>
{
public:
  Gaudi::Property<double> m_travelLimit{this, "TravelLimit", 1e-10 * m};
  using extends::extends;

  virtual ~HepMC3ToGeant4Tool();
  StatusCode initialize() override;

  virtual G4Event* g4Event( const std::vector<HepMC::GenEvent>& ) override final;

private:
  void convert( const HepMC::GenParticlePtr& hepmc, G4PrimaryParticle* g4parent = nullptr,
                G4PrimaryVertex* g4vertex = nullptr );
  bool IsTraveling( const HepMC::GenParticlePtr& part );
  LHCb::IParticlePropertySvc* m_ppsvc{nullptr};
};

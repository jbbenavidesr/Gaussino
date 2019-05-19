#pragma once
// G4
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Geant4/G4VPhysicsConstructor.hh"
#include "Geant4/G4VUserPhysicsList.hh"

#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

//@class GiGaMTModularPhysListFAC
//
// Modular physics list factory which constructs a G4ModularPhysicsList
// and adds user specified pysics constructors. These constructors have to
// be implementations of the GiGaFactoryBase<G4VPhysicsConstructor> interface
// and can hence to controlled in the python configurables
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTModularPhysListFAC : public extends<GiGaTool, GiGaFactoryBase<G4VUserPhysicsList>>
{
public:
  typedef GiGaFactoryBase<G4VPhysicsConstructor> ConstructorFactory;
  typedef std::vector<ConstructorFactory*> ConstructorFactories;
  StatusCode initialize() override;
  StatusCode finalize() override;

  using extends::extends;
  virtual ~GiGaMTModularPhysListFAC(){};

public:
  virtual G4VUserPhysicsList* construct() const override;

  //virtual void SetCuts();

private:

  ConstructorFactories m_constructors{};
  Gaudi::Property<std::vector<std::string>> m_constructorNames{this, "PhysicsConstructors", {}};
  Gaudi::Property<bool> m_dumpCutsTable{this, "DumpCutsTable", false};
  Gaudi::Property<double> m_cutForGamma{this, "CutForGamma", -1 * CLHEP::km};
  Gaudi::Property<double> m_cutForElectron{this, "CutForElectron", -1 * CLHEP::km};
  Gaudi::Property<double> m_cutForPositron{this, "CutForPositron", -1 * CLHEP::km};
};

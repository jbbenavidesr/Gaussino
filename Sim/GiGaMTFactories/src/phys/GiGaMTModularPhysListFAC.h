#pragma once
// G4
#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4VPhysicsConstructor.hh"
#include "Geant4/G4VUserPhysicsList.hh"

#include "GiGaMTFactories/GiGaFactoryBase.h"

//@class GiGaMTModularPhysListFAC
//
// Modular physics list factory which constructs a G4ModularPhysicsList
// and adds user specified pysics constructors. These constructors have to
// be implementations of the GiGaFactoryBase<G4VPhysicsConstructor> interface
// and can hence to controlled in the python configurables
//
//@author Dominik Muller <dominik.muller@cern.ch>

class GiGaMTModularPhysListFAC : public extends<GaudiTool, GiGaFactoryBase<G4VUserPhysicsList>>
{
public:
  Gaudi::Property<std::vector<std::string>> m_physconstr{this, "PhysicsConstructors"};
  Gaudi::Property<bool> m_dumpCutsTable{this, "DumpCutsTable", false};
  typedef GiGaFactoryBase<G4VPhysicsConstructor> ConstructorFactory;
  typedef std::vector<ConstructorFactory*> ConstructorFactories;

  using extends::extends;
  virtual ~GiGaMTModularPhysListFAC(){};

public:
  virtual StatusCode initialize() override;
  virtual G4VUserPhysicsList* construct() const override;

  //virtual void SetCuts();

private:
  ConstructorFactories m_constructors;
};

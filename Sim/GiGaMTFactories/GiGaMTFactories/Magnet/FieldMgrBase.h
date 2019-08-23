#pragma once

// Include files
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"
#include "Geant4/G4Mag_EqRhs.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4MagIntegratorStepper.hh"
#include "Geant4/G4MagneticField.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

/** @class FieldMgrBase
 *
 * Implements the basic functionality to construct and configure
 * the G4FieldManager. A purely virtual function remains
 *
 *  @author Dominik Muller
 *  @date   2019-06-03
 */
namespace Gaussino
{
  class FieldMgrBase : public extends<GiGaTool, GiGaFactoryBase<G4FieldManager, bool>>
  {
  public:
    using extends::extends;
    StatusCode initialize() override {
      auto sc = extends::finalize();
      m_stepper.retrieve();
      return sc;
    }

    virtual ~FieldMgrBase()                = default;
    virtual G4MagneticField* field() const = 0;

    virtual G4FieldManager* construct(const bool &) const override;
  private:
    ToolHandle<GiGaFactoryBase<G4MagIntegratorStepper, G4Mag_EqRhs*>> m_stepper{this, "Stepper", "G4ClassicalRK4"};
    Gaudi::Property<double> m_minStep{this, "MinStep", 0.01 * CLHEP::mm};
    Gaudi::Property<double> m_deltaChord{this, "DeltaChord", 0.25 * CLHEP::mm};
    Gaudi::Property<double> m_deltaintersection{this, "DeltaIntersection", 1e-05 * CLHEP::mm};
    Gaudi::Property<double> m_deltaonestep{this, "DeltaOneStep", 0.001 * CLHEP::mm};
    Gaudi::Property<double> m_minimumEpsilonStep{this, "MinEpsilonStep", 5e-05 * CLHEP::mm};
    Gaudi::Property<double> m_maximumEpsilonStep{this, "MaxEpsilonStep", 0.001 * CLHEP::mm};
  };
}

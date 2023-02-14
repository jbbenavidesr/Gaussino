/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

// Geant4
#include "G4CashKarpRKF45.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixHeum.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4ImplicitEuler.hh"
#include "G4RKG3_Stepper.hh"
#include "G4SimpleHeum.hh"
#include "G4SimpleRunge.hh"

// Gaussino
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

class G4Mag_EqRhs;

namespace Gaussino::MagneticField {
  template <typename STEPPER>
  class StepperFactory : public extends<GiGaTool, GiGaFactoryBase<G4MagIntegratorStepper, G4Mag_EqRhs*>> {
    static_assert( std::is_base_of<G4MagIntegratorStepper, STEPPER>::value );

  public:
    using extends::extends;
    virtual STEPPER* construct( G4Mag_EqRhs* equation ) const override { return new STEPPER{equation}; }
  };
} // namespace Gaussino::MagneticField

DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4ExplicitEuler>, "G4ExplicitEuler" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4ImplicitEuler>, "G4ImplicitEuler" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4SimpleRunge>, "G4SimpleRunge" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4ClassicalRK4>, "G4ClassicalRK4" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4CashKarpRKF45>, "G4CashKarpRKF45" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4RKG3_Stepper>, "G4RKG3_Stepper" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4HelixExplicitEuler>, "G4HelixExplicitEuler" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4HelixImplicitEuler>, "G4HelixImplicitEuler" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4HelixSimpleRunge>, "G4HelixSimpleRunge" )
DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::StepperFactory<G4HelixHeum>, "G4HelixHeum" )
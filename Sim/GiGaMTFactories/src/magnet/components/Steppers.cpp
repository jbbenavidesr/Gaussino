#include "GaudiAlg/GaudiTool.h"
#include "Geant4/G4EquationOfMotion.hh"
#include "Geant4/G4MagIntegratorStepper.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

#include "Geant4/G4ExplicitEuler.hh"
#include "Geant4/G4ImplicitEuler.hh"
#include "Geant4/G4SimpleRunge.hh"
#include "Geant4/G4SimpleHeum.hh"
#include "Geant4/G4ClassicalRK4.hh"
#include "Geant4/G4CashKarpRKF45.hh"
#include "Geant4/G4RKG3_Stepper.hh"
#include "Geant4/G4HelixExplicitEuler.hh"
#include "Geant4/G4HelixImplicitEuler.hh"
#include "Geant4/G4HelixSimpleRunge.hh"
#include "Geant4/G4HelixHeum.hh"

template <typename STEPPER>
class GiGaMTMagnetStepperFAC : public extends<GaudiTool, GiGaFactoryBase<G4MagIntegratorStepper, G4Mag_EqRhs*> >
{
  static_assert( std::is_base_of<G4MagIntegratorStepper, STEPPER>::value );

public:
  using extends::extends;
  virtual STEPPER* construct( G4Mag_EqRhs* equation ) const override
  {
    auto tmp = new STEPPER{equation};
    return tmp;
  }
};

typedef GiGaMTMagnetStepperFAC<G4ExplicitEuler> GiGaMT_G4ExplicitEuler;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4ExplicitEuler, "GiGaMT_G4ExplicitEuler" )

typedef GiGaMTMagnetStepperFAC<G4ImplicitEuler> GiGaMT_G4ImplicitEuler;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4ImplicitEuler, "GiGaMT_G4ImplicitEuler" )

typedef GiGaMTMagnetStepperFAC<G4SimpleRunge> GiGaMT_G4SimpleRunge;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4SimpleRunge, "GiGaMT_G4SimpleRunge" )

typedef GiGaMTMagnetStepperFAC<G4SimpleRunge> GiGaMT_G4SimpleRunge;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4SimpleRunge, "GiGaMT_G4SimpleRunge" )

typedef GiGaMTMagnetStepperFAC<G4ClassicalRK4> GiGaMT_G4ClassicalRK4;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4ClassicalRK4, "GiGaMT_G4ClassicalRK4" )

typedef GiGaMTMagnetStepperFAC<G4CashKarpRKF45> GiGaMT_G4CashKarpRKF45;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4CashKarpRKF45, "GiGaMT_G4CashKarpRKF45" )

typedef GiGaMTMagnetStepperFAC<G4RKG3_Stepper> GiGaMT_G4RKG3_Stepper;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4RKG3_Stepper, "GiGaMT_G4RKG3_Stepper" )

typedef GiGaMTMagnetStepperFAC<G4HelixExplicitEuler> GiGaMT_G4HelixExplicitEuler;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HelixExplicitEuler, "GiGaMT_G4HelixExplicitEuler" )

typedef GiGaMTMagnetStepperFAC<G4HelixImplicitEuler> GiGaMT_G4HelixImplicitEuler;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HelixImplicitEuler, "GiGaMT_G4HelixImplicitEuler" )

typedef GiGaMTMagnetStepperFAC<G4HelixSimpleRunge> GiGaMT_G4HelixSimpleRunge;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HelixSimpleRunge, "GiGaMT_G4HelixSimpleRunge" )

typedef GiGaMTMagnetStepperFAC<G4HelixHeum> GiGaMT_G4HelixHeum;
DECLARE_COMPONENT_WITH_ID( GiGaMT_G4HelixHeum, "GiGaMT_G4HelixHeum" )

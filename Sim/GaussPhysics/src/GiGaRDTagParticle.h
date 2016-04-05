// ============================================================================
#ifndef GAUSSPHYS_GIGARDTAGPARTICLE_H
#define GAUSSPHYS_GIGARDTAGPARTICLE_H 1

#include "GiGa/GiGaPhysConstructorBase.h"
#include "G4Decay.hh"
#include "G4UnknownDecay.hh"
// ============================================================================
// forward declarations
template <class TYPE>
class GiGaFactory;

class GiGaRDTagParticle : public GiGaPhysConstructorBase {
  /// friend factory for instantiation
  friend class GiGaFactory<GiGaRDTagParticle>;

  public:
  GiGaRDTagParticle(const std::string& type, const std::string& name,
                    const IInterface* parent);

  virtual ~GiGaRDTagParticle();

  public:
  void ConstructParticle();
  void ConstructProcess(){};  // construct the processed
};

#endif

// ============================================================================
#ifndef    GAUSSREDECAY_GIGAPHYSG4RDTAG_H
#define    GAUSSREDECAY_GIGAPHYSG4RDTAG_H 1 
// ============================================================================
// include files 
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/GiGaPhysConstructorBase.h"
#include "Geant4/G4Decay.hh"
#include "Geant4/G4UnknownDecay.hh"
// ============================================================================
// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysG4RDTag : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysG4RDTag>;

public:

  GiGaPhysG4RDTag  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;


public:
  void ConstructParticle ();
  void ConstructProcess  ();
  
private:
  
  G4Decay m_decayProcess ;
  G4UnknownDecay m_unknownDecay ;
  int m_g4_reserve = 0;
  int m_initial_placeholder = 424242;
  
};

#endif

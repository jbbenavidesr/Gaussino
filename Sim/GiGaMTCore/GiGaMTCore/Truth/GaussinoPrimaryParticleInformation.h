#pragma once
// ============================================================================
// Include files
// ============================================================================
// GiGa
// ============================================================================
#include "GiGa/GiGaUtil.h"
// ============================================================================
// G4
// ============================================================================
#include "Geant4/G4VUserPrimaryParticleInformation.hh"
#include "Geant4/G4ios.hh"
// ============================================================================
// Event
#include "Event/HepMCEvent.h"

namespace LHCb {
  class MCParticle ;
}

/** @class GaussinoPrimaryParticleInformation
 *
 *  Class to hold oscillation and signal information to be passed along with
 *  a G4PrimaryParticle
 *  @author Patrick Robbe
 *  @date   2004-02-06
 */
class GaussinoPrimaryParticleInformation :
  public G4VUserPrimaryParticleInformation {
public:
  /// Standard constructors
  /// sets oscillation information
  GaussinoPrimaryParticleInformation( bool osc ) :
    G4VUserPrimaryParticleInformation ( ),
    m_hasOscillated(  osc  ) ,
    m_isSignal     ( false ) ,
    m_signalBarcode( -1    ) ,
    m_pHepMCEvent  (   0   ) ,
    m_mcParticle   (   0   ) { ; }

  /// Sets link to HepMC Particle
  GaussinoPrimaryParticleInformation( bool sig, int code, LHCb::HepMCEvent *event ) :
    G4VUserPrimaryParticleInformation ( ),
    m_hasOscillated ( false ),
    m_isSignal      (  sig  ),
    m_signalBarcode ( code  ),
    m_pHepMCEvent   ( event ),
    m_mcParticle    (   0   ) { ; }


  virtual ~GaussinoPrimaryParticleInformation( ) {;} ///< Destructor

  /// Sets oscillation information
  void setHasOscillated( bool osc ) { m_hasOscillated = osc ; }

  /// Sets signal information
  void setIsSignal( bool sig ) { m_isSignal = sig ; }

  /// Sets signal barcode
  void setSignalBarcode( int code ) { m_signalBarcode = code ; }

  /// Sets HepMCEvent pointer
  void setHepMCPointer( LHCb::HepMCEvent * event ) { m_pHepMCEvent = event ; }

  /// Sets pointer to mother MCParticle
  void setMotherMCParticle( LHCb::MCParticle * mcp ) { m_mcParticle = mcp ; }

  /// returns true if the particle has oscillated
  bool hasOscillated( ) { return m_hasOscillated ; }

  /// returns true is the particle is the forced decay particle
  bool isSignal( ) { return m_isSignal ; }

  /// returns the barcode of the signal particle
  int  signalBarcode( ) { return m_signalBarcode ; }

  /// returns a pointer to the HepMC Event containing the signal HepMC particle
  LHCb::HepMCEvent * pHepMCEvent ( ) { return m_pHepMCEvent ; }

  /// Returns a pointer to the mother MCParticle
  LHCb::MCParticle * motherMCParticle( ) { return m_mcParticle ; }

  /// Print function needed in G4VUserPrimaryParticleInformation
  void Print( ) const override { if ( m_hasOscillated )
    G4cout << "Particle has oscillated" << G4endl ;
  }


protected:

private:
  /// Flag to indicate if particle has oscillated
  bool m_hasOscillated ;
  /// Flag to indicate if particle is the signal particle
  bool m_isSignal ;
  /// Corresponding HepMC Particle barcode (filled for signal particle)
  int m_signalBarcode ;
  /// Pointer to HepMCEvent which the HepMC particle belongs to
  LHCb::HepMCEvent * m_pHepMCEvent ;
  /// Pointer to the MCParticle which is the mother of this G4PrimaryParticle
  LHCb::MCParticle * m_mcParticle ;
};
// ============================================================================

// ============================================================================
/** simple function for safe/fast casting
 *  @param info pointer to G4VUserPrimaryParticleInformation
 *  @return result of "fast cast" procedure
 *  @see GiGaUtil::FastCast
 */
// ============================================================================
inline GaussinoPrimaryParticleInformation*
gigaPrimaryParticleInformation
( G4VUserPrimaryParticleInformation* info )
{
  if( 0 == info ) { return 0 ; }
  GiGaUtil::FastCast
    <
    G4VUserPrimaryParticleInformation ,
    GaussinoPrimaryParticleInformation
    >                                 cast;
  return cast( info );
}


// ============================================================================
/** simple function for safe/fast casting
 *  @param info pointer to G4VUserPrimaryParticleInformation
 *  @return result of "fast cast" procedure
 *  @see GiGaUtil::FastCast
 */
// ============================================================================
inline const GaussinoPrimaryParticleInformation*
gigaPrimaryParticleInformation
( const G4VUserPrimaryParticleInformation* info )
{
  if( 0 == info ) { return 0 ; }
  GiGaUtil::FastCast
    <
    const G4VUserPrimaryParticleInformation ,
    const GaussinoPrimaryParticleInformation
    >                                         cast;
  return cast( info );
}

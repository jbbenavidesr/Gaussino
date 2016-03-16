// $Id: IGaussRDStr.h,v 0.1 2015-12-10 18:58:18 ibelyaev Exp $
// ============================================================================
// ============================================================================
//
// ============================================================================
#ifndef GIGA_IGIGASVC_H
#define GIGA_IGIGASVC_H 1 
/// STD and STL 
//#include   <utility>
/// Include files from the Framework
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/IService.h"

// From Geant4 
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

/** @class IGaussRDStr IGaussRDStr.h GiGa/IGaussRDStr.h
 *
 *  definition of the abstract interface to Geant4 Service 
 *   for event-by-event communications with Geant4 
 *
 *  @author Vanya Belyaev
 */

class HepMC::GenEvent;
class HepMC::GenVertex;
class HepMC::GenParticle;

class IGaussRDStr : virtual public IService
{
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() ;
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   initialize() = 0 ;
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   finalize  () = 0 ;

public:
  
  /** Store simualted G4 underlying event for the underlying event
   *
   *  @param G4Event* event
   *  @return status code  
   */
  virtual StatusCode  saveJunkG4Event( G4Event* event ) = 0 ;

  /** Load the stored G4 event for the underlying event
   *
   *  @param G4Event*& event
   *  @return status code  
   */
  virtual StatusCode  loadJunkG4Event( G4Event* & event ) const = 0 ;

  /** Save the signal particle and its origin vertex for redecay
   *
   *  @param G4Event*& event
   *  @return status code  
   */
  virtual StatusCode  saveSignalGenInfo( HepMC::GenParticle* part, HepMC::GenVertex * vertex) = 0 ;

  /** Load the signal particle and its origin vertex for redecay
   *
   *  @param G4Event*& event
   *  @return status code  
   */
  virtual StatusCode  saveSignalGenInfo( HepMC::GenParticle*& part, HepMC::GenVertex *& vertex) const = 0 ;

protected:
  
  /// virtual destructor
  virtual ~IGaussRDStr();

};

// ============================================================================
#endif ///< GIGA_GIGASVC_H
// ============================================================================








































/// STD & ATL 
#include <string>
#include <typeinfo> 
/// GaudiKernel 
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/System.h" 
#include  "GaudiKernel/MsgStream.h" 
/// G4 
#include  "Geant4/G4VUserDetectorConstruction.hh"
#include  "Geant4/G4VPhysicalVolume.hh"
#include  "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include  "Geant4/G4VUserPhysicsList.hh"
#include  "Geant4/G4UserRunAction.hh"
#include  "Geant4/G4UserEventAction.hh"
#include  "Geant4/G4UserStackingAction.hh"
#include  "Geant4/G4UserTrackingAction.hh"
#include  "Geant4/G4UserSteppingAction.hh"

/// GiGa
#include "GiGaMTCore/GiGaMTRunManager.h"
#include "GiGaMT/GiGaException.h"
//#include "GiGa/GiGaUtil.h"
/// local 
#include "GiGaMT.h"


// ============================================================================
/**  @file
 * 
 *   Implementation of class GiGa  
 *   all methods from abstract interface IGiGaMTSetUpSvc 
 *   Based on original GiGa interface by Vanya Balyaev
 *
 *
 *   @author: Dominik Muller
 */
// ============================================================================

// ============================================================================
/** set detector constructon module 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 *
 *  @param  obj      pointer to detector construction module  
 *  @return StatusCode
 */
// ============================================================================
StatusCode GiGaMT::setConstruction ( G4VUserDetectorConstruction   * obj )
{
  try{
    GiGaMTRunManager::GetGiGaMTRunManager()->SetUserInitialization(obj);}

  catch(...)                            
    { return Error( "Failed to set G4VUserDetectorConstruction "      ) ; }
  return StatusCode::SUCCESS;
}

StatusCode GiGaMT::setDetector     ( G4VPhysicalVolume             * obj )
{ 
  //FIXME: make sure this is properly set up
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported throw exception
 * 
 *  @param  obj        pointer to new generator   
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4VUserPrimaryGeneratorAction * obj )
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4VUserPrimaryGenerator*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4VUserPrimaryGenerator*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4VUserPrimaryGenerator*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through  exception thrown 
 * 
 *  @param  obj      pointer to physics list    
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4VUserPhysicsList            * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4VUserPhysicsList*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4VUserPhysicsList*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4VUserPhysicsList*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new run action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserRunAction               * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserRunAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserRunAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserRunAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new event action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserEventAction             * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserEventAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserEventAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserEventAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through  exception thrown 
 * 
 *  @param  obj     pointer to new stacking action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserStackingAction          * obj ) 
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserStackingAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserStackingAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserStackingAction*)"         ) ; }
  ///
  return *this;
}

    
// ============================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new tracking action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserTrackingAction          * obj ) 
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserTrackingAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserTrackingAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserTrackingAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new stepping action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserSteppingAction          * obj ) 
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserSteppingAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserSteppingAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserSteppingAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new world wolume 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj    pointer to  new world volume   
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setDetector     ( G4VPhysicalVolume             * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setDetector()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setDetector()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setDetector()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj        pointer to new generator   
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setGenerator    ( G4VUserPrimaryGeneratorAction * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setGenerator()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setGenerator()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setGenerator()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to physics list    
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setPhysics      ( G4VUserPhysicsList            * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setPhysics()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setPhysics()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setPhysics()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new run action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setRunAction    ( G4UserRunAction               * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setRunAction()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setRunAction()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setRunAction()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new event action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setEvtAction    ( G4UserEventAction             * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setEvtAction()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setEvtAction()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setEvtAction()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stacking action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setStacking     ( G4UserStackingAction          * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setStacking()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setStacking()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setStacking()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new tracking action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setTracking     ( G4UserTrackingAction          * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setTracking()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setTracking()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setTracking()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stepping action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setStepping     ( G4UserSteppingAction          * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setStepping()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setStepping()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setStepping()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
// The END 
// ============================================================================


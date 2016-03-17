// $Id: IGaussRDCtr.h,v 0.1 2015-12-10 18:58:18 ibelyaev Exp $
// ============================================================================
// ============================================================================
//
// ============================================================================
#ifndef GAUSS_IGAUSSRDStr_H
#define GAUSS_IGAUSSRDStr_H 1 
/// STD and STL 
//#include   <utility>
/// Include files from the Framework
#include  "GaudiKernel/IService.h"

/** @class IGaussRDCtr IGaussRDCtr.h GiGa/IGaussRDCtr.h
 *
 *  
 * 
 *
 *  @author Dominik Muller dominik.muller@cern.ch
 */

static const InterfaceID IID_IGaussRDCtr(123, 1 , 0);

class IGaussRDCtr : virtual public IService
{
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID(){return IID_IGaussRDCtr;};
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   initialize() = 0 ;
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   finalize  () = 0 ;

public:
  
  /** Registers a new event, returns false if the UD is already simulated and should be reused.
   *  Returns true if everything needs to be redone and deletes the internal storage objects.
   *
   *  @return bool
   */
  virtual bool registerNewEvent() = 0 ;

protected:
  
  /// virtual destructor
  virtual ~IGaussRDCtr(){};

};

// ============================================================================
#endif ///< GIGA_GIGASVC_H
// ============================================================================

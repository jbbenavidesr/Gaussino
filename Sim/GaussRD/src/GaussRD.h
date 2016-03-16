// $Id: GiGa.h,v 1.9 2009-10-14 13:50:02 gcorti Exp $
#ifndef REDECAY_SERVICE_H
#define REDECAY_SERVICE_H 1

// Include files
// from STD & STL
#include <string>
#include <list>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/IToolSvc.h"

// from GaussRD
#include "GaussRD/IGaussRDStr.h"  //Abstract storage interface
#include "GaussRD/IGaussRDCtr.h"  //Abstract control interface

// Forwad declarations
// from Gaudi
class ISvcLocator;
template <class TYPE>
class SvcFactory;

/**  @class GaussRD GaussRD.h
 *
 *   Implementation of abstract Interfaces IGaussRDStr
 *   (for event-by-event communications with Geant4)
 *    and IGiGaSetUpSvc (for configuration of Geant4)
 *
 *    @author: Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class GaussRD : public Service, virtual public IGaussRDStr, virtual public IGaussRDCtr {
  /// friend factory
  friend class SvcFactory<GaussRD>;

  public:
  /// useful typedef
  typedef std::vector<std::string> Strings;

  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   initialize() override;
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   finalize  () override;

  //Implementation of the control interface IGaussRDCtr
  
  /** Registers a new event, returns false if the UD is already simulated and should be reused.
   *  Returns true if everything needs to be redone and deletes the internal storage objects.
   *
   *  @return bool
   */
  bool registerNewEvent() override;

  //Implementation of the storage interface IGaussRDStr

  /** Store simualted G4 underlying event for the underlying event
   *
   *  @param G4Event* event
   *  @return status code  
   */
  virtual StatusCode  saveJunkG4Event( G4Event* event ) override;

  /** Load the stored G4 event for the underlying event
   *
   *  @param G4Event*& event
   *  @return status code  
   */
  virtual StatusCode  loadJunkG4Event( G4Event* & event ) const override;

  /** Save the signal particle and its origin vertex for redecay
   *
   *  @param G4Event*& event
   *  @return status code  
   */
  virtual StatusCode  saveSignalGenInfo( HepMC::GenParticle* part, HepMC::GenVertex * vertex) override;

  /** Load the signal particle and its origin vertex for redecay
   *
   *  @param G4Event*& event
   *  @return status code  
   */
  virtual StatusCode  saveSignalGenInfo( HepMC::GenParticle*& part, HepMC::GenVertex *& vertex) const override;
  

  protected:
  /** standard constructor
   *  @see Service
   *  @param name instrance name
   *  @param svc  pointer to service locator
   */
  GaussRD(const std::string& name, ISvcLocator* svc);

  /// (virtual destructor)
  virtual ~GaussRD();

  private:

  //Counter and max event number
  size_t m_rd_counter;
  size_t m_max_rd_counter;
  
};

#endif  ///<  REDECAY_SERVICE_H

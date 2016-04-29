// $Id: IGaussRedecayCtr.h,v 0.1 2015-12-10 18:58:18 ibelyaev Exp $
// ============================================================================
// ============================================================================
//
// ============================================================================
#ifndef GAUSS_IGAUSSRDCTR_H
#define GAUSS_IGAUSSRDCTR_H 1
/// STD and STL
//#include   <utility>
/// Include files from the Framework
#include "GaudiKernel/IService.h"

/** @class IGaussRedecayCtr IGaussRedecayCtr.h GiGa/IGaussRedecayCtr.h
 *
 *
 *
 *
 *  @author Dominik Muller dominik.muller@cern.ch
 */

static const InterfaceID IID_IGaussRedecayCtr(5623694057, 1, 0);

class IGaussRedecayCtr : virtual public IService {
  friend class GaussRedecayCtrFilter;

  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGaussRedecayCtr; };

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode finalize() = 0;

  public:
  /** Registers a new event, returns false if the UD is already simulated and
   * should be reused.
   *  Returns true if everything needs to be redone and deletes the internal
   * storage objects.
   *
   *  @return bool
   */
  virtual bool registerNewEvent() = 0;
  virtual size_t numberOfRedecays() const = 0;

  /** Returns the phase of the redecay flow.
   * 0: Nominal simulation
   * 1: Generating a full new event
   * 2: Doing the signal (re)decay right now
   *
   *  @return int
   */
  virtual int getPhase() const = 0;
  virtual void setPhase(int p) = 0;

  /** Returns the redecay mode, used by the sorter
   * to decide what should be redecayed.
   * 0: Redecay signal only.
   * 1: Redecay everything of at least the signal mass.
   *
   *  @return int
   */
  virtual int getRedecayMode() const = 0;

  protected:
  /// virtual destructor
  virtual ~IGaussRedecayCtr(){};

  private:
};

#endif

#ifndef GAUSS_IGAUSSRDCTR_H
#define GAUSS_IGAUSSRDCTR_H 1
/// Include files from the Framework
#include "GaudiKernel/IService.h"
#include "GaudiKernel/System.h"

/** @class IGaussRedecayCtr IGaussRedecayCtr.h GiGa/IGaussRedecayCtr.h
 * Interface to the GaussRedecay service to control the program flow
 *
 *
 *
 *  @author Dominik Muller dominik.muller@cern.ch
 */

class GAUDI_API IGaussRedecayCtr : virtual public IService {
  friend class GaussRedecayCtrFilter;

  public:
  DeclareInterfaceID(IGaussRedecayCtr, 1, 0);

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

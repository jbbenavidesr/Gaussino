#pragma once

#include "GaudiKernel/IService.h"

//#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"


/** @class IBeamInfoSvc IBeamInfoSvc.h
 * Dummy service to store beam information.
 * Should be replaced by something better ...
 *
 *  @author Dominik Muller dominik.muller@cern.ch
 */

class GAUDI_API IBeamInfoSvc : virtual public IService {
  public:
  DeclareInterfaceID(IBeamInfoSvc, 1, 0);

  virtual StatusCode initialize() = 0;

  virtual StatusCode finalize() = 0;

  /// Mean number of interactions per event (nu)
  virtual double nu() const = 0;

  /// emittance
  virtual double emittance() const = 0;

  /// sigma X of the luminous region
  virtual double sigmaX() const = 0;

  /// sigma Y of the luminous region
  virtual double sigmaY() const = 0;

  /// sigma Z of the luminous region
  virtual double sigmaZ() const = 0;

  /// Smearing of the angle between the beams
  virtual double angleSmear() const = 0;

  /// Retrieve const  Beam energy
  virtual double energy() const = 0;

  /// Retrieve const  RMS of the bunch length
  virtual double sigmaS() const = 0;

  /// Retrieve const  Normalized emittance
  virtual double epsilonN() const = 0;

  /// Retrieve const  Revolution frequency
  virtual double revolutionFrequency() const = 0;

  /// Retrieve const  Total cross-section
  virtual double totalXSec() const = 0;

  /// Retrieve const  Horizontal crossing angle
  virtual double horizontalCrossingAngle() const = 0;

  /// Retrieve const  Vertical crossing angle
  virtual double verticalCrossingAngle() const = 0;

  /// Retrieve const  Horizontal beamline angle
  virtual double horizontalBeamlineAngle() const = 0;

  /// Retrieve const  Vertical beamline angle
  virtual double verticalBeamlineAngle() const = 0;

  /// Retrieve const  Beta star
  virtual double betaStar() const = 0;

  /// Retrieve const  Bunch spacing
  virtual double bunchSpacing() const = 0;

  /// Retrieve const  Luminous region mean position
  virtual const Gaudi::XYZPoint& beamSpot() const = 0;

  /// Retrieve const  Luminosity
  virtual double luminosity() const = 0;

  protected:
  /// virtual destructor
  virtual ~IBeamInfoSvc() = default;
};

/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once

#include "Gaudi/Property.h"
#include "GaudiKernel/Service.h"
#include "GenBeam/IBeamInfoSvc.h"

class BeamInfoSvc : public Service, virtual public IBeamInfoSvc {

public:
  BeamInfoSvc( const std::string& name, ISvcLocator* svcloc ) : Service( name, svcloc ){};

  virtual ~BeamInfoSvc() = default;
  StatusCode initialize() override;

  StatusCode finalize() override;

  StatusCode queryInterface( const InterfaceID& iid, void** pI ) override;

  /// Mean number of interactions per event (nu)
  virtual double nu() const override;

  /// emittance
  virtual double emittance() const override;

  /// sigma X of the luminous region
  virtual double sigmaX() const override;

  /// sigma Y of the luminous region
  virtual double sigmaY() const override;

  /// sigma Z of the luminous region
  virtual double sigmaZ() const override;

  /// Smearing of the angle between the beams
  virtual double angleSmear() const override;

  /// Retrieve const  Beam energy
  virtual double energy() const override;

  /// Retrieve const  RMS of the bunch length
  virtual double sigmaS() const override;

  /// Retrieve const  Normalized emittance
  virtual double epsilonN() const override;

  /// Retrieve const  Revolution frequency
  virtual double revolutionFrequency() const override;

  /// Retrieve const  Total cross-section
  virtual double totalXSec() const override;

  /// Retrieve const  Horizontal crossing angle
  virtual double horizontalCrossingAngle() const override;

  /// Retrieve const  Vertical crossing angle
  virtual double verticalCrossingAngle() const override;

  /// Retrieve const  Horizontal beamline angle
  virtual double horizontalBeamlineAngle() const override;

  /// Retrieve const  Vertical beamline angle
  virtual double verticalBeamlineAngle() const override;

  /// Retrieve const  Beta star
  virtual double betaStar() const override;

  /// Retrieve const  Bunch spacing
  virtual double bunchSpacing() const override;

  /// Retrieve const  Luminous region mean position
  virtual const Gaudi::XYZPoint& beamSpot() const override;

  /// Retrieve const  Luminosity
  virtual double luminosity() const override;

private:
  Gaudi::XYZPoint m_beamspot;

  Gaudi::Property<double> m_beamEnergy{ this, "BeamEnergy", 3.5 * Gaudi::Units::TeV };
  Gaudi::Property<double> m_bunchLengthRMS{ this, "BunchLengthRMS", 1 };
  Gaudi::Property<double> m_normalizedEmittance{ this, "NormalizedEmittance", 1. * Gaudi::Units::mm };
  Gaudi::Property<double> m_totalCrossSection{ this, "TotalCrossSection", 100. * Gaudi::Units::microbarn };
  Gaudi::Property<double> m_horizontalCrossingAngle{ this, "HorizontalCrossingAngle", 0.3 * Gaudi::Units::mrad };
  Gaudi::Property<double> m_verticalCrossingAngle{ this, "VerticalCrossingAngle", 0.3 * Gaudi::Units::mrad };
  Gaudi::Property<double> m_horizontalBeamlineAngle{ this, "HorizontalBeamlineAngle", 0.3 * Gaudi::Units::mrad };
  Gaudi::Property<double> m_verticalBeamlineAngle{ this, "VerticalBeamlineAngle", 0.3 * Gaudi::Units::mrad };
  Gaudi::Property<double> m_betaStar{ this, "BetaStar", 3. * Gaudi::Units::m };
  Gaudi::Property<double> m_bunchSpacing{ this, "BunchSpacing", 50. * Gaudi::Units::ns };
  Gaudi::Property<double> m_xLuminousRegion{ this, "XLuminousRegion", 0. };
  Gaudi::Property<double> m_yLuminousRegion{ this, "YLuminousRegion", 0. };
  Gaudi::Property<double> m_zLuminousRegion{ this, "ZLuminousRegion", 0. };
  Gaudi::Property<double> m_luminosity{ this, "Luminosity", 1.e32 / ( Gaudi::Units::cm2 * Gaudi::Units::s ) };
  Gaudi::Property<double> m_revolutionFrequency{ this, "RevolutionFrequency", 11.245 * Gaudi::Units::kilohertz };
};

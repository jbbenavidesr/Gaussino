#pragma once
// Include files
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimInterfaces/IGaussinoTool.h"

/** @class GiGaRegionTool GiGaRegionTool.h Components/GiGaRegionTool.h
 *
 *  The simple tool that creates and configures the 'Region'
 *  One tool per region
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-05-30
 *  @author Dominik Muller
 */
class GiGaRegionTool : public extends<GaudiTool, IGaussinoTool>
{
public:
  using extends::extends;

  /** the only one method
   *  @see GiGaToolBase
   *  @see IGiGaTool
   *  @param region region name
   *  @return status code
   */
  StatusCode process( const std::string& region = "" ) const override;

private:
  typedef std::vector<std::string> Volumes;

  Gaudi::Property<std::string> m_region{this, "Region", "UNKNOWN"};
  Gaudi::Property<Volumes> m_volumes{this, "Volumes", {}};

  // cut for gammas
  Gaudi::Property<double> m_gamma{this, "CutForGamma", 10 * CLHEP::mm};
  // cut for electrons
  Gaudi::Property<double> m_electron{this, "CutForElectron", 10 * CLHEP::mm};
  // cut for positrons
  Gaudi::Property<double> m_positron{this, "CutForPositron", 10 * CLHEP::mm};

  Gaudi::Property<bool> m_overwrite{this, "Overwrite", true};
};

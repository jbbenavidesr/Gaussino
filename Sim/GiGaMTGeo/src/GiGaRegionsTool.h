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
// Include files
// Include files
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimInterfaces/IGaussinoTool.h"

// forward declarations
class IRegionsDefinitionSvc ;

/** @class GiGaRegionsTool GiGaRegionsTool.h Components/GiGaRegionsTool.h
 *
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-06-04
 *  @author Dominik Muller
 */

class GiGaRegionsTool : public extends<GaudiTool, IGaussinoTool>
{
public:
  using extends::extends;

  /** the only one method
   *  @see GiGaToolBase
   *  @see IGiGaTool
   *  @param region region name
   *  @return status code
   */
  StatusCode process ( const std::string& region  = "" ) const override;

private:

  // default constructor  is disabled
  GiGaRegionsTool();
  // copy    constructor  is disabled
  GiGaRegionsTool           ( const GiGaRegionsTool& ) ;
  // assignement operator is disabled
  GiGaRegionsTool& operator=( const GiGaRegionsTool& ) ;

private:

  ServiceHandle<IRegionsDefinitionSvc> m_simSvc{this, "RegionsDefinitionService", "RegionsDefinitionSvc"};
  Gaudi::Property<bool> m_overwrite{this, "Overwrite", true};

};

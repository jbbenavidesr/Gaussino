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
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimInterfaces/IGaussinoTool.h"
// SimSvc
class IRegionsDefinitionSvc;
class G4LogicalVolume;
namespace Gaussino
{
  class UserLimits;
}
// ============================================================================

/** @class GiGaSetSimAttributes GiGaSetSimAttributes.h
 *
 *  The simplest tool which attach the simulation attributes
 *  for G4 Logical Volume
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 *  @author Dominik Muller
 */
class GiGaSetSimAttributes : public extends<GaudiTool, IGaussinoTool>
{
  // friend factory for instantiation
  //  friend class GiGaFactory<GiGaSetSimAttributes>;
public:
  using extends::extends;

  /** the only one method
   *  @see IGiGaTool
   *  @param volume volume name
   *  @return status code
   */
  StatusCode process( const std::string& volume ) const override;

protected:
  /** find g4 volume by name
   *  @param name name of the volume
   */
  G4LogicalVolume* g4volume( const std::string& adddress ) const;

  /** set user Limits for the given logical volume
   *  and propagate it to all daughetr volumes
   *  @param lv logicla volume
   *  @param ul user limits
   *  @return status code
   */
  StatusCode setUserLimits( G4LogicalVolume* lv, const Gaussino::UserLimits& ul ) const;

  // policy for overwtiting the existing limits
  inline bool overwrite() const { return m_overwrite; }

private:
  ServiceHandle<IRegionsDefinitionSvc> m_simSvc{this, "RegionsDefinitionService", "RegionsDefinitionSvc"};
  Gaudi::Property<bool> m_overwrite{this, "Overwrite", true};
};

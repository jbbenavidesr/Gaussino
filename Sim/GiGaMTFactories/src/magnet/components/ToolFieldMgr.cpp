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
#include "Geant4/G4MagneticField.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/Magnet/FieldMgrBase.h"
#include "GaudiKernel/ToolHandle.h"

/** @class ToolFieldMgr
 *
 *  Tool that interfaces the call to field() to an externally provided factory for G4MagneticField.
 *
 *  @author Dominik Muller
 *  @date   2019-06-05
 */
class ToolFieldMgr : public Gaussino::FieldMgrBase
{
public:
  using Gaussino::FieldMgrBase::FieldMgrBase;

  G4MagneticField* field() const override { return m_magfield_constructor->construct(); };

  virtual ~ToolFieldMgr() = default;

private:
  ToolHandle<GiGaFactoryBase<G4MagneticField>> m_magfield_constructor{this, "MagneticField", "MagFieldFromSvc"};
};

DECLARE_COMPONENT( ToolFieldMgr )

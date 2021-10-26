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
#include "GiGaMTFactories/Magnet/FieldMgrBase.h"
class G4ChordFinder;

/** @class ZeroFieldMgr
 *
 *  Simple magnetic field manager that has a G4MagneticField nullptr.
 *  This effectively turns of the field in the assigned volumes.
 *  Useful if there is a global magnetic field that should be
 *  ignored in only a few volumes
 *
 *  @author Dominik Muller
 *  @date   2019-06-04
 */
class ZeroFieldMgr : public Gaussino::FieldMgrBase
{
public:
  using Gaussino::FieldMgrBase::FieldMgrBase;

  G4MagneticField* field() const override {return nullptr;};

  virtual ~ZeroFieldMgr( ) = default;

};

DECLARE_COMPONENT( ZeroFieldMgr )

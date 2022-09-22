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
#include "NewRnd/RndAlgSeeder.h"
#include "GaudiAlg/GaudiTool.h"

#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/RanshiEngine.h"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/DRand48Engine.h"
#include "CLHEP/Random/Hurd160Engine.h"
#include "CLHEP/Random/Hurd288Engine.h"

template <typename ENGINE>
class CLHEP_ENGINE : public extends<GaudiTool, IExtEngine>
{
  public:
  static_assert( std::is_base_of<CLHEP::HepRandomEngine, ENGINE>::value,
                 "Random engine must inherit from CLHEP::HepRandomEngine" );
  using extends::extends;
  virtual CLHEP::HepRandomEngine* construct() const override { return new ENGINE{}; }
};

typedef CLHEP_ENGINE<CLHEP::MixMaxRng> MixMaxRng;
DECLARE_COMPONENT_WITH_ID( MixMaxRng, "MixMaxRng" )
typedef CLHEP_ENGINE<CLHEP::RanluxEngine> RanluxEngine;
DECLARE_COMPONENT_WITH_ID( RanluxEngine, "RanluxEngine" )
typedef CLHEP_ENGINE<CLHEP::MTwistEngine> MTwistEngine;
DECLARE_COMPONENT_WITH_ID( MTwistEngine, "MTwistEngine" )
typedef CLHEP_ENGINE<CLHEP::RanshiEngine> RanshiEngine;
DECLARE_COMPONENT_WITH_ID( RanshiEngine, "RanshiEngine" )
typedef CLHEP_ENGINE<CLHEP::RanecuEngine> RanecuEngine;
DECLARE_COMPONENT_WITH_ID( RanecuEngine, "RanecuEngine" )
typedef CLHEP_ENGINE<CLHEP::DRand48Engine> DRand48Engine;
DECLARE_COMPONENT_WITH_ID( DRand48Engine, "DRand48Engine" )
typedef CLHEP_ENGINE<CLHEP::Hurd160Engine> Hurd160Engine;
DECLARE_COMPONENT_WITH_ID( Hurd160Engine, "Hurd160Engine" )
typedef CLHEP_ENGINE<CLHEP::Hurd288Engine> Hurd288Engine;
DECLARE_COMPONENT_WITH_ID( Hurd288Engine, "Hurd288Engine" )

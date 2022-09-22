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
#include "GiGaMTPhysFactories/GiGaMTG4PhysicsFactory.h"

// All includes I could find that definethe physics lists
// that are directly provided by Geant4

#include "FTFP_BERT.hh"
#include "FTFP_BERT_ATL.hh"
#include "FTFP_BERT_HP.hh"
#include "FTFP_BERT_TRV.hh"
#include "FTFP_INCLXX.hh"
#include "FTFP_INCLXX_HP.hh"
#include "FTFQGSP_BERT.hh"
#include "FTF_BIC.hh"
#include "INCLXXPhysicsListHelper.hh"
#include "LBE.hh"
#include "NuBeam.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_AllHP.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_FTFP_BERT.hh"
#include "QGSP_INCLXX.hh"
#include "QGSP_INCLXX_HP.hh"
#include "QGS_BIC.hh"
#include "Shielding.hh"
#include "ShieldingLEND.hh"

typedef GiGaMTG4PhysicsFactory<FTFP_BERT> GiGaMT_FTFP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_BERT, "GiGaMT_FTFP_BERT" )

typedef GiGaMTG4PhysicsFactory<QGSP_BERT> GiGaMT_QGSP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_BERT, "GiGaMT_QGSP_BERT" )

typedef GiGaMTG4PhysicsFactory<FTF_BIC> GiGaMT_FTF_BIC;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTF_BIC, "GiGaMT_FTF_BIC" )

typedef GiGaMTG4PhysicsFactory<FTFP_BERT_ATL> GiGaMT_FTFP_BERT_ATL;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_BERT_ATL, "GiGaMT_FTFP_BERT_ATL" )

typedef GiGaMTG4PhysicsFactory<FTFP_BERT_HP> GiGaMT_FTFP_BERT_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_BERT_HP, "GiGaMT_FTFP_BERT_HP" )

typedef GiGaMTG4PhysicsFactory<FTFP_INCLXX> GiGaMT_FTFP_INCLXX;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_INCLXX, "GiGaMT_FTFP_INCLXX" )

typedef GiGaMTG4PhysicsFactory<FTFP_INCLXX_HP> GiGaMT_FTFP_INCLXX_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_INCLXX_HP, "GiGaMT_FTFP_INCLXX_HP" )

typedef GiGaMTG4PhysicsFactory<FTFQGSP_BERT> GiGaMT_FTFQGSP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFQGSP_BERT, "GiGaMT_FTFQGSP_BERT" )

typedef GiGaMTG4PhysicsFactory<QGSP_INCLXX> GiGaMT_QGSP_INCLXX;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_INCLXX, "GiGaMT_QGSP_INCLXX" )

typedef GiGaMTG4PhysicsFactory<QGSP_INCLXX_HP> GiGaMT_QGSP_INCLXX_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_INCLXX_HP, "GiGaMT_QGSP_INCLXX_HP" )

typedef GiGaMTG4PhysicsFactory<FTFP_INCLXX> GiGaMT_FTFP_INCLXX;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_INCLXX, "GiGaMT_FTFP_INCLXX" )

typedef GiGaMTG4PhysicsFactory<FTFP_INCLXX_HP> GiGaMT_FTFP_INCLXX_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_INCLXX_HP, "GiGaMT_FTFP_INCLXX_HP" )

typedef GiGaMTG4PhysicsFactory<FTFP_BERT_TRV> GiGaMT_FTFP_BERT_TRV;
DECLARE_COMPONENT_WITH_ID( GiGaMT_FTFP_BERT_TRV, "GiGaMT_FTFP_BERT_TRV" )

typedef GiGaMTG4PhysicsFactory<LBE> GiGaMT_LBE;
DECLARE_COMPONENT_WITH_ID( GiGaMT_LBE, "GiGaMT_LBE" )

typedef GiGaMTG4PhysicsFactory<NuBeam> GiGaMT_NuBeam;
DECLARE_COMPONENT_WITH_ID( GiGaMT_NuBeam, "GiGaMT_NuBeam" )

typedef GiGaMTG4PhysicsFactory<QGSP_BERT_HP> GiGaMT_QGSP_BERT_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_BERT_HP, "GiGaMT_QGSP_BERT_HP" )

typedef GiGaMTG4PhysicsFactory<QGSP_BIC> GiGaMT_QGSP_BIC;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_BIC, "GiGaMT_QGSP_BIC" )

typedef GiGaMTG4PhysicsFactory<QGSP_BIC_AllHP> GiGaMT_QGSP_BIC_AllHP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_BIC_AllHP, "GiGaMT_QGSP_BIC_AllHP" )

typedef GiGaMTG4PhysicsFactory<QGSP_BIC_HP> GiGaMT_QGSP_BIC_HP;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_BIC_HP, "GiGaMT_QGSP_BIC_HP" )

typedef GiGaMTG4PhysicsFactory<QGSP_FTFP_BERT> GiGaMT_QGSP_FTFP_BERT;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_FTFP_BERT, "GiGaMT_QGSP_FTFP_BERT" )

typedef GiGaMTG4PhysicsFactory<QGS_BIC> GiGaMT_QGS_BIC;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGS_BIC, "GiGaMT_QGS_BIC" )

typedef GiGaMTG4PhysicsFactory<QGSP_INCLXX> GiGaMT_QGSP_INCLXX;
DECLARE_COMPONENT_WITH_ID( GiGaMT_QGSP_INCLXX, "GiGaMT_QGSP_INCLXX" )

typedef GiGaMTG4PhysicsFactory<Shielding> GiGaMT_Shielding;
DECLARE_COMPONENT_WITH_ID( GiGaMT_Shielding, "GiGaMT_Shielding" )

typedef GiGaMTG4PhysicsFactory<ShieldingLEND> GiGaMT_ShieldingLEND;
DECLARE_COMPONENT_WITH_ID( GiGaMT_ShieldingLEND, "GiGaMT_ShieldingLEND" )

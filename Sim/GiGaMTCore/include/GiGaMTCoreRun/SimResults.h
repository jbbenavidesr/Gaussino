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
#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"

namespace Gaussino {
  typedef std::tuple<G4EventProxyPtr, Gaussino::MCTruthPtr> GiGaSimReturn;
  typedef std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> GiGaSimReturns;
} // namespace Gaussino

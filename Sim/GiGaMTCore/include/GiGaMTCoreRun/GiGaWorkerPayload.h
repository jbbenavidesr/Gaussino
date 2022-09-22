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

#include "GiGaMTCoreRun/G4EventProxy.h"
#include "GiGaMTCoreRun/MCTruthConverter.h"
#include "Utils/blockingconcurrentqueue.h"
#include <future>
#include <vector>
#include <optional>
#include "NewRnd/RndCommon.h"
#include "GiGaMTCoreRun/SimResults.h"

// Collection of wrapper classes to pass information between the
// Gaudi and G4 threads mediated by a queue, somehow, magically ...

namespace HepMC
{
  class GenEvent;
} /* HepMC */
namespace CLHEP
{
  class HepRandomEngine;
} /* CLHEP */

// TODO: Just wrapping it into a tuple does not seem to work because it does not find a suitable copy/move constructor
// for the queue. This is, I think, caused by the default copy constructor of a tuple failing to be valid due to promise
// which cannot be copied and the default move constructor is not noexcept. So we are trying a pointer instead...
typedef std::tuple<Gaussino::MCTruthConverterPtr, HepRandomEnginePtr, std::promise<Gaussino::GiGaSimReturn>*> GiGaWorkerPayload;

typedef std::optional<GiGaWorkerPayload> GiGaWorkerPayloadOpt;

typedef moodycamel::BlockingConcurrentQueue<GiGaWorkerPayloadOpt> GiGaPayloadQueue;

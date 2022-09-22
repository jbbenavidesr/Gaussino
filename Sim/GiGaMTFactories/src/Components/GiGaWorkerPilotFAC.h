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
#include "GiGaMTCoreRun/GiGaWorkerPilot.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

// Factory class implemented as a GaudiTool that creates and configures the
// the individual worker thread pilots. As these pilots are responsible for
// managing the WorkerRunManager, the Gaudi side of this configuration also happens
// here. TODO: rethink this ...

class GiGaWorkerPilotFAC : public extends<GiGaTool, GiGaFactoryBase<GiGaWorkerPilot>>
{
public:
  using extends::extends;
  Gaudi::Property<bool> m_track_structure{this, "PrintMCTruth", false};

  GiGaWorkerPilot* construct() const override;

private:
  static std::atomic_uint n_created;
};

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

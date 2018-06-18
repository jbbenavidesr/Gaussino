#include "GiGaFactoryBase.h"
#include "GiGaMTCore/GiGaWorkerPilot.h"

// Factory class implemented as a GaudiTool that creates and configures the
// the individual worker thread pilots. As these pilots are responsible for
// managing the WorkerRunManager, the Gaudi side of this configuration also happens
// here. TODO: rethink this ...

class GiGaWorkerPilotFAC : public GiGaFactoryBase<GiGaWorkerPilot>
{
  public:
  GiGaWorkerPilotFAC( const std::string& type, const std::string& name, const IInterface* parent );
  Gaudi::Property<size_t> m_blub{this, "SomeProp", "Hello"};

  GiGaWorkerPilot* construct() const override;
  private:
  static std::atomic_uint n_created;
};

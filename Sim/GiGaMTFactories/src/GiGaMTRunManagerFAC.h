#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTCore/GiGaMTRunManager.h"

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaMTRunManagerFAC : public GiGaFactoryBase<GiGaMTRunManager>
{
  public:
  using GiGaFactoryBase<GiGaMTRunManager>::GiGaFactoryBase;
  Gaudi::Property<std::string> m_someprop{this, "SomeProp", "Hello"};

  GiGaMTRunManager* construct() const override;
  private:
  static std::atomic_bool created;
};

#include "GiGaMTCoreRun/GiGaRunActionCommand.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaRunActionCommandFAC : public extends<GiGaTool, GiGaFactoryBase<G4UserRunAction>>
{
public:
  Gaudi::Property<std::vector<std::string>> m_beginCmds{this, "BeginOfRunCommands", {}};
  Gaudi::Property<std::vector<std::string>> m_endCmds{this, "EndOfRunCommands", {}};
  using extends::extends;

  G4UserRunAction* construct() const override;

private:
  static std::atomic_bool created;
};

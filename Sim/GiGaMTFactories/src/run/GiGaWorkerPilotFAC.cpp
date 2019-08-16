#include "GiGaWorkerPilotFAC.h"

DECLARE_COMPONENT( GiGaWorkerPilotFAC )

/*static*/ std::atomic_uint GiGaWorkerPilotFAC::n_created{0};

GiGaWorkerPilot* GiGaWorkerPilotFAC::construct() const
{
  n_created++;
  debug() << "Creating instance number " << std::to_string( n_created ) << endmsg;

  auto pilot = new GiGaWorkerPilot{};
  pilot->SetMessageInterface( message_interface() );
  pilot->iWorker = n_created - 1;
  return pilot;
}

#include "GiGaMTDD4hep/Utilities.h"

dd4hep::PrintLevel DD4hepGaudiMessaging::Convert(MSG::Level level) {
  switch (level) {
    case MSG::VERBOSE:
      return dd4hep::VERBOSE;
    case MSG::DEBUG:
      return dd4hep::DEBUG;
    case MSG::INFO:
      return dd4hep::INFO;
    case MSG::WARNING:
      return dd4hep::WARNING;
    case MSG::ERROR:
      return dd4hep::ERROR;
    case MSG::FATAL:
      return dd4hep::FATAL;
    case MSG::ALWAYS:
      return dd4hep::ALWAYS;
    default:
      // Just pass the enum value of level converted as a backup solution
      // Should not get to here if MSG::Level as defined in Gaudi are used
      return static_cast<dd4hep::PrintLevel>(level);
  }
}

MSG::Level DD4hepGaudiMessaging::Convert(dd4hep::PrintLevel level) {
  switch (level) {
    case dd4hep::VERBOSE:
      return MSG::VERBOSE;
    case dd4hep::DEBUG:
      return MSG::DEBUG;
    case dd4hep::INFO:
      return MSG::INFO;
    case dd4hep::WARNING:
      return MSG::WARNING;
    case dd4hep::ERROR:
      return MSG::ERROR;
    case dd4hep::FATAL:
      return MSG::FATAL;
    case dd4hep::ALWAYS:
      return MSG::ALWAYS;
    default:
      // Just pass the enum value of level converted as a backup solution
      // Should not get to here if only defined dd4hep::PrintLevel are used
      return static_cast<MSG::Level>(level);
  }
}

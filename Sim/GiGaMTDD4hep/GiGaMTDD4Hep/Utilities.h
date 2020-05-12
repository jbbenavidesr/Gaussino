#include "DD4hep/Printout.h"
#include "GaudiKernel/IMessageSvc.h"

namespace DD4hepGaudiMessaging {
/*Two helper functions to convert the output/print levels between Gaudi and
 * DD4hep. A second look at it revealed that in principle, a static_cast would
 * be sufficient but implementation kept as is for now to allow potential tuning
 * later.*/
dd4hep::PrintLevel Convert(MSG::Level level);
MSG::Level Convert(dd4hep::PrintLevel level);
}  // namespace DD4HepGaudiMessaging

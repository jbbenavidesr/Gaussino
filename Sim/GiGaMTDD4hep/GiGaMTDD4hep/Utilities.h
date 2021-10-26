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
#include "DD4hep/Printout.h"
#include "GaudiKernel/IMessageSvc.h"

namespace DD4hepGaudiMessaging {
/*Two helper functions to convert the output/print levels between Gaudi and
 * DD4hep. A second look at it revealed that in principle, a static_cast would
 * be sufficient but implementation kept as is for now to allow potential tuning
 * later.*/
dd4hep::PrintLevel Convert(MSG::Level level);
MSG::Level Convert(dd4hep::PrintLevel level);
}  // namespace DD4hepGaudiMessaging

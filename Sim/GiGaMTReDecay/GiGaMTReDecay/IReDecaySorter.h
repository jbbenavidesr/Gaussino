#pragma once

#include "GaudiKernel/IAlgTool.h"
#include "HepMC3/GenParticle_fwd.h"
#include "HepMCUser/typedefs.h"

class IReDecaySorter: public extend_interfaces<IAlgTool> {
  public:
    DeclareInterfaceID( IReDecaySorter, 1, 0 );
    virtual bool FlagAndRemoveReDecays(std::vector<HepMC3::GenEventPtr> &) const = 0;
};

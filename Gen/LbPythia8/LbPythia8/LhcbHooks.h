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
#ifndef LBPYTHIA8_LHCBHOOKS_H
#define LBPYTHIA8_LHCBHOOKS_H 1

#include "Pythia8/Pythia.h"

/** @class LhcbHooks LhcbHooks.h LbPythia8/LhcbHooks.h
 *
 *  Custom LHCb user hooks used to modify default Pythia 8 event generation.
 *  Currently the hooks are used to provide pT damping of the QCD 2 -> 2
 *  processes.
 *
 *  @author Philip Ilten
 *  @date   2014-04-06
 */

namespace Pythia8 {
  class LhcbHooks : public UserHooks {

  public:

    /// Constructor.
    LhcbHooks() {isInit = false;}

    /// Modifiy cross-section.
    bool canModifySigma() override {return true;}

    /// Cross-section weight.
    double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
				   const PhaseSpace* phaseSpacePtr, bool) override;

  private:

    // Members.
    bool   isInit;      ///< Flag whether the object has been initialized.
    double pT20;        ///< Stored value of the pT damping parameter.
    AlphaStrong alphaS; ///< Stored value of alpha strong.
  };
}

#endif // LBPYTHIA8_LHCBHOOKS_H

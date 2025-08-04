/*****************************************************************************\
* (c) Copyright 2000-2019 CERN for the benefit of the LHCb Collaboration      *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once
#include <ostream>

namespace Gsino {

  // Namespace for locations in TDS
  namespace SimHeaderLocation {
    inline const std::string Default = "Sim/Header";
  }

  /** @class SimHeader SimHeader.h
   *
   * Header for event generator information
   *
   * @author Witek Pokorski
   *
   */

  class SimHeader {
  public:
    /// Constructor
    SimHeader() = default;

    /// Retrieve const  Run number
    unsigned int runNumber() const { return m_runNumber; }

    /// Update  Run number
    void setRunNumber( unsigned int value ) { m_runNumber = value; }

    /// Retrieve const  Event number
    long long evtNumber() const { return m_evtNumber; }

    /// Update  Event number
    void setEvtNumber( long long value ) { m_evtNumber = value; }

  private:
    unsigned int m_runNumber = 0;  ///< Run number
    long long    m_evtNumber = 0; ///< Event number

  }; // class SimHeader

} // namespace Gsino

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

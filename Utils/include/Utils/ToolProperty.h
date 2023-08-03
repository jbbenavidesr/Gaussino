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
#pragma once
#include "GaudiKernel/GaudiHandle.h"
#include <string>
#include <vector>
namespace Gaudi::Details {
  class PropertyBase;
}

template <typename TOOL>
class tool_array_setter {
public:
  tool_array_setter( ToolHandleArray<TOOL>& tarray, const std::vector<std::string>& names )
      : m_tarray{ tarray }, m_names{ names } {}
  void operator()( Gaudi::Details::PropertyBase& ) {
    for ( auto name : m_names ) {
      if ( !name.empty() ) { m_tarray.push_back( name ); }
    }
  }

private:
  ToolHandleArray<TOOL>&          m_tarray;
  const std::vector<std::string>& m_names;
};

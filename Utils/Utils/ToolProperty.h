#pragma once
#include <string>
#include <vector>
#include "GaudiKernel/GaudiHandle.h"
namespace Gaudi::Details {
  class PropertyBase;
}

template <typename TOOL>
class tool_array_setter
{
public:
  tool_array_setter( ToolHandleArray<TOOL>& tarray, const std::vector<std::string>& names ) : m_tarray{tarray}, m_names{names} {}
  void operator()( Gaudi::Details::PropertyBase& )
  {
    for ( auto name : m_names ) {
      if ( !name.empty() ) {
        m_tarray.push_back( name );
      }
    }
  }

private:
  ToolHandleArray<TOOL>& m_tarray;
  const std::vector<std::string>& m_names;
};

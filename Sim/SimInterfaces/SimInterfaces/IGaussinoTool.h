#pragma once
#include "GaudiKernel/IAlgTool.h"

/** @class IGiGaTool IGiGaTool.h GiGa/IGiGaTool.h
 *
 *  An abstract interface for generic Tool
 *
 *  @author Vanay BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 *  @author Dominik Muller
 */
class IGaussinoTool : virtual public IAlgTool
{
public:
  DeclareInterfaceID(IGaussinoTool,1,0);
  
  /** the only one essential method 
   *  @param par parameters 
   *  @return status code
   */
  virtual StatusCode process ( const std::string& parameter = "" ) const = 0 ;
  
protected:
  virtual ~IGaussinoTool() = default ;
};

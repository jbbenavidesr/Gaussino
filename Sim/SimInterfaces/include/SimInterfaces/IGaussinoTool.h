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
#include "GaudiKernel/IAlgTool.h"

/** @class IGiGaTool IGiGaTool.h GiGa/IGiGaTool.h
 *
 *  An abstract interface for generic Tool
 *
 *  @author Vanay BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 *  @author Dominik Muller
 */
class IGaussinoTool : virtual public IAlgTool {
public:
  DeclareInterfaceID( IGaussinoTool, 1, 0 );

  /** the only one essential method
   *  @param par parameters
   *  @return status code
   */
  virtual StatusCode process( const std::string& parameter = "" ) const = 0;

protected:
  virtual ~IGaussinoTool() = default;
};

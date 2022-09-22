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
// $Id: StreamForGenerator.h,v 1.2 2007-10-11 13:23:26 robbep Exp $
#ifndef GENERATORS_STREAMFORGENERATOR_H 
#define GENERATORS_STREAMFORGENERATOR_H 1

// Include files
#include "GaudiKernel/MsgStream.h"

/** @class StreamForGenerator StreamForGenerator.h Generators/StreamForGenerator.h
 *  Simple class to interface with Gaudi Output MsgStream
 *
 *  @author Patrick Robbe
 *  @date   2007-10-10
 */
class StreamForGenerator {
public:

  static MsgStream *& getStream() ;

protected:

private:
  static MsgStream * s_gaudiStream ;
};
#endif 

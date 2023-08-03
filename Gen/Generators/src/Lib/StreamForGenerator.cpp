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
// $Id: StreamForGenerator.cpp,v 1.2 2007-10-11 13:23:27 robbep Exp $
// Include files

// local
#include "Generators/StreamForGenerator.h"

MsgStream* StreamForGenerator::s_gaudiStream = 0;

MsgStream*& StreamForGenerator::getStream() { return s_gaudiStream; }

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
#ifndef GENERATORS_LBPHOTOS_H
#define GENERATORS_LBPHOTOS_H 1

// Include files

/** @class LbPhotos LbPhotos.h Generators/LbPhotos.h
 *
 *
 *  @author Patrick Robbe
 *  @date   2013-05-31
 */
class LbPhotos {
public:
  /// Set output unit of Photos
  static void setOutputUnit( int outputUnit );
};
#endif // GENERATORS_LBPHOTOS_H

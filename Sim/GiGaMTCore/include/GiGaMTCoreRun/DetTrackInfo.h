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
// $Id: DetTrackInfo.h,v 1.1 2003-07-28 10:26:22 witoldp Exp $
#ifndef GAUSSTOOLS_DETTRACKINFO_H
#define GAUSSTOOLS_DETTRACKINFO_H 1

// Include files

/** @class DetTrackInfo DetTrackInfo.h GaussTools/DetTrackInfo.h
 *
 *
 *  @author Witold POKORSKI
 *  @date   2003-07-26
 */
class DetTrackInfo {
public:
  /// Standard constructor
  DetTrackInfo(){};

  virtual ~DetTrackInfo(){}; ///< Destructor

protected:
private:
};
#endif // GAUSSTOOLS_DETTRACKINFO_H

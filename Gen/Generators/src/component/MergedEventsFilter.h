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
// $Id: MergedEventsFilter.h,v 1.1 2008-05-06 08:27:55 gcorti Exp $
#ifndef GENERATORS_MERGEDEVENTSFILTER_H
#define GENERATORS_MERGEDEVENTSFILTER_H 1

// Include files
// from Gaudi
#include "Defaults/Locations.h"
#include "Event/GenCollision.h"
#include "GaudiAlg/Consumer.h"
#include "HepMC3/GenEvent.h"

class IFullGenEventCutTool;

/** @class MergedEventsFilter MergedEventsFilter.h
 * component/MergedEventsFilter.h
 *
 *
 *  @author Gloria CORTI
 *  @date   2008-04-30
 */
class MergedEventsFilter
    : public Gaudi::Functional::Consumer<void(
          const HepMC3::GenEventPtrs&, const LHCb::GenCollisions&)> {
  ToolHandle<IFullGenEventCutTool> m_fullGenEventCutTool{
      this, "FullGenEventCutTool", ""};

  public:
  /// Standard constructor
  MergedEventsFilter(const std::string& name, ISvcLocator* pSvcLocator)
      : Consumer(name, pSvcLocator,
                 {{KeyValue{"HepMCEventLocation",
                            Gaussino::HepMCEventLocation::Default},
                   KeyValue{"GenCollisions",
                            LHCb::GenCollisionLocation::Default}}}) {}

  virtual void operator()(const HepMC3::GenEventPtrs&,
                          const LHCb::GenCollisions&) const override;
};
#endif  // GENERATORS_MERGEDEVENTSFILTER_H

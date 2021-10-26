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
#include "BaseSorter.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Relatives.h"
#include "HepMCUtils/HepMCUtils.h"

#include "HepMCUser/Status.h"

void BaseSorter::RecursiveTagger(HepMC3::GenParticlePtr part) const {
  if (part->status() != HepMC3::Status::ChildOfReDecay) {
    part->set_status(HepMC3::Status::ReDecay);
  }
  auto ev = part->end_vertex();
  if (ev) {
    for (auto child : HepMC3::Relatives::CHILDREN(part)) {
      child->set_status(HepMC3::Status::ChildOfReDecay);
      RecursiveTagger(child);
    }
  }
}

void BaseSorter::RemoveDecayTrees(std::vector<HepMC3::GenEventPtr> & events) const {
  std::set<HepMC3::GenParticlePtr> to_delete;
  for(auto & evt: events){
    for(auto& part: evt->particles()){
      if (part->status() == HepMC3::Status::ReDecay) {
        to_delete.insert(part);
      }
    }
  }
  for(auto & part: to_delete){
    HepMCUtils::RemoveDaughters(part);
    part->set_status(HepMC3::Status::ReDecay);
  }
}

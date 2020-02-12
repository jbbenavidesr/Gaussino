#include "BaseSorter.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Relatives.h"
#include "HepMCUtils/HepMCUtils.h"

void BaseSorter::RecursiveTagger(HepMC3::GenParticlePtr part) const {
  if (part->status() != 1043) {
    part->set_status(1042);
  }
  auto ev = part->end_vertex();
  if (ev) {
    for (auto child : HepMC3::Relatives::CHILDREN(part)) {
      child->set_status(1043);
      RecursiveTagger(child);
    }
  }
}

void BaseSorter::RemoveDecayTrees(std::vector<HepMC3::GenEventPtr> & events) const {
  std::set<HepMC3::GenParticlePtr> to_delete;
  for(auto & evt: events){
    for(auto& part: evt->particles()){
      if (part->status() == 1043) {
        to_delete.insert(part);
      }
    }
  }
  for(auto & part: to_delete){
    HepMCUtils::RemoveDaughters(part);
    part->set_status(1043);
  }
};

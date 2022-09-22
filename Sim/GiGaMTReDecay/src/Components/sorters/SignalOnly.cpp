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
#include "HepMCUser/VertexAttribute.h"
#include "Defaults/HepMCAttributes.h"
#include "HepMCUtils/PrintDecayTree.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalOnly
//
// 2020-02-10: Dominik Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
class SignalOnly : public BaseSorter
{

public:
  /// Standard constructor
  using BaseSorter::BaseSorter;
  virtual bool FlagAndRemoveReDecays(std::vector<HepMC3::GenEventPtr> &) const override;

};

DECLARE_COMPONENT( SignalOnly )

bool SignalOnly::FlagAndRemoveReDecays(std::vector<HepMC3::GenEventPtr> & events) const {
  bool found = false;
  for(auto & evt: events){
    auto sig_vtx_attr = evt->attribute<HepMC3::VertexAttribute>(Gaussino::HepMC::Attributes::SignalProcessVertex);
    if(sig_vtx_attr.get() && sig_vtx_attr->value()){
      auto sig_vtx = sig_vtx_attr->value();
      if(sig_vtx->particles_in().size() == 1){
        found = true;
        RecursiveTagger( *std::begin(sig_vtx->particles_in()));
        if ( msgLevel( MSG::DEBUG ) ) {
          debug() << "After flagging" << endmsg;
          debug() << PrintDecay(*std::begin(sig_vtx->particles_in())) << endmsg;
        }
        
      }
    }
  }
  // FIXME: Not removing decays of redecay particles here so the monitoring tools
  // work as before. Future plan should extend monitoring tool to include redecay
  // and nominal event as before, for now just skips monitoring in case it isn't an original event
  // RemoveDecayTrees(events);

  return found;
}

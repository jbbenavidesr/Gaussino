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
#include "HepMCUtils/PrintDecayTree.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include <sstream>

// from Gaudi
#include "HepMCUser/Status.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

std::string PrintDecay( const HepMC3::ConstGenParticlePtr& part, int level, const LHCb::IParticlePropertySvc* ppsvc,
                        IDs* usedid ) {
  IDs* tmp{ nullptr };
  if ( !usedid ) {
    tmp    = new IDs{};
    usedid = tmp;
  }
  std::string space = "";
  for ( int i = 0; i < level; i++ ) { space += "|---> "; }
  using HepMC3::operator<<;
  std::stringstream outstream;
  outstream << space;
  if ( ppsvc ) {
    if ( auto tmp = ppsvc->find( LHCb::ParticleID( part->pdg_id() ) ); tmp ) {
      outstream << tmp->name();
    } else {
      outstream << "UNKNOWN(" << part->pdg_id() << ")";
    }
  } else {
    outstream << part->pdg_id();
  }
  outstream << " -> #" << part->id() << ", " << HepMC3::to_status_type( part->status() );
  outstream << " Attributes {";
  for ( auto& atts : part->attribute_names() ) { outstream << atts << ","; }
  outstream << "} \n";
  if ( auto ev = part->end_vertex(); ev ) {
    if ( usedid->count( ev->id() ) > 0 ) {
      outstream << space << " *** Already printed this vertex *** \n";
    } else {
      usedid->insert( ev->id() );
      for ( auto p : part->children() ) { outstream << PrintDecay( p, level + 1, ppsvc, usedid ); }
    }
  }
  if ( tmp ) { delete tmp; }
  return outstream.str();
}

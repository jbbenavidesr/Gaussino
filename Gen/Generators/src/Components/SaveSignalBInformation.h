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
#ifndef COMPONENT_SAVESIGNALBINFORMATION_H
#define COMPONENT_SAVESIGNALBINFORMATION_H 1

// Include files
// from Gaudi
#include "Defaults/Locations.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/Transformer.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include "HepMCUser/typedefs.h"

/** @class SaveSignalBInformation SaveSignalBInformation.h
 *
 *  Algorithm that takes the b string information from HepMC
 *  and stores it at a specific location
 *
 *  @author Patrick Robbe
 *  @date   2013-06-26
 */
class SaveSignalBInformation
    : public Gaudi::Functional::Transformer<HepMC3::GenEventPtrs( const HepMC3::GenEventPtrs& )> {

public:
  /// Standard constructor
  SaveSignalBInformation( const std::string& name, ISvcLocator* pSvcLocator )
      : Transformer( name, pSvcLocator, { KeyValue{ "InputHepMCEvent", Gaussino::HepMCEventLocation::Default } },
                     { KeyValue{ "OutputHepMCEvent", Gaussino::HepMCEventLocation::BInfo } } ){};

  virtual ~SaveSignalBInformation(){}; ///< Destructor

  HepMC3::GenEventPtrs operator()( const HepMC3::GenEventPtrs& ) const override;

protected:
private:
  /// Extract B string from signal
  HepMC3::GenEventPtr extractSignal( const HepMC3::ConstGenVertexPtr& theVertex ) const;

  /// make a new HepMC event
  StatusCode fillHepMCEvent( HepMC3::GenParticlePtr&            theNewParticle,
                             const HepMC3::ConstGenParticlePtr& theOldParticle ) const;
};
#endif // COMPONENT_SAVESIGNALBINFORMATION_H

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

/** @class SaveSignalBInformation SaveSignalBInformation.h
 *
 *  Algorithm that takes the b string information from HepMC
 *  and stores it at a specific location
 *
 *  @author Patrick Robbe
 *  @date   2013-06-26
 */
class SaveSignalBInformation
    : public Gaudi::Functional::Transformer<std::vector<HepMC3::GenEvent>( const std::vector<HepMC3::GenEvent>& )>
{

public:
  /// Standard constructor
  SaveSignalBInformation( const std::string& name, ISvcLocator* pSvcLocator )
      : Transformer( name, pSvcLocator, {KeyValue{"InputHepMCEvent", Gaussino::HepMCEventLocation::Default}},
                     {KeyValue{"OutputHepMCEvent", Gaussino::HepMCEventLocation::BInfo}} ){};

  virtual ~SaveSignalBInformation(){}; ///< Destructor

  std::vector<HepMC3::GenEvent> operator()( const std::vector<HepMC3::GenEvent>& ) const override;

protected:
private:
  /// Extract B string from signal
  HepMC3::GenEvent* extractSignal( const HepMC3::ConstGenVertexPtr& theVertex ) const;

  /// make a new HepMC event
  StatusCode fillHepMCEvent( HepMC3::GenParticlePtr & theNewParticle, const HepMC3::ConstGenParticlePtr & theOldParticle ) const;
};
#endif // COMPONENT_SAVESIGNALBINFORMATION_H

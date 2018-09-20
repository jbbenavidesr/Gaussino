#ifndef COMPONENT_SAVESIGNALBINFORMATION_H
#define COMPONENT_SAVESIGNALBINFORMATION_H 1

// Include files
// from Gaudi
#include "Defaults/Locations.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/Transformer.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

/** @class SaveSignalBInformation SaveSignalBInformation.h
 *
 *  Algorithm that takes the b string information from HepMC
 *  and stores it at a specific location
 *
 *  @author Patrick Robbe
 *  @date   2013-06-26
 */
class SaveSignalBInformation
    : public Gaudi::Functional::Transformer<std::vector<HepMC::GenEvent>( const std::vector<HepMC::GenEvent>& )>
{

public:
  /// Standard constructor
  SaveSignalBInformation( const std::string& name, ISvcLocator* pSvcLocator )
      : Transformer( name, pSvcLocator, {KeyValue{"InputHepMCEvent", Gaussino::HepMCEventLocation::Default}},
                     {KeyValue{"OutputHepMCEvent", Gaussino::HepMCEventLocation::BInfo}} ){};

  virtual ~SaveSignalBInformation(){}; ///< Destructor

  std::vector<HepMC::GenEvent> operator()( const std::vector<HepMC::GenEvent>& ) const override;

protected:
private:
  /// Extract B string from signal
  HepMC::GenEvent* extractSignal( const HepMC::GenVertexPtr& theVertex ) const;

  /// make a new HepMC event
  StatusCode fillHepMCEvent( HepMC::GenParticlePtr & theNewParticle, const HepMC::GenParticlePtr & theOldParticle ) const;
};
#endif // COMPONENT_SAVESIGNALBINFORMATION_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GenInterfaces/ISampleGenerationTool.h"
#include "GiGaMTReDecay/IRedecaySvc.h"
#include "GiGaMTReDecay/Token.h"


#include <boost/array.hpp>

/** @class Signal Signal.h "Generators/Signal.h"
 *  
 *  Base class for signal samples generation tools. It contains utility 
 *  function that can be used when generating signal samples.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class ReDecaySampleGenerationTool : public extends< GaudiTool, ISampleGenerationTool> {
 public:
   using extends::extends;
  /** Generate a set of pile-up interactions to form an event
   *  @param[in]  nPileUp        Number of pile-up event to generate for 
   *                             this event.
   *  @param[out] theEvents      Container of the generated pile-up 
   *                             interactions.
   *  @param[out] theCollisions  Container of the hard process information
   *                             for each pile-up interaction.
   */
  StatusCode initialize() override;
  virtual bool generate( const unsigned int nPileUp ,
                         HepMC3::GenEventPtrs & theEvents ,
                         LHCb::GenCollisions & theCollisions,
                         HepRandomEnginePtr & engine ) const override;

  /// Print counters and efficiencies at the end of the job.
  virtual void printCounters( ) const override;
 private:
  ToolHandle<ISampleGenerationTool> m_NominalGeneration{this, "NominalSampleGenerationTool", "SignalPlain"};
  ToolHandle<ISampleGenerationTool> m_SignalGeneration{this, "SignalSampleGenerationTool", "SignalPlain"};
  ServiceHandle<IReDecaySvc> m_redecaysvc{this, "ReDecaySvc", "ReDecaySvc"};

};

DECLARE_COMPONENT( ReDecaySampleGenerationTool )
  
StatusCode ReDecaySampleGenerationTool::initialize(){
  auto sc = extends::initialize();
  // Explicitly initialize the tools as the first event might not
  // do both and then multiple threads might make a big mess.
  sc &= m_NominalGeneration->initialize();
  sc &= m_SignalGeneration->initialize();
  return sc;
}


void ReDecaySampleGenerationTool::printCounters()const {
  m_NominalGeneration->printCounters();
}

bool ReDecaySampleGenerationTool::generate( const unsigned int nPileUp ,
                         HepMC3::GenEventPtrs & theEvents ,
                         LHCb::GenCollisions & theCollisions,
                         HepRandomEnginePtr & engine ) const {

  bool ret = true;
  if(m_redecaysvc->isCurrentOriginal()){
    ret = m_NominalGeneration->generate(nPileUp, theEvents, theCollisions, engine);
  } else {
    // Call signal plain sample generation tool which is interfaced to ReDecayProduction
    ret = m_NominalGeneration->generate(nPileUp, theEvents, theCollisions, engine);
  }
  return true;
};

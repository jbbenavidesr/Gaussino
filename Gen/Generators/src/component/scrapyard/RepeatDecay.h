// $Id: RepeatDecay.h,v 1.1 2007-04-01 21:28:12 robbep Exp $
#ifndef GENERATORS_REPEATDECAY_H 
#define GENERATORS_REPEATDECAY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "GenInterfaces/ISampleGenerationTool.h" 

// forward declarations

namespace HepMC {
  class GenParticle ;
}

/** @class RepeatDecay RepeatDecay.h "RepeatDecay.h" 
 *  
 *  Tool for the generation of samples re-using the same event
 *  several times and re-decaying it only. When an event has been found
 *  passing the generator level cut, it is kept and re-decayed several times
 *  (applying again the same cut on it).
 *  Concrete implementation of a ISampleGenerationTool .
 *
 *  @author Patrick Robbe
 *  @date   2007-03-31
 */
class RepeatDecay : public GaudiTool , 
                    virtual public ISampleGenerationTool  {
 private:
  /// Name of the tool used to generate the events (set by option)
  Gaudi::Property<std::string> m_baseToolName{this, "BaseTool", "Inclusive"};
  /// Number of times to redecay the events (set by option)
  Gaudi::Property<unsigned int> m_nRedecayLimit{this,"NRedecay", 50};
 public:
  /// Standard constructor
  RepeatDecay( const std::string& type, const std::string& name,
             const IInterface* parent);
  
  virtual ~RepeatDecay( ); ///< Destructor

  /** Initialize method 
   */
  virtual StatusCode initialize( ) override;

  /** Generate a set of interactions.
   *  Implements ISampleGenerationTool::generate.
   *  -# Generate a set of interactions with the ISampleGenerationTool,
   *       if there is no event in memory or the same event has been re-used
   *       too many times.
   *  -# If there is an event in memory, just erase the particles in it.
   */
  virtual bool generate( const unsigned int nPileUp ,
                         std::vector<HepMC::GenEvent> & theEvents ,
                         LHCb::GenCollisions & theCollisions ) override;

  /** Print generation counters.
   *  Implements ISampleGenerationTool::printCounters.
   */
  virtual void printCounters( ) const override;

 protected:

 private:
  
  /// Tool used to generate the base events which are re-decayed
  ISampleGenerationTool * m_baseTool ;

  /// counter of repetitions
  unsigned int m_nRedecay ;


  /// Memorized events
  std::vector<HepMC::GenEvent> m_theMemorizedEvents ;
  
  /// Memorized collisions
  LHCb::GenCollisions m_theMemorizedCollisions ;
  
  /// Copy a set of events into another
  void copyEvents( std::vector<HepMC::GenEvent> & from , 
                   std::vector<HepMC::GenEvent> & to )  ;
  
  /* Copy a set of collisions into another, with the list of corresponding 
   * events
   */
  void copyCollisions( LHCb::GenCollisions & from , LHCb::GenCollisions & to ,
                       std::vector<HepMC::GenEvent> & theEvents ) ;
};
#endif // GENERATORS_REPEATDECAY_H

// $Id: StandAloneDecayTool.h,v 1.3 2007-09-09 19:33:10 robbep Exp $
#ifndef GENERATORS_STANDALONEDECAYTOOL_H 
#define GENERATORS_STANDALONEDECAYTOOL_H 1

// Include files
// from Gaudi
#include "Generators/Signal.h"


/** @class StandAloneDecayTool StandAloneDecayTool.h component/StandAloneDecayTool.h
 *  Class to generate decay with only the decay tool 
 *
 *  @author Patrick Robbe
 *  @date   2006-04-18
 */
class StandAloneDecayTool : public Signal {
public:
  /// Standard constructor
  StandAloneDecayTool( const std::string & type , const std::string & name ,
                       const IInterface * parent ) ;
  
  virtual ~StandAloneDecayTool( ) ; ///< Destructor
  
  virtual StatusCode initialize( ) override;    ///< Tool initialization
  
  virtual bool generate( const unsigned int nPileUp ,
                         std::vector<HepMC3::GenEvent> & theEvents ,
                         LHCb::GenCollisions & theCollisions ,
                         HepRandomEnginePtr & engine ) const override;

private:
  double m_signalMass ; ///< Mass of the particle to decay

  bool m_inclusive ;  ///< Generate inclusive decay
};
#endif // GENERATORS_STANDALONEDECAYTOOL_H

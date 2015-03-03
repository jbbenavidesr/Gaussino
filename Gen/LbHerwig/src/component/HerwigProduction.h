// $Id: HerwigProduction.h,v 1.5 2007-03-13 19:09:20 gcorti Exp $
#ifndef LBHERWIG_HERWIGPRODUCTION_H 
#define LBHERWIG_HERWIGPRODUCTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"
#include "GaudiKernel/IParticlePropertySvc.h"

class IBeamTool ;

/** @class HerwigProduction HerwigProduction.h 
 *  
 *  Interface tool to produce events with Herwig
 * 
 *  @author Karl Harrison
 *  @date   2005-10-24
 *
 *  Adapted from PythiaProduction.h
 *
 */
class HerwigProduction : public GaudiTool, virtual public IProductionTool {
 public:
  typedef std::vector< std::string > CommandVector ;
  
  /// Standard constructor
  HerwigProduction( const std::string & type , const std::string & name ,
                    const IInterface * parent ) ;
  
  virtual ~HerwigProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode finalize( ) ;   ///< Finalize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;
  
  virtual StatusCode initializeGenerator( ) ;
  
  virtual void setStable( const ParticleProperty * thePP ) ;

  virtual void updateParticleProperties( const ParticleProperty * thePP ) ;

  virtual void turnOnFragmentation( ) ;
  
  virtual void turnOffFragmentation( ) ;

  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                LHCb::GenCollision * theCollision ) ;
  
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) ;

  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) ;

  virtual void printRunningConditions( ) ;

  virtual bool isSpecialParticle( const ParticleProperty * thePP ) const ;

  virtual StatusCode setupForcedFragmentation( const int thePdgId ) ;

 protected:
  /// Parse Herwig commands from a string vector
  StatusCode parseHerwigCommands( const CommandVector & theVector ) ;

  /// Print Herwig parameters
  void printHerwigParameter( ) ;
  
  /// Retrieve hard process information
  void hardProcessInfo( LHCb::GenCollision * theCollision ) ;

  /// Set Herwig parameters
  StatusCode setHerwigParameters( ) ;

  /// Get mass (GeV) for particle with PDG identifier thePdgId
  double getMass( const int thePdgId, double value ) ;

  /// Get event type defined in Generation algorithm
  void setGenerationEventType( ) ;

 private:
  CommandVector m_defaultSettings ; ///< Herwig default settings
  CommandVector m_commandVector ;   ///< Herwig user settings
  
//  Herwig isn't very flexible about print-outs...
//  int m_eventListingLevel ;
//  int m_initializationListingLevel ;
//  int m_finalizationListingLevel ;
  
  std::string m_beamToolName ; ///< Name of beam tool

//  Herwig doesn't allow redirection of output
//  std::string m_herwigListingFileName ; 
//  int m_herwigListingUnit ;

//  Variable beam energy not enabled for Herwig
//  bool m_variableEnergy ;
  
  int m_nevhep ; ///< Internal event counter
  bool m_jimmy ; ///< Flag to request use of Jimmy
  bool m_mcatnlo ; ///< Flag to request use of MC@NLO
  int m_eventType ; ///< Event type
  int m_herwigProcess ; ///< Herwig process code
  std::string m_hepMCName ; ///< Generator name
  
  IBeamTool * m_beamTool ; ///< Pointer to beam tool
  IParticlePropertySvc * m_ppSvc ; ///< Pointer to particl-properties service
};
#endif // LBHERWIG_HERWIGPRODUCTION_H

#ifndef GAUSSRICH_RICHG4TRACKACTIONRICH2PHOTON_H 
#define GAUSSRICH_RICHG4TRACKACTIONRICH2PHOTON_H 1

// Include files
// STL
#include <string>
#include <vector>

// GiGa
#include "GiGa/GiGaTrackActionBase.h"

// forward declarations
class G4Track;
class G4particleDefinition;


/** @class RichG4TrackActionRich2Photon RichG4TrackActionRich2Photon.h GaussRICH/RichG4TrackActionRich2Photon.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2016-05-08
 *  Track  action for photons created in RICH2
 */

class RichG4TrackActionRich2Photon: virtual public GiGaTrackActionBase {
public: 
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames;
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs;
  ///


  /// Standard constructor
  RichG4TrackActionRich2Photon(const std::string& type,
                               const std::string& name,
                               const IInterface*  parent ) ; 

  virtual ~RichG4TrackActionRich2Photon( ); ///< Destructor

  virtual StatusCode initialize () ;   ///< initialize

  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  //  virtual void PostUserTrackingAction ( const G4Track* ) ;


protected:

private:

  RichG4TrackActionRich2Photon() ; ///< no default constructor
  RichG4TrackActionRich2Photon ( const RichG4TrackActionRich2Photon& ) ; ///< no copy
  RichG4TrackActionRich2Photon& operator=( const RichG4TrackActionRich2Photon& ) ;  ///< no copy

 G4double  m_ZCoordInDnsOfRich1;

};
#endif // GAUSSRICH_RICHG4TRACKACTIONRICH2PHOTON_H

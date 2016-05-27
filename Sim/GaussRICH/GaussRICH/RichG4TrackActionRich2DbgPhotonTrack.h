#ifndef GAUSSRICH_RICHG4TRACKACTIONRICH2DBGPHOTONTRACK_H 
#define GAUSSRICH_RICHG4TRACKACTIONRICH2DBGPHOTONTRACK_H 1

// Include files
// STL
#include <string>
#include <vector>

// GiGa
#include "GiGa/GiGaTrackActionBase.h"

// forward declarations
class G4Track;
class G4particleDefinition;


/** @class RichG4TrackActionRich2DbgPhotonTrack RichG4TrackActionRich2DbgPhotonTrack.h 
 *   GaussRICH/RichG4TrackActionRich2DbgPhotonTrack.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2016-05-27
 */
class RichG4TrackActionRich2DbgPhotonTrack: virtual public GiGaTrackActionBase {
public: 
  /// Standard constructor
  RichG4TrackActionRich2DbgPhotonTrack(const std::string& type,
                                       const std::string& name,
                                       const IInterface*  parent ); 

  virtual ~RichG4TrackActionRich2DbgPhotonTrack( ); ///< Destructor
  virtual StatusCode initialize () ;   ///< initialize
  virtual void PreUserTrackingAction  ( const G4Track* ) ;

protected:

private:

  RichG4TrackActionRich2DbgPhotonTrack() ; ///< no default constructor
  RichG4TrackActionRich2DbgPhotonTrack ( const RichG4TrackActionRich2DbgPhotonTrack& ) ; ///< no copy
  RichG4TrackActionRich2DbgPhotonTrack& operator=( const RichG4TrackActionRich2DbgPhotonTrack& ) ;  ///< no copy


  G4double  m_ZCoordInDownstreamOfRich1;

  G4int     m_DeActiavteRich2PhotonTrackForDebug;  // when this is 1, the rich2 photon tracking is deactivated . 
                                                  // This is only for special studies.
                                                  // By default this is 0 and this means rich2 photon 
                                                  // tracking is activated
};
#endif // GAUSSRICH_RICHG4TRACKACTIONRICH2DBGPHOTONTRACK_H

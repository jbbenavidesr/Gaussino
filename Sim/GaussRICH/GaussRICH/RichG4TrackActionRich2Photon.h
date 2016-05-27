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


  /// Standard constructor
  RichG4TrackActionRich2Photon(const std::string& type,
                               const std::string& name,
                               const IInterface*  parent ) ; 

  virtual ~RichG4TrackActionRich2Photon( ); ///< Destructor

  virtual StatusCode initialize () ;   ///< initialize

  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  
  

protected:

private:

  RichG4TrackActionRich2Photon() ; ///< no default constructor
  RichG4TrackActionRich2Photon ( const RichG4TrackActionRich2Photon& ) ; ///< no copy
  RichG4TrackActionRich2Photon& operator=( const RichG4TrackActionRich2Photon& ) ;  ///< no copy

  void FillRich2ScintPhotonDirHisto(G4double aPhotPx, G4double aPhotPy, G4double aPhotPz, 
                                    G4double aPhotX,  G4double aPhotY,  
                                    G4double aPhotZ, G4double PhotWLen, G4int PrepostFlag );

  std::vector<G4double>  getPhotonScintProdAngles(G4double aPhotPx, G4double aPhotPy,
                                                  G4double aPhotPz);
  

  G4double  m_ZCoordInDnsOfRich1;
  G4double  m_MaxPolarYScintPhotRich2;
  G4double  m_Rich2ScintPolarAngleWrtNegZLimA;
  G4double  m_Rich2ScintPolarAngleWrtNegZLimB;
  G4double  m_Rich2ScintPolarAngleWrtNegZLimC;
  G4double  m_Rich2ScintPolarAngleWrtNegZLimD;
  
  G4double  m_Rich2ScintPolarAngleWrtPosZLimA;
  G4double  m_Rich2ScintPolarAngleWrtPosZLimB;

  G4double  m_Rich2ScintPolarAngleWrtXLimA;
  G4double  m_Rich2ScintPolarAngleWrtXLimB;

  G4double  m_Rich2ScintPolarAngleXZSlope;
  G4double  m_Rich2ScintPolarAngleXNegZSlope;
  G4double  m_Rich2ScintPolarAngleXZIntercept;
  G4double  m_Rich2ScintPolarAngleXNegZZIntercept;
  

  
  G4double  m_Rich2ScintYCoordProdLimA;
  G4double  m_Rich2ScintYCoordProdLimB;
  
  G4double  m_Rich2ScintZCoordLimA;
  
  

  G4String  m_RichHpdQWLogVolName;
  G4String  m_RichHpdPhCathodeLogVolName;
  G4int     m_RichG4TrackActionRich2PhotonDebugMode; // when 0 this is false and debug done for production
                                                     // when 1 this is true and debug activated for 
                                                     // special studies.
  
  G4int     m_ActivateRich2ScintPhotReduction;   // when this is 1 , it is true and reduction is activated
                                                 // when this is 0, it is false and reduction not activated
                                                 // the default is 1


};
#endif // GAUSSRICH_RICHG4TRACKACTIONRICH2PHOTON_H

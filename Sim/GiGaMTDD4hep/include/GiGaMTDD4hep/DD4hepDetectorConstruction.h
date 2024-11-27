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
#pragma once
// Geant
#include "DD4hep/Printout.h"
#include "G4VUserDetectorConstruction.hh"
#include <functional>

namespace dd4hep {
  class Detector;
}

/** @class GDMLDetectorConstruction
 *
 *  Detector construction wrapper that converts a DD4hep detector into a Geant4
 * detector
 *
 *  @author Dominik Muller
 */

class DD4hepDetectorConstruction : public G4VUserDetectorConstruction {
  typedef std::function<void( const dd4hep::Detector& )> sdConstructor;

public:
  /**  Constructor.
   *   @param[in] Reference to the DD4hep detector.
   */
  explicit DD4hepDetectorConstruction( const dd4hep::Detector& detector );
  /**  Default constructor.
   *   Reference to the DD4hep detector taken from the global instance.
   */
  DD4hepDetectorConstruction();
  virtual ~DD4hepDetectorConstruction() = default;
  /**  Create volumes using the DD4hep converter.
   *   @return World wolume.
   */
  virtual G4VPhysicalVolume* Construct() override;
  /**  Create sensitive detectors. Simple sensitive detector is created.
   *   See Geant examples/extended/persistency/gdml/G04 for further details.
   *   As this depends on the individual use-case for the mapping of gdml
   *   auxiliar 'SensDet' fields to the SensDet implementation, no
   *   general solution can be used and hence a function needs to be set.
   */
  virtual void ConstructSDandField() override { m_sd_constructor( *m_detector ); };
  void         SetSDConstructor( sdConstructor constr ) { m_sd_constructor = constr; }
  void         setPrintLevel( dd4hep::PrintLevel level ) { m_printlevel = level; }

  /// Flag to debug materials during conversion mechanism
  void SetDebugMaterials( bool val = true ) { m_debugMaterials = val; }
  /// Flag to debug elements during conversion mechanism
  void SetDebugElements( bool val = true ) { m_debugElements = val; }
  /// Flag to debug shapes during conversion mechanism
  void SetDebugShapes( bool val = true ) { m_debugShapes = val; }
  /// Flag to debug volumes during conversion mechanism
  void SetDebugVolumes( bool val = true ) { m_debugVolumes = val; }
  /// Flag to debug placements during conversion mechanism
  void SetDebugPlacements( bool val = true ) { m_debugPlacements = val; }
  /// Flag to debug regions during conversion mechanism
  void SetDebugRegions( bool val = true ) { m_debugRegions = val; }

private:
  /// Pointer to the detector handed over by the constructor for convenience.
  const dd4hep::Detector* m_detector;
  sdConstructor           m_sd_constructor = []( const dd4hep::Detector& ) {};
  dd4hep::PrintLevel      m_printlevel     = dd4hep::NOLOG;

  /// Property: Flag to debug materials during conversion mechanism
  bool m_debugMaterials = false;
  /// Property: Flag to debug elements during conversion mechanism
  bool m_debugElements = false;
  /// Property: Flag to debug shapes during conversion mechanism
  bool m_debugShapes = false;
  /// Property: Flag to debug volumes during conversion mechanism
  bool m_debugVolumes = false;
  /// Property: Flag to debug placements during conversion mechanism
  bool m_debugPlacements = false;
  /// Property: Flag to debug regions during conversion mechanism
  bool m_debugRegions = false;
};

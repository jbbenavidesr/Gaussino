#pragma once
// Geant
#include "Geant4/G4GDMLParser.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include <functional>

/** @class GDMLDetectorConstruction
 *
 *  Detector construction from the GDML file. It includes a hook to provide a callable function which is executed
 *  to construct the sensitive detectors and fields.
 *
 *  @author Dominik Muller
 */

class GDMLDetectorConstruction : public G4VUserDetectorConstruction
{
  typedef std::function<void(G4GDMLParser*)> sdConstructor;

public:
  /**  Constructor.
   *   @param[in] aFileName Name of the GDML file with the detector description.
   */
  explicit GDMLDetectorConstruction( const std::string& aFileName ) : m_fileName( aFileName )
  {
    m_parser.Read( m_fileName );
  }
  virtual ~GDMLDetectorConstruction(){};
  /**  Create volumes using the GDML parser.
   *   @return World wolume.
   */
  virtual G4VPhysicalVolume* Construct() override
  {
    static auto worldvol = m_parser.GetWorldVolume();
    return worldvol;
  }
  /**  Create sensitive detectors. Simple sensitive detector is created.
   *   See Geant examples/extended/persistency/gdml/G04 for further details.
   *   As this depends on the individual use-case for the mapping of gdml
   *   auxiliar 'SensDet' fields to the SensDet implementation, no
   *   general solution can be used and hence a function needs to be set.
   */
  virtual void ConstructSDandField() override { m_sd_constructor(&m_parser); };
  void SetSDConstructor( sdConstructor constr ) { m_sd_constructor = constr; }

private:
  /// Parser of the GDML file
  G4GDMLParser m_parser;
  /// name of the GDML file
  std::string m_fileName;
  sdConstructor m_sd_constructor = [](G4GDMLParser*) {};
};

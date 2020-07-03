// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "LbDD4hep/IDD4hepSvc.h"
#include "GiGaMTDD4hep/DD4hepDetectorConstruction.h"

/** @class DD4hepDetectorConstructionFAC SimG4Components/src/DD4hepDetectorConstructionFAC.h DD4hepDetectorConstructionFAC.h
 *
 *  Simple tool to create a detector construction which instantiates the geometry from a GDML file.
 *  No support for sensitive detectors or fields
 *
 *  @author Dominik Muller
 */

class DD4hepDetectorConstructionFAC : public extends<GaudiTool, GiGaFactoryBase<G4VUserDetectorConstruction>>
{
private:
  Gaudi::Property<bool> m_debugMaterials  {this, "DebugMaterials", false};
  Gaudi::Property<bool> m_debugElements   {this, "DebugElements", false};
  Gaudi::Property<bool> m_debugShapes     {this, "DebugShapes", false};
  Gaudi::Property<bool> m_debugVolumes    {this, "DebugVolumes", false};
  Gaudi::Property<bool> m_debugPlacements {this, "DebugPlacements", false};
  Gaudi::Property<bool> m_debugRegions    {this, "DebugRegions", false};
  ServiceHandle<LHCb::Det::LbDD4hep::IDD4hepSvc> m_dd4Svc{this, "DD4hepSvc", "DD4hepSvc"};

public:
  using extends::extends;
  StatusCode initialize() override;
  G4VUserDetectorConstruction* construct() const override;
};

#include "GiGaMTDD4hep/Utilities.h"

DECLARE_COMPONENT(DD4hepDetectorConstructionFAC)

G4VUserDetectorConstruction* DD4hepDetectorConstructionFAC::construct() const {
  auto constr = new DD4hepDetectorConstruction(m_dd4Svc->getDetector());
  // Propagate the output level of the factory to the constructed
  // object which will enable outputting a lot of debug during the DD4hep->G4
  // transformation
  constr->setPrintLevel(DD4hepGaudiMessaging::Convert(msgLevel()));
  if(m_debugMaterials) constr->SetDebugMaterials();
  if(m_debugElements) constr->SetDebugElements();
  if(m_debugShapes) constr->SetDebugShapes();
  if(m_debugVolumes) constr->SetDebugVolumes();
  if(m_debugPlacements) constr->SetDebugPlacements();
  if(m_debugRegions) constr->SetDebugRegions();
  return constr;
}

StatusCode DD4hepDetectorConstructionFAC::initialize() {
  auto sc = extends::initialize();
  if (!sc.isSuccess()) {
    return sc;
  }
  if (!m_dd4Svc.get()) {
    return Error("Could not find DD4hepGeoSvc");
  }
  return StatusCode::SUCCESS;
}

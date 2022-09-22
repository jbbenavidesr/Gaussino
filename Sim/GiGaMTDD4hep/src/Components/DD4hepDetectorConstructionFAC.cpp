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
// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "LbDD4hep/IDD4hepSvc.h"
#include "GiGaMTDD4hep/DD4hepDetectorConstruction.h"
#include "TSystem.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"
#include "G4FieldManager.hh"

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
  Gaudi::Property<std::string> m_detDescLocation{this, "DescriptionLocation",
                                                 "${DETECTOR_PROJECT_ROOT}/compact/LHCb-no-GDML.xml",
                                                 "Location of the XML detector description"};

public:
  using extends::extends;
  StatusCode initialize() override;
  G4VUserDetectorConstruction* construct() const override;
private:

  // Storage maps to manage assignment of sensdet factories to volumes
  std::map<GiGaFactoryBase<G4VSensitiveDetector>*, std::set<G4LogicalVolume*>>
      mmap_sensdet_to_lvols;
  std::map<std::string, ToolHandle<GiGaFactoryBase<G4VSensitiveDetector>>>
      mmap_name_to_sensdetfac;

  std::atomic_bool m_found_global_fieldmgr{false};
  std::map<GiGaFactoryBase<G4FieldManager, bool>*, std::set<G4LogicalVolume*>>
      mmap_fieldmgr_to_lvols;
  std::map<std::string, ToolHandle<GiGaFactoryBase<G4FieldManager, bool>>>
      mmap_name_to_fieldmgrfac;
};

#include "GiGaMTDD4hep/Utilities.h"

DECLARE_COMPONENT(DD4hepDetectorConstructionFAC)

G4VUserDetectorConstruction* DD4hepDetectorConstructionFAC::construct() const {
  auto constr = new DD4hepDetectorConstruction(dd4hep::Detector::getInstance());
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
  auto & detector = dd4hep::Detector::getInstance();

  TString descriptionFile( m_detDescLocation.value() );
  gSystem->ExpandPathName( descriptionFile );
  const char* fname = descriptionFile.Data();
  if ( this->msgLevel( MSG::DEBUG ) ) debug() << "Loading DD4hep Geometry: " << fname << endmsg;
  detector.apply( "DD4hep_CompactLoader", 1, (char**)&fname );
  return StatusCode::SUCCESS;
}

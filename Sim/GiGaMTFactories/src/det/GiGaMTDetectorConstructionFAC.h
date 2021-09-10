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
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

#include "GaudiAlg/FunctionalDetails.h"
#include "GaudiAlg/FunctionalUtilities.h"
#include "Utils/ToolProperty.h"

class IGiGaMTGeoSvc;
class IGaussinoTool;

// Factory class implemented as a GaudiTool that creates and configures the
// GiGaMTRunMangager singleton.

class GiGaMTDetectorConstructionFAC : public extends<GiGaTool, GiGaFactoryBase<G4VUserDetectorConstruction>> {
public:
  using extends::extends;

  G4VUserDetectorConstruction* construct() const override;
  StatusCode                   initialize() override;

protected:
  typedef std::pair<std::string, std::vector<std::string>>  SensDetNameVolumesPair;
  typedef std::map<std::string, std::vector<std::string>>   SensDetNameVolumesMap;
  typedef ToolHandle<GiGaFactoryBase<G4VSensitiveDetector>> SensDetFac;
  typedef std::map<std::string, SensDetFac>                 SensDetVolumeMap;

  void                                      DressVolumes() const;
  void                                      SaveGDML( G4LogicalVolume* ) const;
  ServiceHandle<IGiGaMTGeoSvc>              m_geoSvc{this, "GiGaMTGeoSvc", "GiGaMTGeo"};
  ToolHandleArray<IGaussinoTool>            m_afterGeo{this};
  Gaudi::Property<std::vector<std::string>> m_afterGeoNames{this,
                                                            "AfterGeoConstructionTools",
                                                            {},
                                                            tool_array_setter( m_afterGeo, m_afterGeoNames ),
                                                            Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};

  Gaudi::Property<std::string> m_schema{this, "Schema", "$GDML_base/src/GDMLSchema/gdml.xsd"};
  Gaudi::Property<std::string> m_outfile{this, "Output", ""};

private:
  SensDetVolumeMap                       m_sens_dets;
  Gaudi::Property<SensDetNameVolumesMap> m_namemap{this,
                                                   "SensDetVolumeMap",
                                                   {},
                                                   [this]( Gaudi::Details::PropertyBase& ) {
                                                     for ( auto& keypairs : this->m_namemap ) {
                                                       auto& name = keypairs.first;
                                                       m_sens_dets.emplace( std::piecewise_construct,
                                                                            std::forward_as_tuple( name ),
                                                                            std::forward_as_tuple( name, this ) );
                                                     }
                                                   },
                                                   Gaudi::Details::Property::ImmediatelyInvokeHandler{true}};
};

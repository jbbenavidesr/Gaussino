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

// Include files
#include "GaudiKernel/IInterface.h"

#include "GiGaMTRegions/SimAttribute.h"
#include "GiGaMTRegions/RegionCuts.h"
#include <map>

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IRegionsDefinitionSvc(948, 1 , 0);


/** @class IRegionsDefinitionSvc IRegionsDefinitionSvc.h SimDesc/IRegionsDefinitionSvc.h
 *
 *  this interface defines a RegionsDefinition service that is able to answer
 *  many question concerning the simulation of the detector. It
 *  is able to associate simulation attributes to logical volumes.
 *  These describe the way these volumes should be simulated
 *
 *  @author Sebastien Ponce
 */

class IRegionsDefinitionSvc : virtual public IInterface {
  
public:

  // typedefs used  
  typedef std::map<int, const SimAttribute*> PartAttr;
  typedef std::vector<RegionCuts> VectOfRegCuts;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRegionsDefinitionSvc; }

  /**
   * This method returns the simulation attribute associated to a given
   * logical volume.
   * @param vol the logical volume
   * @return the simulation attribute that should be used to simulate
   * this logical volume
   */
  virtual const PartAttr* simAttribute (const std::string volname) const = 0;

  /**
   * This method tells whether a simulation attribute is associated to a given
   * logical volume or not
   * @param vol the logical volume
   */
  virtual bool hasSimAttribute (const std::string volname) const = 0;

  /**
   * This method returns the pointer to the vector of region definitions.
   * @param 
   * @return std::vector<RegionCuts>
   */
  virtual const std::vector<RegionCuts>* regionsDefs () const = 0;

  /**
   * this method erases the current set of attributes and loads a new set
   */
  virtual void reload () = 0;

};

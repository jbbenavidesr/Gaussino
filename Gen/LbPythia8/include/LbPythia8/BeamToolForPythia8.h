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
// $Id: BeamToolForPythia8.h,v 1.1.1.1 2012-11-26 17:02:19 amartens Exp $
#ifndef LBPYTHIA8_BEAMTOOLFORPYTHIA8_H
#define LBPYTHIA8_BEAMTOOLFORPYTHIA8_H 1

// Include files
// from Pythia8
#include "Pythia8/BeamShape.h"
#include "Pythia8/Settings.h"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Vector3DTypes.h"

// from Generators
#include "GenInterfaces/IBeamTool.h"

/** @class BeamToolForPythia8 BeamToolForPythia8.h LbPythia8/BeamToolForPythia8.h
 *  Random generator class to interface Gaudi Random Generator to Pythia8
 *
 *  @author Aurelien Martens
 *  @date   2012-11-26
 */

class BeamToolForPythia8 : public Pythia8::BeamShape {

public:
  // Initialize beam parameters.
  void init(Pythia8::Settings& settings, Pythia8::Rndm* rndmPtrIn) override;

  /// Constructor
  BeamToolForPythia8( IBeamTool *i , Pythia8::Settings& settings, StatusCode &sc ) ;

  /// pick parameterisation for the beam spread and direction
  void pick() override;

  /// Destructor
  ~BeamToolForPythia8();

private:
  IBeamTool* m_iBeamTool;
  Gaudi::XYZVector m_meanBeam1;
  Gaudi::XYZVector m_meanBeam2;

} ;
#endif // LBPYTHIA8_BEAMTOOLFORPYTHIA8_H

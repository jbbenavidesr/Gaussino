// $Id: CollidingBeamsWithSvc.cpp,v 1.5 2007-02-08 17:46:06 gcorti Exp $
// Include files

// local
#include "CollidingBeamsWithSvc.h"

// from Gaudi
#include "GenBeam/IBeamInfoSvc.h"

// From Kernel
#include "GaudiKernel/SystemOfUnits.h"

#include "NewRnd/RndGlobal.h"
#include "CLHEP/Random/RandGauss.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CollidingBeamsWithSvc
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_COMPONENT(CollidingBeamsWithSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CollidingBeamsWithSvc::CollidingBeamsWithSvc(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IBeamTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
CollidingBeamsWithSvc::~CollidingBeamsWithSvc() { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode CollidingBeamsWithSvc::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_beaminfosvc = svc<IBeamInfoSvc>("BeamInfoSvc", true);
  if (!m_beaminfosvc) {
    return Error("Error retrieving the BeamInfosvc");
  }

  return sc;
}

//=============================================================================
// Mean value of the beam momentum
//=============================================================================
void CollidingBeamsWithSvc::getMeanBeams(Gaudi::XYZVector& pBeam1,
                                         Gaudi::XYZVector& pBeam2) const {

  double p1x, p1y, p1z, p2x, p2y, p2z;

  p1x = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->horizontalCrossingAngle() + m_beaminfosvc->horizontalBeamlineAngle());
  p1y = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->verticalCrossingAngle() + m_beaminfosvc->verticalBeamlineAngle());
  p1z = m_beaminfosvc->energy();
  pBeam1.SetXYZ(p1x, p1y, p1z);

  p2x = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->horizontalCrossingAngle() - m_beaminfosvc->horizontalBeamlineAngle());
  p2y = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->verticalCrossingAngle() - m_beaminfosvc->verticalBeamlineAngle());
  p2z = -m_beaminfosvc->energy();
  pBeam2.SetXYZ(p2x, p2y, p2z);
}

//=============================================================================
// Current value of the smeared beams
//=============================================================================
void CollidingBeamsWithSvc::getBeams(Gaudi::XYZVector& pBeam1,
                                     Gaudi::XYZVector& pBeam2) {
  double p1x, p1y, p1z, p2x, p2y, p2z;
  CLHEP::RandGauss gaussianDist{ThreadLocalEngine::Get(), 0, 1};
  p1x = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->horizontalCrossingAngle() + m_beaminfosvc->horizontalBeamlineAngle() +
            gaussianDist() * m_beaminfosvc->angleSmear());
  p1y = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->verticalCrossingAngle() + m_beaminfosvc->verticalBeamlineAngle() +
            gaussianDist() * m_beaminfosvc->angleSmear());
  p1z = m_beaminfosvc->energy();
  pBeam1.SetXYZ(p1x, p1y, p1z);

  p2x = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->horizontalCrossingAngle() - m_beaminfosvc->horizontalBeamlineAngle() +
            gaussianDist() * m_beaminfosvc->angleSmear());
  p2y = m_beaminfosvc->energy() *
        sin(m_beaminfosvc->verticalCrossingAngle() - m_beaminfosvc->verticalBeamlineAngle() +
            gaussianDist() * m_beaminfosvc->angleSmear());
  p2z = -m_beaminfosvc->energy();
  pBeam2.SetXYZ(p2x, p2y, p2z);
}

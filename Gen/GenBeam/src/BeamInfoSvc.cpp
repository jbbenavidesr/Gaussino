#include "BeamInfoSvc.h"

DECLARE_COMPONENT(BeamInfoSvc)

StatusCode BeamInfoSvc::initialize() {
  auto sc = Service::initialize();
  m_beamspot.SetXYZ(m_xLuminousRegion, m_yLuminousRegion, m_zLuminousRegion);
  return sc; 
}

StatusCode BeamInfoSvc::finalize() { return Service::finalize(); }

StatusCode BeamInfoSvc::queryInterface(const InterfaceID& id, void** ppI) {
  if (0 == ppI) {
    return StatusCode::FAILURE;  //  RETURN !!!
  } else if (IBeamInfoSvc::interfaceID() == id) {
    *ppI = static_cast<IBeamInfoSvc*>(this);
  } else {
    return Service::queryInterface(id, ppI);  //  RETURN !!!
  }

  addRef();

  return StatusCode::SUCCESS;
}
double BeamInfoSvc::energy() const { return m_beamEnergy; }

double BeamInfoSvc::sigmaS() const { return m_bunchLengthRMS; }

double BeamInfoSvc::epsilonN() const { return m_normalizedEmittance; }

double BeamInfoSvc::revolutionFrequency() const {
  return m_revolutionFrequency;
}

double BeamInfoSvc::horizontalCrossingAngle() const {
  return m_horizontalCrossingAngle;
}

double BeamInfoSvc::verticalCrossingAngle() const {
  return m_verticalCrossingAngle;
}

double BeamInfoSvc::horizontalBeamlineAngle() const {
  return m_horizontalBeamlineAngle;
}

double BeamInfoSvc::totalXSec() const { return m_totalCrossSection; }

double BeamInfoSvc::verticalBeamlineAngle() const {
  return m_verticalBeamlineAngle;
}

double BeamInfoSvc::betaStar() const { return m_betaStar; }

double BeamInfoSvc::bunchSpacing() const { return m_bunchSpacing; }

const Gaudi::XYZPoint& BeamInfoSvc::beamSpot() const { return m_beamspot; }

double BeamInfoSvc::luminosity() const { return m_luminosity; }

double BeamInfoSvc::nu() const {
  return luminosity() * totalXSec() / revolutionFrequency();
}

double BeamInfoSvc::emittance() const {
  double beta = sqrt(energy() * energy() - 938.272013 * Gaudi::Units::MeV *
                                               938.272013 * Gaudi::Units::MeV) /
                energy();
  double gamma = 1. / sqrt(1. - beta * beta);
  return epsilonN() / gamma / beta;
}

double BeamInfoSvc::sigmaX() const {
  return sqrt(betaStar() * emittance() / 2.);
}

double BeamInfoSvc::sigmaY() const {
  return sqrt(betaStar() * emittance() / 2.);
}

double BeamInfoSvc::sigmaZ() const { return sigmaS() / sqrt(2.); }

double BeamInfoSvc::angleSmear() const {
  if (0.0 != betaStar())
    return (emittance() / betaStar());
  else
    return 0.0;
}

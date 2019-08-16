// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// GaudiKernel
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "Geant4/G4MagneticField.hh"
#include "GiGaMTCore/IGiGaMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

/** @file
 *
 *  Magnetic field that uses a magnetic field service to get
 *  the field value. This file contains the magnetic field itself,
 *  which is constructed with a pointer to the magnetic field service
 *  and a factory that gets the service and constructs the magnetic fields on demand.
 *
 *  @author Dominik Muller
 */

class MagFieldFromSvc : public G4MagneticField, public GiGaMessage
{
public:
  MagFieldFromSvc( IMagneticFieldSvc* svc ) : G4MagneticField(), m_mfSvc( svc ) {}
  virtual ~MagFieldFromSvc() = default;

  void GetFieldValue( const double Point[4], double* B ) const override
  {
    if ( !Point ) {
      throw GaudiException( "GetFieldValue: Point = 0 !", "MagFieldFromSvc", StatusCode::FAILURE );
    }
    if ( !B ) {
      throw GaudiException( "GetFieldValue: B     = 0 !", "MagFieldFromSvc", StatusCode::FAILURE );
    }
    if ( !m_mfSvc ) {
      throw GaudiException( "GetFieldValue: m_mfSvc = 0 !", "MagFieldFromSvc", StatusCode::FAILURE );
    }

    // point
    // const HepPoint3D point( Point[0] , Point[1] , Point[2] );
    const Gaudi::XYZPoint point( Point[0], Point[1], Point[2] );
    Gaudi::XYZVector field( 0., 0., 0. );
    StatusCode sc = m_mfSvc->fieldVector( point, field );
    HepGeom::Vector3D<double> m_field{field.x(), field.y(), field.z()};

    if ( sc.isFailure() ) {
      using CLHEP::cm;
      using CLHEP::tesla;
      std::stringstream msg;
      msg << " Position[cm]=(" << point.x() / cm << "," << point.y() / cm << "," << point.z() / cm << ")"
          << " Field[tesla]=(" << m_field.x() / tesla << "," << m_field.y() / tesla << "," << m_field.z() / tesla
          << ")";
      error( msg.str() );
      throw GaudiException( "GetFieldValue, error status code from IMagneticFieldSvc", "MagFieldFromSvc", sc );
    }
    ///
    *( B + 0 ) = m_field.x();
    *( B + 1 ) = m_field.y();
    *( B + 2 ) = m_field.z();
    ///
  }

private:
private:
  IMagneticFieldSvc* m_mfSvc{nullptr};
};

class MagFieldFromSvcFAC : public extends<GiGaTool, GiGaFactoryBase<G4MagneticField>>
{
public:
  using extends::extends;
  StatusCode initialize() override {
    auto sc = extends::initialize();
    m_magFieldSvc.retrieve();
    return sc;
  }
  MagFieldFromSvc* construct() const override { return new MagFieldFromSvc{m_magFieldSvc.get()}; }

private:
  ServiceHandle<IMagneticFieldSvc> m_magFieldSvc{this, "MagneticFieldService", "MagneticFieldSvc"};
};

DECLARE_COMPONENT_WITH_ID( MagFieldFromSvcFAC, "MagFieldFromSvc" )

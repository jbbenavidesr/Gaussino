/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Gaussino
#include "GiGaMTMagnetFactories/MagneticField.h"

namespace Gaussino::MagneticField {
  class UniformField : public Field {
    Gaudi::XYZVector m_vector;

  public:
    using Field::Field;
    void                    setVector( Gaudi::XYZVector vector ) { m_vector = vector; };
    inline Gaudi::XYZVector fieldVector( const Gaudi::XYZPoint& /* point */ ) const override { return m_vector; };
  };

  class UniformFieldFactory : public FieldFactory<UniformField> {
    Gaudi::Property<double> m_x{ this, "B_x", 0. * Gaudi::Units::tesla };
    Gaudi::Property<double> m_y{ this, "B_y", 0. * Gaudi::Units::tesla };
    Gaudi::Property<double> m_z{ this, "B_z", 0. * Gaudi::Units::tesla };

  public:
    using FieldFactory<UniformField>::FieldFactory;
    virtual UniformField* construct() const override {
      auto field = FieldFactory<UniformField>::construct();
      debug() << "Constructing external uniform magnetic field with"
              << " x: " << m_x.value() / Gaudi::Units::tesla << " T,"
              << " y: " << m_y.value() / Gaudi::Units::tesla << " T,"
              << " z: " << m_z.value() / Gaudi::Units::tesla << " T." << endmsg;
      Gaudi::XYZVector vector{ m_x.value(), m_y.value(), m_z.value() };
      field->setVector( vector );
      field->SetMessageInterface( message_interface() );
      return field;
    }
  };
} // namespace Gaussino::MagneticField

DECLARE_COMPONENT_WITH_ID( Gaussino::MagneticField::UniformFieldFactory, "UniformMagneticField" )

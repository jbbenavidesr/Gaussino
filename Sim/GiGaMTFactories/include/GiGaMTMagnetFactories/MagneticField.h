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

#pragma once

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Geant4
#include "G4MagneticField.hh"

// Gaussino
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

namespace Gaussino::MagneticField {
  class Field : public G4MagneticField, public Gsino::Message {

  public:
    using G4MagneticField::G4MagneticField;

    virtual Gaudi::XYZVector fieldVector( const Gaudi::XYZPoint& point ) const = 0;
    void                     GetFieldValue( const double Point[4], double* B ) const override;
  };

  class FieldFromCallback : public Field {
  public:
    using Callback = std::function<Gaudi::XYZVector( const Gaudi::XYZPoint& point )>;
    using Field::Field;

    inline Gaudi::XYZVector fieldVector( const Gaudi::XYZPoint& point ) const override {
      return m_fieldCallback( point );
    }
    void setCallback( Callback fieldCallback ) { m_fieldCallback = fieldCallback; }

  private:
    Callback m_fieldCallback;
  };

  template <class AnyField>
  class FieldFactory : public extends<GiGaTool, GiGaFactoryBase<G4MagneticField>> {
    static_assert( std::is_base_of<G4MagneticField, AnyField>::value );

  public:
    using extends::extends;

    virtual AnyField* construct() const override {
      auto field = new AnyField{};
      field->SetMessageInterface( message_interface() );
      return field;
    };
  };
} // namespace Gaussino::MagneticField

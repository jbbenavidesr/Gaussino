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
#include "GiGaMTFactories/GiGaTool.h"
#include "G4EmStandardPhysics_option1.hh"
#include "G4VPhysicsConstructor.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"

/** GiGaMTPhysConstr
 *
 *  Template classes to integrate physics constructors found in Geant4
 *  into the configurable Gaudi framework by providing
 *
 *  @author Dominik Muller
 *  @date   2018-07-22
 */

// Base object to encapsulate the call to the actual constructor. Implement to reduce the
// amount of redudant code that needs to be written during template specialization if the
// specific implementation of G4VPhysicsConstructor needs additional options
class GiGaMTPhysConstr : public GiGaTool
{
protected:
  using GiGaTool::GiGaTool;
  virtual ~GiGaMTPhysConstr(){};
  inline int verbosity() const
  {
    auto verb = msgLevel();
    if ( verb == MSG::DEBUG ) {
      return 1;
    } else if ( verb == MSG::VERBOSE ) {
      return 2;
    } else {
      return 0;
    }
  }
};

template <typename T>
using hasGiGaMessage = typename std::enable_if<std::is_base_of<GiGaMessage, T>::value>::type;

template <typename PHYS, typename dummy=void>
class GiGaMTG4PhysicsConstrFAC : public extends<GiGaMTPhysConstr, GiGaFactoryBase<G4VPhysicsConstructor>>
{
  static_assert( std::is_base_of<G4VPhysicsConstructor, PHYS>::value );
  static_assert( std::is_default_constructible<PHYS>::value );

public:
  using extends::extends;
  PHYS* construct() const override
  {
    auto tmp = new PHYS{};
    tmp->SetPhysicsName( name() );
    tmp->SetVerboseLevel( verbosity() );
    return tmp;
  }
};

template <typename PHYS>
class GiGaMTG4PhysicsConstrFAC<PHYS, hasGiGaMessage<PHYS>> : public extends<GiGaMTPhysConstr, GiGaFactoryBase<G4VPhysicsConstructor>>
{
  static_assert( std::is_base_of<G4VPhysicsConstructor, PHYS>::value );
  static_assert( std::is_default_constructible<PHYS>::value );

public:
  using extends::extends;
  PHYS* construct() const override
  {
    auto tmp = new PHYS{};
    tmp->SetPhysicsName( name() );
    tmp->SetVerboseLevel( verbosity() );
    tmp->SetMessageInterface( message_interface() );
    return tmp;
  }
};

#pragma once

// Include files
#include "Geant4/G4VSensitiveDetector.hh"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

/** GiGaMTG4SensDet
 *
 *  Template classes to integrate sensitive detector found in Geant4
 *  into the configurable Gaudi framework by providing
 *
 *  @author Dominik Muller
 *  @date   2018-12-18
 */

// Base object to encapsulate the call to the actual constructor. Implement to reduce the
// amount of redudant code that needs to be written during template specialization if the
// specific implementation of G4VSensitiveDetector needs additional options
class GiGaMTG4SensDet : public GiGaTool
{
protected:
  using GiGaTool::GiGaTool;
  virtual ~GiGaMTG4SensDet() = default;
  inline int verbosity() const
  {
    int verb = msgLevel();
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

template <typename SENSDET, typename dummy = void>
class GiGaMTG4SensDetFactory : public extends<GiGaMTG4SensDet, GiGaFactoryBase<G4VSensitiveDetector>>
{
  static_assert( std::is_base_of<G4VSensitiveDetector, SENSDET>::value );
  Gaudi::Property<std::string> m_name{this, "DetName", name()};

public:
  using extends::extends;
  virtual SENSDET* construct() const override
  {
    auto tmp = new SENSDET{m_name};
    tmp->SetVerboseLevel( verbosity() );
    return tmp;
  }
};

template <typename SENSDET>
class GiGaMTG4SensDetFactory<SENSDET, hasGiGaMessage<SENSDET>>
    : public extends<GiGaMTG4SensDet, GiGaFactoryBase<G4VSensitiveDetector>>
{
  static_assert( std::is_base_of<G4VSensitiveDetector, SENSDET>::value );
  Gaudi::Property<std::string> m_name{this, "DetName", name()};

public:
  using extends::extends;
  virtual SENSDET* construct() const override
  {

    std::string detname( name() );
    std::string::size_type posdot = detname.find( "." );
    while ( posdot != std::string::npos ) {
      detname.erase( 0, posdot + 1 );
      posdot = detname.find( "." );
    }

    debug() << "Sens det shortened name " << name() << " -> " << detname << endmsg;
    auto tmp = new SENSDET{detname};
    tmp->SetVerboseLevel( verbosity() );
    tmp->SetMessageInterface( message_interface() );
    return tmp;
  }
};

#include "GiGaMTFactories/Magnet/FieldMgrBase.h"
class G4ChordFinder;

/** @class ZeroFieldMgr
 *
 *  Simple magnetic field manager that has a G4MagneticField nullptr.
 *  This effectively turns of the field in the assigned volumes.
 *  Useful if there is a global magnetic field that should be
 *  ignored in only a few volumes
 *
 *  @author Dominik Muller
 *  @date   2019-06-04
 */
class ZeroFieldMgr : public Gaussino::FieldMgrBase
{
public:
  using Gaussino::FieldMgrBase::FieldMgrBase;

  G4MagneticField* field() const override {return nullptr;};

  virtual ~ZeroFieldMgr( ) = default;

};

DECLARE_COMPONENT( ZeroFieldMgr )

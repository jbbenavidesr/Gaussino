#include "GiGaMTFactories/GiGaMTRunManagerFAC.h"

DECLARE_COMPONENT(GiGaMTRunManagerFAC)

/*static*/ std::atomic_bool GiGaMTRunManagerFAC::created{false};

GiGaMTRunManager* GiGaMTRunManagerFAC::construct() const {
  if(created){
    error() << "Already called construct(). Just returning the singleton!" << endmsg;
    return GiGaMTRunManager::GetGiGaMTRunManager();
  }
  debug() << "Constructing the main GiGaMTRunManager instance" << endmsg;

  auto mgr = GiGaMTRunManager::GetGiGaMTRunManager();
  mgr->SetMessageInterface(this);
  created = true;
  return mgr;
}

// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "AmpGen/Minimisable.h"
using namespace AmpGen;

Minimisable::Minimisable(MinuitParameterSet* mps)
  : _pset(mps)
{

}

Minimisable::Minimisable(const Minimisable& other) 
   : _pset(other._pset)
{

}
 
void Minimisable::setPset(MinuitParameterSet* mps){
  _pset = mps;
}

MinuitParameterSet* Minimisable::getParSet(){
  if(0 != _pset){
    return _pset;
  }else{
    return MinuitParameterSet::getDefaultSet();
  }
}

//


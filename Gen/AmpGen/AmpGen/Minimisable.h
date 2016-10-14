#ifndef MINIMISABLE_HH
#define MINIMISABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "AmpGen/MinuitParameterSet.h"

namespace AmpGen{
class Minimisable {
  MinuitParameterSet* _pset;
 public:
  Minimisable(MinuitParameterSet* mps=0);
  Minimisable(const Minimisable& other);
  void setPset(MinuitParameterSet* mps);
  MinuitParameterSet* getParSet();


  // you can choose to implement these:
  void beginFit(){}
  void parametersChanged(){}
  void endFit(){}

 // you'll have to implement this:
  virtual double getVal() =0;

  // useful:
  double getNewVal(){
    parametersChanged();
    return getVal();
  }
};
}//namespace AmpGen

#endif
//

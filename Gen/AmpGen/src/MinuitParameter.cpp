// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "AmpGen/MinuitParameter.h"
#include "AmpGen/MinuitParameterSet.h"
#include "AmpGen/MsgService.h"

using namespace AmpGen; 

int    MinuitParameter::iFixInit() const{ return _iFixInit;}
double MinuitParameter::meanInit() const{ return _meanInit;}
double MinuitParameter::stepInit() const{ return _stepInit;}
double MinuitParameter::minInit() const{ return _minInit;}
double MinuitParameter::maxInit() const{ return _maxInit;}

double MinuitParameter::mean() const{ return _meanResult;}
double MinuitParameter::errPos() const { return _errPosResult;}
double MinuitParameter::errNeg() const { return _errNegResult;}
double MinuitParameter::err() const{ return _errResult;}
const std::string& MinuitParameter::name() const{ return _name; }

double MinuitParameter::getCurrentFitVal() const{
  return _meanResult;
}

bool MinuitParameter::hidden() const { return _iFixInit == 1 ; } 

const AmpGen::MinuitParameterSet* MinuitParameter::parSet() const { 
  return _pset;
}


void MinuitParameter::setCurrentFitVal(double cfv){
  _meanResult = cfv;
}

void MinuitParameter::setInit( const double& val ){
  _meanInit = val; 
}

void MinuitParameter::setResult(double fitMean
			     , double fitErr
			     , double fitErrPos
			     , double fitErrNeg){
  _meanResult   = fitMean;
  _errResult    = fitErr;
  _errPosResult = fitErrPos;
  _errNegResult = fitErrNeg;

}

bool MinuitParameter::setParSet(MinuitParameterSet* ps){
  //  if(0 == ps) return false;
  if(ps == _pset) return true;
  if(0 != _pset) _pset->unregister(this);
  _pset = ps;
  _psetIndex = -9999;
  if(0 != ps) ps->add(this);
  return true;
}
bool MinuitParameter::setParSetIndex(int psetIndex){
  _psetIndex = psetIndex;
  return true;
}
int MinuitParameter::parSetIndex() const{
  return _psetIndex;
}

bool MinuitParameter::scan() const{
  return 0;
}
double MinuitParameter::scanMin() const{
  return 0.0;
  //else return _scanParameters.getVal(0);
}
double MinuitParameter::scanMax() const{
  return 0;
//  if(_scanParameters.size() < 2) return 0.0;
//  else return _scanParameters.getVal(1);
}

void MinuitParameter::print(std::ostream& os) const{
  // ... in a format that can be read back in
  os << "\"" <<  name() << "\""
     << "\t" << iFixInit()
     << "\t" << mean()
     << "\t" << err()
     << "\t" << minInit()
     << "\t" << maxInit();
}

void MinuitParameter::printResultVsInput(std::ostream& os) const{

  double pull = -9999;
  if(err() != 0) pull = (mean() - meanInit() )/err();


  os << "\"" <<  name() << "\""
     << "\t" << iFixInit()
     << "\t" << mean() << " - " << meanInit() << " / " << err()
     << " = \t" << pull;
}

MinuitParameter::MinuitParameter(const std::string& name
			   , const int& fix
			   , const double& mean
			   , const double& step
			   , const double& mi
			   , const double& ma
			   , MinuitParameterSet& setPtr
			   )
  : 
    _iFixInit(fix)
  , _name(name)
  , _meanInit(mean)
  , _stepInit(step)
  , _minInit(mi)
  , _maxInit(ma)
  , _pset(0)
{
  DEBUG("Building parameter : " << name );
  
  setParSet(&setPtr);
  resetToInit();
}



void MinuitParameter::resetToInit(){
  _meanResult   = _currentFitVal = _meanInit;
  _errResult    = _stepInit;
  _errPosResult = -9999;
  _errNegResult = -9999;
}


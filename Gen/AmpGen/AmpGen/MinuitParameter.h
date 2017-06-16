#ifndef IMINUIT_PARAMETER_HH
#define IMINUIT_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <iostream>

//class TMinuit;

namespace AmpGen{

  class MinuitParameterSet;

  class MinuitParameter{
    private : 
      int _iFixInit;
      std::string _name;
      double _meanInit;
      double _stepInit;
      double _minInit;
      double _maxInit;
      double  _meanResult; 
      double _errPosResult;
      double _errNegResult;
      double _errResult; 
      double _currentFitVal;
      MinuitParameterSet* _pset; 
      int   _psetIndex;
    protected:
      MinuitParameter(){};
      bool setParSet(MinuitParameterSet* ps);
      bool setParSetIndex(int psetIndex);
    public:
      MinuitParameter( const std::string& name,
          const int& iFixInit,
          const double& mean,
          const double& step,
          const double& min,
          const double& max,
          MinuitParameterSet& mps ); 


      int iFixInit() const;
      bool hidden() const;
      const std::string& name() const;

      double meanInit() const;
      double stepInit() const;
      double minInit() const;
      double maxInit() const;
      void setInit( const double& init );

      double mean() const;
      double err() const;
      double errPos() const;
      double errNeg() const;

      double scanMin() const;
      double scanMax() const;   
      bool scan() const  ;
      double getCurrentFitVal() const ;
      void setCurrentFitVal(double cfv);
      const MinuitParameterSet* parSet() const;
      int parSetIndex() const;

      void setResult(double fitMean
          , double fitErr
          , double fitErrPos
          , double fitErrNeg);
      void resetToInit();

      //    operator double() const;

      void print(std::ostream& os = std::cout) const;
      void printResultVsInput(std::ostream& os = std::cout) const;

      ~MinuitParameter(){}
      void setName( const std::string& name ){ _name = name ; }
      friend class MinuitParameterSet;

  };
}//namespace AmpGen

#endif
//

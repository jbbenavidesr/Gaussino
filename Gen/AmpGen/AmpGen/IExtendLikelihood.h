#ifndef IEXTENDLIKELIHOOD_H
#define IEXTENDLIKELIHOOD_H
#include <string>

namespace AmpGen { 
  class MinuitParameterSet; 
  class MinuitParameter;

  class IExtendLikelihood {
    public:
      virtual double getVal() const = 0; 
      virtual void configure( const std::string& configString , const AmpGen::MinuitParameterSet& mps) = 0; 
      virtual IExtendLikelihood* create() = 0; 
  }; 

  class GaussianConstraint : public IExtendLikelihood {
    public: 
      virtual double getVal() const; 
      virtual void configure( const std::string& configString, const AmpGen::MinuitParameterSet& mps ) ; 
      virtual IExtendLikelihood* create(){
        return new GaussianConstraint();
      }
      static std::string _id; 
     private:
       MinuitParameter* m_param;
       double m_mean;
       double m_sigma;
  };

}

#endif

#ifndef BW_H
#define BW_H

#include <map>
#include "AmpGen/Expression.h"
#include "AmpGen/Factory.h"

#define DECLARE_LINESHAPE(X)         \
  struct X : public AmpGen::IShape { \
  static std::string _id ; \
  virtual AmpGen::Expression get(const AmpGen::Expression& s, const AmpGen::Expression& s1, const AmpGen::Expression& s2, const std::string& particleName, const unsigned int& L , const std::string& lineshapeModifier \
      ,  std::vector<std::pair< std::string, AmpGen::Expression >>* dbexpressions =0) ;  \
  \
} \

#define DEFINE_LINESHAPE(X) \
  REGISTER(IShape,X);       \
  AmpGen::Expression X::get( const AmpGen::Expression& s, const AmpGen::Expression& s1, const AmpGen::Expression& s2, const std::string& particleName, const unsigned int& L, const std::string& lineshapeModifier, std::vector<AmpGen::DBSYMBOL>* dbexpressions )

namespace AmpGen { 
  /// some general helper functions /// 
  Expression Q2(const Expression& Msq, const Expression& M1sq, const Expression& M2sq) ;

  Expression kFactor( const Expression& mass, const Expression& width ,
      std::vector<std::pair< std::string, Expression >>* dbexpressions =0
      ) ;
  Expression BlattWeisskopf_Norm( const Expression& z2, const Expression& z02, unsigned int L);

  Expression BlattWeisskopf( const Expression& z2, unsigned int L);

  Expression Gamma( const Expression& s
      , const Expression& s1
      , const Expression& s2
      , const Expression& mass
      , const Expression& width
      , const Expression& radius
      , unsigned int L
      , std::vector<DBSYMBOL>* dbexpressions=0);

  /// Shape base class 
  struct IShape { 

    virtual ~IShape(){} 
    virtual Expression get( 
        const Expression& s,
        const Expression& s1,
        const Expression& s2,
        const std::string& particleName,
        const unsigned int& L ,
        const std::string& lineshapeModifier, 
        std::vector< std::pair< std::string, Expression >>* dbexpressions =0)=0 ;
    IShape* create(){ return this ; }
  };
  
  /// static, global lineshape factory /// 
  class LineshapeFactory : public Factory<IShape> {
    public :
    static Expression getLineshape( const std::string& lineshape
        , const Expression& s
        , const Expression& s1
        , const Expression& s2
        , const std::string& particleName
        , const unsigned int& L
        , std::vector< std::pair< std::string, Expression >>* dbexpressions =0 ) ;

    static bool isLineshape( const std::string& lineshape ) ;
  }; 
} 

DECLARE_LINESHAPE(NonRes);
DECLARE_LINESHAPE(BW);
DECLARE_LINESHAPE(LASS);
DECLARE_LINESHAPE(gLASS);
DECLARE_LINESHAPE(Flatte);

DECLARE_LINESHAPE(GS);

DECLARE_LINESHAPE(SBW);
DECLARE_LINESHAPE(Bugg);
DECLARE_LINESHAPE(FormFactor);

DECLARE_LINESHAPE(Spline);
DECLARE_LINESHAPE(WidthSpline);
DECLARE_LINESHAPE(FormFactorSpline);
DECLARE_LINESHAPE(DecaySpline);
DECLARE_LINESHAPE(LassSpline);
DECLARE_LINESHAPE(Lupton);

#endif

#ifndef BW_H
#define BW_H

#include <map>
#include "AmpGen/Expression.h"

#define DECLARE_LINESHAPE(X) struct X : public AmpGen::IShape { \
  virtual AmpGen::Expression get(const AmpGen::Expression& s, const AmpGen::Expression& s1, const AmpGen::Expression& s2, const std::string& particleName, const unsigned int& L , const std::string& lineshapeModifier \
      ,  std::vector<std::pair< std::string, AmpGen::Expression >>* dbexpressions =0) ;  \
  \
} \

#define DEFINE_LINESHAPE(X) Expression X::get( const Expression& s, const Expression& s1, const Expression& s2, const std::string& particleName, const unsigned int& L, const std::string& lineshapeModifier, std::vector<DBSYMBOL>* dbexpressions )

#define ADD_LINESHAPE(X) LineshapeFactory::get()->addLineshape( #X, new X() )

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

  struct IShape { 

    virtual ~IShape(){} 
    virtual Expression get( 
        const Expression& s,
        const Expression& s1,
        const Expression& s2,
        const std::string& particleName,
        const unsigned int& L ,
        const std::string& lineshapeModifier, 
        std::vector< std::pair< std::string, Expression >>* dbexpressions =0)=0 ; };

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


namespace AmpGen { 
  class LineshapeFactory {

    static LineshapeFactory *s_instance; 
    public:
    std::map<std::string,IShape*> m_lineshapes;

    LineshapeFactory() : 
      m_lineshapes( 
          { {"NonRes", new NonRes()},
          {"BW", new BW() },
          {"LASS", new LASS()},
          {"gLASS", new gLASS()},
          {"Flatte", new Flatte()},
          {"Bugg", new Bugg()},
          {"SBW", new SBW() },
          {"GS", new GS() },
          {"FormFactor", new FormFactor() },
          {"Spline", new Spline() },
          {"WidthSpline", new WidthSpline() },
          {"FormFactorSpline", new FormFactorSpline() },
          {"DecaySpline", new DecaySpline() },
          {"BlattWeisskopf", new NonRes() },
          {"LassSpline", new LassSpline() },
          {"Lupton", new Lupton() } 
          }) {}; 

    static LineshapeFactory* get(){
      if( !s_instance ) 
        s_instance = new LineshapeFactory();
      return s_instance; 
    };
    static Expression getLineshape( const std::string& lineshape
        , const Expression& s
        , const Expression& s1
        , const Expression& s2
        , const std::string& particleName
        , const unsigned int& L
        , std::vector< std::pair< std::string, Expression >>* dbexpressions =0 ) ;
    void addLineshape( const std::string& name, IShape* shape){
      if( m_lineshapes.find(name) != m_lineshapes.end() ){
        IShape* oldShape = m_lineshapes.find(name)->second; 
        delete oldShape; 
        WARNING("Overwriting lineshape - " << name);
      }
      m_lineshapes[name] = shape; 
    }

    const std::map<std::string,IShape*>& lineshapes() const{ return get()->m_lineshapes; }
    static bool isLineshape( const std::string& lineshape ) ;
  }; 
} 

#endif

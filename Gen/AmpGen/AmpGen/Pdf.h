#include "AmpGen/CompiledExpression.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/EventList.h"

#include <memory>
#include <functional>
// #include "Cubature/cubature.h"
#include "Cubature/cubature.h"

#include "AmpGen/FitParameter.h"
#include "AmpGen/MinuitParameter.h"
#include "AmpGen/MinuitParameterSet.h"

/// one dimensional PDF for mass fits
class Pdf {

  private : 
  struct addressMapping {
    AmpGen::MinuitParameter* source;
    unsigned int param;
    addressMapping( AmpGen::MinuitParameter* _source, 
                                  
                                unsigned int _param ) : 
    source(_source), param(_param) {};
  };

  std::vector<addressMapping> m_addressMapping; /// complicated transfer logic //// 
  AmpGen::MinuitParameter* m_yield; 
  std::shared_ptr<CompiledExpression<double>> m_pdf ; 
  double m_min;
  double m_max; 
  double m_norm; 
  //EventList<Event> m_events;  
 
  public:
  void setEvents( EventList<Event>& list ){ ; }

  Pdf( const CompiledExpression<double>& pdf , 
       const EventList<Event>& eventList,
       AmpGen::MinuitParameter* yield, 
       const double& min,
       const double& max
    ) : 
    m_pdf( std::make_shared<CompiledExpression<double>>( pdf ) ),
    m_min(min),
    m_max(max),
    m_yield(yield) {
    auto dependencies = pdf.getAddressMapping();
    for( auto& depend : dependencies ){
      m_addressMapping.emplace_back( 
          new AmpGen::FitParameter( depend.first ) , depend.second.first );
    }
  };
  double prob( const Event& evt ){ return m_pdf->getVal( evt.address(0) ) * m_norm ; } 

  //std::vector<CompiledExpression<std::complex<double>>>& getExpressions(){ return m_pdfs;}
  CompiledExpression<double>& getExpressions(){ return *m_pdf; }

  int Integrand( unsigned dim, const double* x, void *data_, unsigned fdim, double *retval ){
    *retval = m_pdf->getVal( x );
    return 0; 
  }    
  void prepare(){
    for( auto& addr : m_addressMapping )
      m_pdf->setExternal( addr.source->mean(), addr.param );
    m_norm = m_yield->mean()*integrate(); 
  }; 
  double integrate(){
    int comp, nregions, neval, fail;
    double integral, error, prob;
    std::function<int(unsigned int, const double*, void*, unsigned int, double*)>
      f = std::bind( &Pdf::Integrand, this, 
          std::placeholders::_1, 
          std::placeholders::_2,
          std::placeholders::_3,
          std::placeholders::_4,
          std::placeholders::_5 ) ; 
    hcubature( 1, f , NULL, 1, &m_min, &m_max, 50000, 0, 1e-6, ERROR_INDIVIDUAL, &integral, &error );
    return integral; 
  };
} ; 

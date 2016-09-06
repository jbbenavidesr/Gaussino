#include <tuple>
#include "AmpGen/EventList.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/FCNLibrary.h"

/// AmpGen ////
#include "AmpGen/Minimisable.h"

#include <omp.h>
/* A tuple PDF is the log likliehood of the form 

   -2*LL(event) = -2*log( Sum( i, P_i(event) ) )
   Where P_i are some probability density functions 
   The sum is variadically unrolled at compile time, i.e. the wrapper
   is the same for 1..N pdfs. The unrolling should be properly inlined,
   hence N can be reasonably large with out afflicting either 
   compile time or binary size.   
*/

namespace AmpGen { 
template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
  for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
  { }

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
  for_each(std::tuple<Tp...>& t, FuncT f)
  {
    f(std::get<I>(t));
    for_each<I + 1, FuncT, Tp...>(t, f);
  }

template< class RETURN_TYPE, class ...TYPES> 
class SumPDF : public Minimisable, public FCNLibrary<RETURN_TYPE> {
  public:
    SumPDF( const TYPES & ...  _pdfs ) : m_pdfs( std::tuple<TYPES...>(_pdfs...) ) {}
    std::tuple<TYPES...> m_pdfs;
    EventList* m_events;
    virtual bool compile(const std::string& fname, bool debug=false ){
      buildLibrary();
      return FCNLibrary<RETURN_TYPE>::compile( fname , debug );
    }
    double getVal(){ 
      double LL=0;
      for_each( m_pdfs, [](auto& A){ A.prepare(); });
      double prob;
      #pragma omp parallel for private(prob) reduction(+:LL)
      for(unsigned int i = 0 ; i < m_events->size(); ++i ){
        prob=0;
        auto& evt = (*m_events)[i];
        for_each( this->m_pdfs, [&evt,&prob](auto & pdf){ 
            prob += pdf.prob(evt);} ); 
        LL += log( prob );
      } 
      //INFO( "FCN = " << std::setprecision(10) << -2*LL);
      return -2*LL; 
    }
    void setEvents( EventList& events ){
      m_events = &events;
      for_each( m_pdfs, [&events](auto& pdf){ pdf.setEvents(events);});
    }
    void setMC( EventList& mc ){
      for_each( m_pdfs, [&mc](auto& pdf){ pdf.setMC(mc);});
    }
    void buildLibrary(){
      INFO("Building library for " << nPDFs() );
      for_each( m_pdfs, [this](auto& A){ 
        INFO("Building library for " << A.prefix() ); 
        this->add( A.getExpressions() );});
    }
    void debug(const unsigned int& N=0, const std::string& nameMustContain=""){
      for_each( m_pdfs, [=](auto& A){
        A.debug(N, nameMustContain);
      } );
    }
    void reweight( EventList& evts , 
        const double& population=1, 
        const unsigned int& category=1){ /// starting weight //// 
      for_each( m_pdfs, [&evts](auto& A){ 
        A.setMC( evts) ; 
        A.prepare() ; 
        A.setWeightIntegral( evts ); } );
      unsigned int size = sizeof...(TYPES);
      unsigned int sizeOfWeights = category + 1 + ( size == 1 ? 0 : size );
      INFO( "Sizes = " << size << " sizeOfWeights = " << sizeOfWeights);
      for( auto& evt : evts ){
        int size = sizeof...(TYPES);
        evt.resizeWeights(sizeOfWeights);
        double probability(0);
        unsigned int counter=category;
        for_each( m_pdfs, [&probability,&evt,&population,&counter]( auto& pdf ){
          double num = pdf.prob(evt) * evt.weight(0) ; 
          double den = ( evt.genPdf() * pdf.weightIntegral() );
          evt.setWeight( num*population / den , counter++ );
          probability += num / den;
          DEBUG( pdf.prob(evt) );
        } );
        
        if( size != 1 ) evt.setWeight( population * probability , counter); /// normalise ////
      }
    }
    /*
    double norm(EventList<Event>& evts ){
      double norm=0;
      for( auto& evt : evts ){
        for_each( m_pdfs, [&norm,&evt](auto & pdf) {
          double num = pdf.prob(evt) * evt.weight(0) ;
          double den = ( evt.genPdf() * pdf.weightIntegral() );
          norm += num / den; 
        }
        );
      } 
     return norm; 
    };
    */
//   unsigned int nPDFs(){ return std::tuple_size<std::tuple<TYPES...>>::value ; } 
  std::size_t nPDFs(){ return sizeof...(TYPES) ; } 
}; } 

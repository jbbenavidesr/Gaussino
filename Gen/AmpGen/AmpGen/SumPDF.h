#include <tuple>
#include "AmpGen/EventList.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/FCNLibrary.h"

/// AmpGen ////
#include "AmpGen/Minimisable.h"
#include "AmpGen/IExtendLikelihood.h"

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

  template< class ...TYPES> 
    class SumPDF : public Minimisable, public FCNLibrary {
      public:
        SumPDF( const TYPES & ...  _pdfs ) : m_pdfs( std::tuple<TYPES...>(_pdfs...) ) {}
        std::tuple<TYPES...> m_pdfs;
        EventList* m_events;
        std::vector<IExtendLikelihood*> m_extendedTerms; 

        std::tuple<TYPES...> pdfs() const { return m_pdfs; }
        virtual bool compile(const std::string& fname, bool debug=false ){
          buildLibrary();
          return FCNLibrary::compile( fname , debug );
        }
        //// these structs essentially play the role of auto lambdas
        //// due to requirement that we have c++11 compatiblity. 

        struct do_prepare {
          template<class TYPE> void operator() (TYPE& obj ){ obj.prepare() ; }
        };
        struct do_sum {
          do_sum( const Event& _evt, double& _prob ) : evt(_evt), prob(_prob) {};
          template<class TYPE> void operator() (TYPE& obj ){ prob+=obj.prob(evt); }
          const Event& evt;
          double& prob;
        };
        struct do_set_events {
          do_set_events( EventList& _evts ) : events( _evts ) {};
          template<class TYPE> void operator() (TYPE& obj ){ obj.setEvents(events); }
          EventList& events;
        };
        struct do_set_mc {
          do_set_mc( EventList& _evts ) : events( _evts ) {};
          template<class TYPE> void operator() (TYPE& obj ){ obj.setMC(events); }
          EventList& events;
        };
        struct do_build_library {
          do_build_library( FCNLibrary* rt ) : lib(rt) {}
           template <class TYPE>
           void operator()( TYPE& obj ){ lib->add( obj.getExpressions() ) ; }
          FCNLibrary* lib;    
        };
        struct do_debug {
          do_debug( const unsigned int& _N, const std::string& _nameMustContain ) : 
            N(_N), nameMustContain(_nameMustContain ) {};
          template<class TYPE> void operator()( TYPE& obj ){ obj.debug(N,nameMustContain) ; }
          const unsigned int& N;
          const std::string& nameMustContain;
        };
        struct do_reweight_prepare {
          do_reweight_prepare( EventList& _evts ) : evts(_evts) {} ;
          template<class TYPE> void operator()( TYPE& obj ){ 
            obj.setMC( evts) ;
            obj.prepare() ;
            obj.setWeightIntegral( evts ); 
          }
          EventList& evts;
        };
        struct do_reweight {
          do_reweight( double& _probability, Event& _evt, const double& _population, unsigned int& _counter ) :
            probability(_probability), evt(_evt), population(_population),counter(_counter){}; 
          template<class TYPE> void operator()( TYPE& pdf ){
            double num = pdf.prob(evt) * evt.weight(0) ;
            double den = ( evt.genPdf() * pdf.weightIntegral() );
            evt.setWeight( num*population / den , counter++ );
            probability += num / den;
            DEBUG( pdf.prob(evt) );
          }
          double& probability;
          Event& evt;
          const double& population;
          unsigned int& counter;
        };
        double getVal(){ 
          double LL=0;
          for_each( m_pdfs, do_prepare() );
          double prob;
          #pragma omp parallel for private(prob) reduction(+:LL)
          for(unsigned int i = 0 ; i < m_events->size(); ++i ){
            prob=0;
            auto& evt = (*m_events)[i];
            for_each( this->m_pdfs, do_sum( evt, prob ) ) ;
            LL += log( prob );
          }
          for( auto& extendTerm : m_extendedTerms ) 
            LL += extendTerm->getVal();
          return -2*LL; 
        }
        void setEvents( EventList& events ){
          m_events = &events;
          for_each( m_pdfs, do_set_events(events) );
        }
        void addExtendedTerm( IExtendLikelihood* m_term ){
          m_extendedTerms.push_back( m_term );
        }
        void setMC( EventList& mc ){
          for_each( m_pdfs, do_set_mc(mc));
        }
        void buildLibrary(){
          INFO("Building library for " << nPDFs() );
          for_each( m_pdfs, do_build_library(this) );
        }
        void debug(const unsigned int& N=0, const std::string& nameMustContain=""){
          for_each( m_pdfs, do_debug(N,nameMustContain) );
        }
        void reweight( EventList& evts , 
            const double& population=1, 
            const unsigned int& category=1){ /// starting weight //// 
          for_each( m_pdfs, do_reweight_prepare(evts) );
          unsigned int size = sizeof...(TYPES);
          unsigned int sizeOfWeights = category + 1 + ( size == 1 ? 0 : size );
          INFO( "Sizes = " << size << " sizeOfWeights = " << sizeOfWeights);
          for( auto& evt : evts ){
            int size = sizeof...(TYPES);
            evt.resizeWeights(sizeOfWeights);
            double probability(0);
            unsigned int counter=category;
            for_each( m_pdfs, do_reweight(probability,evt,population,counter) );
            if( size != 1 ) evt.setWeight( population * probability , counter); /// normalise ////
          }
        }
        std::size_t nPDFs(){ return sizeof...(TYPES) ; } 
    }; 
} 

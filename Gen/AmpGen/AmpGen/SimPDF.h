#include <tuple>
#include "AmpGen/EventList.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/FCNLibrary.h"

/// AmpGen ////
#include "AmpGen/Minimisable.h"

#include <omp.h>
/* A SimPDF is the log likliehood of the form 

   -2*LL = Sum( i, LL(i)) 
*/


template< class RETURN_TYPE, class ...TYPES> 
class SimPDF : public AmpGen::Minimisable, public FCNLibrary<RETURN_TYPE> {
  public:
    SimPDF( const TYPES & ...  _pdfs ) : m_pdfs( std::tuple<TYPES...>(_pdfs...) ) {}
    std::tuple<TYPES...> m_pdfs;
    EventList<Event>* m_events;
    virtual bool compile(const std::string& fname ){
      buildLibrary();
      return FCNLibrary<RETURN_TYPE>::compile( fname );
    }
    double getVal(){ 
      double LL;
      for_each( this->m_pdfs, [&LL](auto& pdf){ LL += pdf.getVal();} );
      return LL; 
    }
   void buildLibrary(){
     for_each( this->m_pdfs, [this](auto& pdf){ pdf.buildLibrary(); this->merge( pdf ) ;  } ); 
      
   }
   unsigned int nPDFs(){ return sizeof...(TYPES) ; } 
};

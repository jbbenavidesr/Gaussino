#include "AmpGen/EventType.h"
#include "AmpGen/EventList.h"
#include "AmpGen/FastCoherentSum.h"


#include "TGenPhaseSpace.h"

namespace AmpGen { 
  class Generator {
    private:
      FastCoherentSum& m_pdf;
      EventType m_eventType;
      TGenPhaseSpace m_gps;
      unsigned int m_generatorBlock;
      TRandom* m_rnd; 
    public:
      Generator( FastCoherentSum& pdf, const EventType& type ) : 
        m_pdf( pdf ), 
        m_eventType( type ),
        m_generatorBlock(1000000)
    {
      TLorentzVector motherP(0,0,0,m_eventType.motherMass() );
      m_gps.SetDecay( motherP, m_eventType.size() , m_eventType.masses().data() );  
    }
      void setRandom(TRandom* rand ){ m_rnd = rand ; } 
      void fillEventListPhaseSpace( EventList& list, const unsigned int& N){
        unsigned int rejected = 0 ;
        list.reserve(N);
        while( list.size() < N ){
          if( m_gps.Generate() > m_rnd->Uniform() ){
            Event newEvent( 4*m_eventType.size() , m_pdf.size() );
            for( unsigned int i = 0 ; i < m_eventType.size() ; ++i )

              newEvent.set( i, { m_gps.GetDecay(i)->Px(),
                  m_gps.GetDecay(i)->Py(),
                  m_gps.GetDecay(i)->Pz(),
                  m_gps.GetDecay(i)->E() } ) ;
            newEvent.setWeight(1,0);
            list.push_back( newEvent );
          } else rejected ++;
        };
        INFO( "Stage 1 efficiency = " 
            << 100.*list.size() / (list.size() + rejected ) 
            << "%, yield = " << list.size() );
      };

      void fillEventList( EventList& list, const unsigned int& N ){

        double normalisationConstant=0;
        TRandom3 qRand;
        unsigned int size0 = list.size();
        auto tStartTotal = std::chrono::high_resolution_clock::now() ;
        while( list.size() - size0 < N ){
          auto t_start = std::chrono::high_resolution_clock::now();

          EventList mc( m_eventType );
          fillEventListPhaseSpace( mc, m_generatorBlock );
          m_pdf.setEvents( mc );
          m_pdf.prepare();

          if( normalisationConstant==0 ){
            double max=0;
            for( auto& evt : mc ){
              double value = std::norm( m_pdf.getVal(evt) );
              if( value > max ) max = value;    
            }
            normalisationConstant=max*1.5;
          }
          unsigned int previousSize = list.size(); 
          for( auto& evt : mc ){
            double value = std::norm( m_pdf.getVal( evt ) );
            if( value > normalisationConstant ){
              WARNING("PDF value exceeds norm value");
            };
            if( value > normalisationConstant * qRand.Uniform() ){
              evt.setGenPdf( value );  
              list.push_back( evt );
            }
            if( list.size() - size0 == N ) break; 
          }
          auto t_end = std::chrono::high_resolution_clock::now();
          double time = std::chrono::duration<double, std::milli>(t_end-t_start).count() ;
          INFO("Stage 2 efficiency = " << 100.* ( list.size() - previousSize ) 
              / (double)m_generatorBlock 
              << "% yield = " << list.size() << " , time = " << time << "ms" );
        };
        double time = std::chrono::duration<double, std::milli>( 
            std::chrono::high_resolution_clock::now() -tStartTotal).count() ;
        INFO("Generated " << N << " events in " << time << " ms");
      };
  };}

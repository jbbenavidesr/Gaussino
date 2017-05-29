#include "AmpGen/EventType.h"
#include "AmpGen/EventList.h"

#include "TGenPhaseSpace.h"
#include "AmpGen/PhaseSpace.h"

namespace AmpGen { 
  template <class PDF>
  class Generator {
    private:
      PDF& m_pdf;
      EventType m_eventType;
      AmpGen::PhaseSpace m_gps;
      TGenPhaseSpace m_gps_root;
      unsigned int m_generatorBlock;
      TRandom* m_rnd; 
    public:
      Generator( PDF& pdf, const EventType& type ) : 
        m_pdf( pdf ), 
        m_eventType( type ),
        m_generatorBlock(1000000),
        m_rnd(0)
    {
      TLorentzVector motherP(0,0,0,m_eventType.motherMass() );
      m_gps.SetDecay( m_eventType.motherMass(), m_eventType.masses() );  
      m_gps_root.SetDecay( motherP, m_eventType.size() , m_eventType.masses().data() );
    }
      void setRandom(TRandom* rand ){ 
        m_rnd = rand ; 
        m_gps.setRandom( m_rnd );
      } 
      void fillEventListPhaseSpace( EventList& list, const unsigned int& N, bool useRoot=false ){
        fillEventListPhaseSpace( list, N, []( const Event& evt ){ return 1; } , useRoot );
      };      

      template <class HARD_CUT > 
      void fillEventListPhaseSpace( EventList& list, const unsigned int& N, HARD_CUT cut , bool useRoot=false){

        unsigned int rejected = 0 ;
        list.reserve(N);
        auto t_start = std::chrono::high_resolution_clock::now();
        if( !useRoot ){
          for( unsigned int j = 0 ; j < N ; ++j ){ 
            m_gps.Generate() ;
            Event newEvent( 4*m_eventType.size() , m_pdf.size() );
            for( unsigned int i = 0 ; i < m_eventType.size() ; ++i )

              newEvent.set( i, { m_gps.GetDecay(i)->Px(),
                  m_gps.GetDecay(i)->Py(),
                  m_gps.GetDecay(i)->Pz(),
                  m_gps.GetDecay(i)->E() } ) ;
            newEvent.setWeight(1,0);
            if( cut(newEvent) ) list.push_back( newEvent );
          }
        }
        else { 
          while( list.size() < N ){
            if( m_gps_root.Generate() < m_rnd->Uniform() ){ rejected++; continue ; }
            Event newEvent( 4*m_eventType.size() , m_pdf.size() );
            for( unsigned int i = 0 ; i < m_eventType.size() ; ++i )
              newEvent.set( i, { 
                  m_gps_root.GetDecay(i)->Px(),
                  m_gps_root.GetDecay(i)->Py(),
                  m_gps_root.GetDecay(i)->Pz(),
                  m_gps_root.GetDecay(i)->E() } ) ;
            newEvent.setWeight(1,0);    
            list.push_back( newEvent );
          }
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(t_end-t_start).count() ;

        INFO( "Stage 1 efficiency = " 
            << 100.*list.size() / (list.size() + rejected ) 
            << "%, yield = " << list.size() << " time = " << time );
      }
      void fillEventList( EventList& list, 
                          const unsigned int& N,
                          bool useRoot=false ){
        fillEventList( list, N ,  []( const Event& evt ){ return 1; } , useRoot);

      }

      template <class HARD_CUT>
      void fillEventList( EventList& list, 
                     const unsigned int& N,
                     HARD_CUT cut,
                     bool useRoot = false ){
        if( m_rnd == nullptr ){
          ERROR("Random generator not set!") ;
          return;
        }
        double normalisationConstant=0;
        unsigned int size0 = list.size();
        auto tStartTotal = std::chrono::high_resolution_clock::now() ;
        while( list.size() - size0 < N ){
          auto t_start = std::chrono::high_resolution_clock::now();
          EventList mc( m_eventType );
          fillEventListPhaseSpace( mc, m_generatorBlock, cut, useRoot );
          auto t_stage2 = std::chrono::high_resolution_clock::now();

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
            }
            if( value > normalisationConstant * m_rnd->Rndm() ){
              evt.setGenPdf( value );  
              list.push_back( evt );
            }
            if( list.size() - size0 == N ) break; 
          }
          auto t_end = std::chrono::high_resolution_clock::now();
          double time = std::chrono::duration<double, std::milli>(t_end-t_stage2).count() ;
          double timeTotal = std::chrono::duration<double, std::milli>(t_end-t_start).count() ;
          INFO("Stage 2 efficiency = " << 100.* ( list.size() - previousSize ) 
              / (double)m_generatorBlock 
              << "% yield = " << list.size() << " , time = " << time << "ms" );
          INFO("Total time = " << timeTotal << " ms");
        }
        double time = std::chrono::duration<double, std::milli>( 
            std::chrono::high_resolution_clock::now() -tStartTotal).count() ;
        INFO("Generated " << N << " events in " << time << " ms");
      }
  };
}

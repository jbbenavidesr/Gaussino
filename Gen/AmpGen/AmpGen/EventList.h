#ifndef EVENT_H
#define EVENT_H 1

#include "AmpGen/Particle.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/CompiledExpression.h"
#include "AmpGen/EventType.h"

#include "AmpGen/TeXFormat.h"
#include "AmpGen/MsgService.h"
#include <chrono>

#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TEventList.h"
#include "TRandom.h"
/// OPEN MP
//

#include <omp.h>

namespace AmpGen { 
  static std::vector<std::vector<unsigned int>> gChi2Indices = { {1,2,3},{0,1},{0,2},{2,3},{0,1,2} };


  struct plotAxis { 
    std::string name;
    std::string title;
    double min;
    double max;
    unsigned int nBins; 
    std::vector<unsigned int> indices;
    plotAxis( const std::string& _title, 
        const double& _min, 
        const double& _max , 
        const unsigned int& _nBins=100) : title(_title), min(_min), max(_max), nBins(_nBins) {}

  };

  class Event {
    private:
      std::vector<double> m_event; 
      std::vector<std::complex<double>> m_cache;
      double m_genPdf;
      std::vector<double> m_weights; /// we store a vector of weights because then we can reweight by whatever for whatever calculation. Needs thought ...
      inline double get(const unsigned int& index ) const { return m_event[index]; };
    public:

      bool isNaN(){
        for( unsigned int j =0;j<m_event.size();++j){
          if( m_event[j] > 10e10 || m_event[j] < -10e10){
            print();
            return true ;
          };
        };
        return false; 
      };
      Event( const unsigned int& N, const unsigned int& cacheSize=0 ) : 
        m_event(std::vector<double>(N)), 
        m_cache(cacheSize), 
        m_genPdf(1),
        m_weights(std::vector<double>(1,0)) 
    {};
      void dumpCache(){
        for( unsigned int i = 0 ; i < m_cache.size(); ++i){
          INFO("Cache adddress [" << i << "] = " << m_cache[i] );
        }
      }
      void set( const unsigned int& i, const std::vector<double>& p ){
        for( unsigned int j = 0 ; j < 4; ++j) m_event[4*i + j ] = p[j];
      }
      unsigned int size(){ return m_event.size(); } 
      const double* address(const unsigned int ref ) const { return &(m_event[ref]) ; }
      double* address( const unsigned int& ref){ return &(m_event[ref]); }
      void swap( const unsigned int& i , const unsigned int& j ){
        /// swap particle I with particle J ///
        double tmp[4];
        memmove( tmp, &m_event[4*j], sizeof(tmp)); 
        memmove( &m_event[4*j], &m_event[4*i],sizeof(tmp));
        memmove( &m_event[4*i], &tmp,sizeof(tmp));
      }; 
      void invertParity(){
        for( unsigned int i = 0 ; i < m_event.size() / 4  ; ++i ){
          m_event[4*i + 0 ] = -m_event[4*i+0] ;
          m_event[4*i + 1 ] = -m_event[4*i+1] ;
          m_event[4*i + 2 ] = -m_event[4*i+2] ;
        };
      };
      inline double* getEvent(){ return &(m_event[0]) ; }
      inline const double* getEvent() const { return m_event.data(); }
      void setCache( const std::complex<double>& m_value, const unsigned int& pos )
      { m_cache[pos] = m_value ; }
      unsigned int cacheSize(){ return m_cache.size() ; } 
      inline std::complex<double> getCache(const unsigned int& pos) const { return m_cache[pos];}
      inline std::vector<std::complex<double>>::const_iterator cacheBegin() const {return m_cache.cbegin(); }
      double* pWeight(){ return &(m_weights[0]); }
      double* pGenPdf(){ return &m_genPdf; }
      double weight() const { return m_weights.size() == 0 ? 1 : *(m_weights.rbegin()); } /// get the last weight added to the stack /// 
      double weight(const unsigned int& index) const { return m_weights[index]; }
      void setWeight( const double& weight , const unsigned int& index = 0){ m_weights[index] = weight ; }
      void resizeWeights( const unsigned int& size ){ if( size > m_weights.size() ) m_weights.resize(size) ; }
      double genPdf() const { return m_genPdf; }
      void setGenPdf( const double& genPdf ){ m_genPdf = genPdf ; } 
      void extendEvent(const double& value) { m_event.push_back( value ); } 

      void print() const {
        for( unsigned int i =  0 ; i < m_event.size(); ++i ) 
          std::cout << "E["<<i<<"] = "<< m_event[i] << std::endl; 
      }
      double pij( const unsigned int& i, const unsigned int& j) const {
        return get(i*4+3)*get(j*4+3) - 
          get(i*4+0)*get(j*4+0) -
          get(i*4+1)*get(j*4+1) -
          get(i*4+2)*get(j*4+2);
      }
      std::string to_string() const { 
        std::string weightString=std::to_string(m_weights[0] ) + "   " ;
        for( unsigned int i=1;i<m_weights.size(); ++i ) weightString+=std::to_string(m_weights[i]) + "    ";
        return weightString + std::to_string(weight());
      }
      double s( const unsigned int& index1, const unsigned int& index2 ) const {
        return ( get(4*index1+3) + get(4*index2+3) )*( get(4*index1+3) + get(4*index2+3) ) - 
          ( get(4*index1+0) + get(4*index2+0) )*( get(4*index1+0) + get(4*index2+0) ) -
          ( get(4*index1+1) + get(4*index2+1) )*( get(4*index1+1) + get(4*index2+1) ) -
          ( get(4*index1+2) + get(4*index2+2) )*( get(4*index1+2) + get(4*index2+2) ) ;
      };
      double s( const unsigned int& index1, const unsigned int& index2, const unsigned int& index3 ) const {
        return ( get(4*index1+3) + get(4*index2+3) + get(4*index3+3) )*( get(4*index1+3) + get(4*index2+3) + get(4*index3+3) ) - 
          ( get(4*index1+0) + get(4*index2+0) + get(4*index3+0) )*( get(4*index1+0) + get(4*index2+0) + get(4*index3+0) ) -
          ( get(4*index1+1) + get(4*index2+1) + get(4*index3+1) )*( get(4*index1+1) + get(4*index2+1) + get(4*index3+1) ) -
          ( get(4*index1+2) + get(4*index2+2) + get(4*index3+2) )*( get(4*index1+2) + get(4*index2+2) + get(4*index3+2) ) ;
      };

      double s( const std::vector<unsigned int>& indices ) const {
        if( indices.size() == 2 ) return s( indices[0], indices[1] );
        if( indices.size() == 3 ) return s( indices[0], indices[1], indices[2] );
        double E=0;
        double px=0;
        double py=0;
        double pz=0;
        for( auto& i : indices ){
          E +=get(i*4+3);
          px+=get(i*4+0);
          py+=get(i*4+1);
          pz+=get(i*4+2);
        }
        return E*E -px*px - py*py - pz*pz;
      }
      bool isIn( const unsigned int& index, const std::pair<double,double>& domain , bool dbThis=false) const {
        const double o1p = s( gChi2Indices[index]) ;
        return o1p > domain.first && o1p < domain.second ;
      }
  };

  template <typename EVENT>
    struct less_than{
      unsigned int m_index;
      less_than(unsigned int index) : m_index(index){};
      bool operator()(const double& x, const EVENT& o1){
        return o1.s( gChi2Indices[m_index ] ) > x;
      }
    };

  template <typename EVENT>
    struct sorter{
      unsigned int m_index;
      sorter(unsigned int index) : m_index(index){};
      bool operator()(const EVENT& o1, const EVENT& o2){
        return o1.s( gChi2Indices[m_index ] ) < o2.s( gChi2Indices[m_index] );
      }
    };

  class EventList : public std::vector<Event> {
    private: 
      EventType m_eventType; 
      std::map<std::string, unsigned int> m_extendedEventData;
      std::map<uint64_t,unsigned int> m_pdf_index;
    public:
      enum OPTIONS {
        PARITY        = (1<<0),
        GETPDF  = (1<<1)
      };

      EventType getEventType() const { return m_eventType; }
      void setEventType( const EventType& type ){ m_eventType = type ; } 
      inline double* getEvent( const unsigned int& index )  { 
        return (*this)[index].getEvent()  ; 
      }
      EventList() {} ; 

      double integral( const unsigned int& cat=0) const {
        double integral=0;
        for( auto& evt : *this ){ integral += evt.weight(cat) ; } 
        return integral;  
      };
      EventList( TTree* tree, 
          const std::vector<std::string>& branches, 
          const EventType& evtType, 
          const unsigned int& opt,  
          const std::vector<unsigned int> eventList = std::vector<unsigned int>() ) ;

      EventList(TTree* tree, 
          const EventType& particles , 
          const unsigned int& pdfsize, 
          const bool& flipState=false, 
          const double& scaleFactor=1 );

      EventList( const EventType& type ); 
      TTree* tree(const std::string& name ) ;   

      std::map<std::string, unsigned int> getExtendedEventFormat() const { return m_extendedEventData; } 
      /// extends event format arbitrarily /// 
      template< class FUNCTOR> unsigned int extendEvent( const std::string& name, FUNCTOR func ){

        unsigned int index = this->begin()->size();
        for( auto& evt : *this ){
          evt.extendEvent( func( evt ) );
        }
        m_extendedEventData[name ] = index; 
        return index;
      }

      template<class TYPE>
        unsigned int cacheIndex( const TYPE& particle ){ 

          auto pdfIndex = m_pdf_index.find(particle.hash());
          if( pdfIndex != m_pdf_index.end() ) return pdfIndex->second ;
          else { 
            unsigned int size = m_pdf_index.size();
            if( size > std::vector<Event>::at(0).cacheSize() )
              ERROR("Cache index " << size << " exceeds cache size = " << std::vector<Event>::at(0).cacheSize() );
            updateCache( particle, m_pdf_index.size() );
            m_pdf_index[particle.hash()]=size;
            return size;
          }
        }
      template<class TYPE> 
        void updateCache( const TYPE& particle, const unsigned int& index){
          //        omp_set_num_threads(16);
          DEBUG("updating cache for : " << particle.name() << " on " << std::vector<Event>::size() << " events" );
          #pragma omp parallel for
          for( unsigned int i = 0 ; i < std::vector<Event>::size(); ++i ){
            auto val = particle.getVal( (*this)[i].getEvent() );
            (*this)[i].setCache( val, index );
          }
        }

      template< class typeA, class typeB > 
        std::complex<double> integrate(const typeA& A, const typeB& B, bool recalculate=false){
          std::complex<double> sum(0.,0.);
          unsigned int iA = m_pdf_index[A.hash()];
          unsigned int iB = m_pdf_index[B.hash()];
          double real=0;
          double imag=0;
          double totalWeight=0;

          #pragma omp parallel for reduction(+:real,imag,totalWeight)
          for( unsigned int i = 0 ; i < std::vector<Event>::size(); ++i){
            double reweight = (*this)[i].weight(0) / (*this)[i].genPdf();
            std::complex<double> value = reweight * (*this)[i].getCache(iA)*std::conj( (*this)[i].getCache(iB) ) ;
            real += std::real(value);
            imag += std::imag(value);
            totalWeight += reweight; 
          }
          return std::complex<double>(real,imag)  / totalWeight;
        }

      std::pair<double, double> minmax( const std::vector<unsigned int>& indices ) const {

        std::vector<unsigned int> ivec( m_eventType.size() );
        std::iota( ivec.begin(), ivec.end(), 0 );
        double min(0);
        double max(m_eventType.motherMass());
        for( auto& x : indices )
          min += m_eventType.mass(x);
        for( auto& x : ivec )
          if( std::find( indices.begin(), indices.end(), x ) == indices.end() )
            max -= m_eventType.mass(x);
        DEBUG( "Binning = " << min << " , " << max );

        return std::pair<double,double>( min*min / (1.1*1000*1000), max*max * (1.1/1000000) );
      }
      std::vector<plotAxis> defaultProjections() const {
        std::vector<plotAxis> axes;
        std::vector<std::vector<unsigned int>> permutations;
        for( unsigned int r=2;r<m_eventType.size();++r){ /// loop over sizes ///
          auto combR = nCr(m_eventType.size(),r);
          for( auto& indices : combR ){
            auto mm = minmax( indices );
            plotAxis thisCombination(
                "s("+particleNamesFromIndices( indices ) + ") [GeV^{2}/c^{4}]", 
                mm.first,
                mm.second );
            thisCombination.name = "s"+vectorToString(indices);
            thisCombination.indices = indices; 
            axes.push_back( thisCombination );
          }
        }
        return axes;
      };

      std::vector<TH1D*> makePlots(const std::string& prefix="", 
          const unsigned int& nBins=100, 
          const unsigned int& category=0);

      std::string particleNamesFromIndices( const std::vector<unsigned int>& indices ) const {
        std::string thing="";
        for( auto& x : indices ){
          thing += getTexFromPDG( m_eventType[x], true );
        }
        return thing;
      }
      std::vector<TH2D*> makePlots2D( const std::string& prefix="", const unsigned int& category=0, const unsigned int& nBins = 25);

  };}

#endif

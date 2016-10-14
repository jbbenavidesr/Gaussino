#ifndef BINNING_H
#define BINNING_H

#include <complex>
#include <vector>
#include <algorithm>
#include "TH1D.h"
#include "TH2D.h"
#include <math.h>
#include "TProfile2D.h"
#include "THStack.h"
#include <iostream>
#include <fstream>



#include "AmpGen/EventList.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/EventType.h"

namespace AmpGen { 
class Bin {
  private: 
    std::vector<std::pair<double,double>> m_boundary; /// phase space boundary of this bin ///
    unsigned int m_dim; /// dimension of binning ///
    std::vector<Event> m_data; /// "DATA" contained within this bin /// 
    std::complex<double> m_value; 
    double norm1;
    double norm2; 
  public: 

    //split bin in two in direction i 
    Bin( const unsigned int& dim=5): m_boundary(dim,{0.,0.}), m_dim(dim) {}

    Bin( const std::string& line){ /// deserializer 
      std::vector<std::string> tokens=split(line,':');
      m_dim = tokens.size();
      for(auto& token : tokens ){
        std::vector<std::string> limits = split( token, ',');
        m_boundary.push_back( std::make_pair(stod(limits[0]),stod(limits[1])));
      };
    };
 
    void print(std::ostream& ost){  /// serializer ///
      
      for( unsigned int i = 0 ; i < dim(); ++i ){
        ost << min(i) << "," << max(i) << ":";
      }
      ost << "#"; 
    }; 


    void setValue( const std::complex<double>& value , const double& n1, const double& n2){ m_value = value ; norm1 = (n1); norm2 = (n2) ; }
    std::complex<double> getValue() const{ return m_value ; } 
    double getNorm1() const { return norm1 ; }
    double getNorm2() const { return norm2 ; } 
    //add a Dalitz-coordinate to the 
    void setBoundary(unsigned int i, double min, double max ){
      m_boundary[i] = {min,max};
    }
    void setBoundary( unsigned int i, std::pair<double,double> minmax ){
      m_boundary[i] = minmax;
    }
    double min( unsigned int i ) const { return m_boundary[i].first; }
    double max( unsigned int i ) const { return m_boundary[i].second; }

    double volume() const {
      double vol = 1;
      for( auto& d : m_boundary ){
        std::cout << d.second << " , " << d.first << std::endl;  
        vol *= ( d.second - d.first );
      }
      return vol;
    }
    std::pair<double,double> minmax( const unsigned int& index ) const { return m_boundary[index] ; }
    std::vector<Event>::const_iterator beginData() const { return m_data.cbegin(); }
    std::vector<Event>::const_iterator endData() const { return m_data.cend(); }
    unsigned int nEvents() const { return m_data.size() ; }
    unsigned int dim() const { return m_dim ; }
    
    const std::vector<Event>& Data() const { return m_data ; }

    void addData( std::vector<Event>::const_iterator begin, 
        std::vector<Event>::const_iterator end , bool checkBoundaries = false ){
      unsigned int failure=0;
      for( auto it = begin; it != end; ++it ){
        if( checkBoundaries ) {
          if ( ! add( *it ) ){
            add(*it,true);
          }
        } 
        else 
          m_data.push_back( *it );
      }
      if( checkBoundaries ) 
        INFO( "Number of events not within phase space = " << failure ) ; 
    }
    void copyBoundaries( const Bin& otherBin ){
      for( unsigned int index = 0;index<dim(); ++index )
        m_boundary[index] = otherBin.minmax(index);
    }
    bool isIn( const Event& point ){
     for( unsigned int i = 0 ; i != dim(); ++i)
       if( ! point.isIn(i,m_boundary[i] ) ) return false;
     return true;
     
    };

    bool add( const Event& point , bool debug=false){ 
      /// add if point is within phase space boundary ////
      if( isIn(point) ){
        m_data.push_back( point );
        return true;
      }
      else return false;
    }
    void clear(){ m_data.clear(); }
    double binVariance( const unsigned int& index , double nSubBins=5){
     DEBUG("Here I am in bin variance for index = " << index ); 
     std::vector<double> binPopulation(nSubBins,0);
      double binWidth  = ( max(index) - min(index) ) / nSubBins;
      for( auto& evt : m_data ){
        int pos = (int) ( ( evt.s( gChi2Indices[index] ) - min(index) ) / binWidth );
        if( pos == nSubBins ) pos--; /// move overflow events in by 1 
        if( pos >= nSubBins || pos < 0 ) 
          ERROR("Event outside of sub-sub-binning " 
            << evt.s( gChi2Indices[index] ) << " , " << min(index) << " " << max(index) );
        binPopulation[pos]++;
      };
      double x=0;
      double x2=0;
      for( unsigned int i = 0 ; i < nSubBins; ++i ){
        x+=binPopulation[i];
        x2+=binPopulation[i]*binPopulation[i];
      }
      x/=(double)nSubBins;
      DEBUG("Returning " << sqrt( x2 / nSubBins - x*x ) );
      return sqrt( x2 / nSubBins - x*x ) ;
    };

    std::pair<Bin,Bin> divide( unsigned int index, double pos ){
      std::pair<Bin,Bin> bins;
      std::sort( m_data.begin(), m_data.end(), sorter<Event>(index) );
      bins.first.copyBoundaries(*this);
      bins.second.copyBoundaries(*this);
      bins.first.setBoundary(index, min(index) , pos );
      bins.second.setBoundary(index, pos , max(index) );
      std::vector<Event>::iterator foo = std::upper_bound( m_data.begin(),
          m_data.end(), pos, less_than<Event>(index) );

      bins.first.addData( m_data.begin(), foo );
      bins.second.addData( foo, m_data.end() );
      return bins;
    }
    /// midpoint division ///
    std::pair<Bin,Bin> divide( const unsigned int& index ){
      std::pair<Bin,Bin> bins;
      std::sort( m_data.begin(), m_data.end(), sorter<Event>(index) );      
      bins.first.copyBoundaries(*this);
      bins.second.copyBoundaries(*this);
      if( m_data.size() < 20 ){
        ERROR("Splitting bin more than we should!");
      }
      unsigned int midpoint = m_data.size() / 2;
   
      double midposition = ( m_data[midpoint].s( gChi2Indices[index] ) +   m_data[midpoint].s( gChi2Indices[index] ) ) / 2.;
      bins.first.setBoundary(index, min(index) , midposition );
      bins.second.setBoundary(index, midposition , max(index) );
      bins.first.addData(m_data.begin(), m_data.begin() + midpoint );
      bins.second.addData( m_data.begin() + midpoint, m_data.end() );
      return bins;
    }

    void sort( const unsigned int& index ){
      std::sort( m_data.begin(), m_data.end(), sorter<Event>(index ));
    }

    std::vector<Bin> binSplit(const unsigned int& minPopulation=10)
    { /// n-dimensional division ///
      std::vector<unsigned int> dims;
      if( getN() / pow( 2, dim() ) < minPopulation ){
        /// the number of divisions is such that getN() / pow( 2, dim() 
        DEBUG("Splitting cleverly ...");
        int NDivisions = floor( log2( getN() / minPopulation ) );
        std::vector<std::pair<unsigned int, double>> directionVariances(dim()); 
          /// would be easier to use a map, but want to avoid collisions;
        for( unsigned int i = 0 ; i < dim(); ++i ){
          double var = binVariance(i);
          directionVariances[i] = std::pair<unsigned int, double>( i , var ); 
        }
        std::sort( directionVariances.begin(), directionVariances.end() , 
          []( const std::pair<unsigned int, double>& v1, const std::pair<unsigned int, double>& v2 ){ return v1.second > v2.second ; } );
        DEBUG( "Splitting along " << NDivisions); 
        for( int i = 0 ; i < NDivisions; ++i ){
          dims.push_back( directionVariances[i].first );
        }
      }
      else {
        dims.resize( dim() );
        std::iota( dims.begin(), dims.end() , 0 );
      }
      std::vector<Bin> splitBins;
      splitBins.push_back( *this );
      for( auto& d : dims){
        if( max(d) - min(d) < 50 ) continue; /// prevent overly narrow bins /// 
        std::vector<Bin> newBins;
        for( auto& bin : splitBins ){
          auto binPair = bin.divide(d );
          newBins.push_back( binPair.first);
          newBins.push_back( binPair.second);
        }
        splitBins = newBins;
      }
      return splitBins;
    }
    template <class BIN_FUNCTOR> std::pair<Bin,Bin>
      split1D( unsigned int index , BIN_FUNCTOR func, double& metric , unsigned int scanDivision=2){
        std::pair<Bin,Bin> bestBinning;
        metric = 999999999999;
        if( max(index) - min(index)  < 120. ){
          return bestBinning; 
        }
        sort(index);
        for( unsigned int n = 1; n != scanDivision; ++n){

          double mid_point =
            (max(index) - min(index )  )*n/scanDivision + min(index);

          auto bins = divide( index, mid_point );
          double tmp_metric = func( bins.first, bins.second );
          if( tmp_metric < metric && 
              bins.first.nEvents() > 10 && 
              bins.second.nEvents() > 10 ){
            metric = tmp_metric ;
            bestBinning = bins ;
          }
        }
        return bestBinning;
      }
    double getVal() const {
      double value=0;

      for( auto& event : m_data){
        // INFO( event.to_string() );
        value += event.weight();
      }
      return value; 
    }
    double getN() const { 
      return m_data.size();
    }
    double getW2() const {
      double value=0;
      for( auto& event : m_data) value += event.weight()*event.weight();
      return value;
    }
    double variance() const {
      double I = getVal() ; // /getN();
      return getN() != 0 ? getW2() - I*I /getN() : 0 ;
    }; 
    template <class BIN_FUNCTOR> void bestSplit(std::vector<Bin>& other, BIN_FUNCTOR func){
      double best_metric = 9999999 ; //func(this,1);
      int best_index = -1;
      std::pair<Bin,Bin> bins;
      for( unsigned int index = 0 ; index < dim() ; ++index ){
        double metric=0;
        auto binPair = split1D(index,func, metric );
        //std::cout << "Axis = " << index << " metric = " << metric << std::endl; 
        if( metric < best_metric){
          best_metric = metric;
          bins = binPair;
          best_index = index;
        }
      }
      if( best_index == -1 ){
        other.push_back( *this );
        return;
      }
      other.push_back( bins.first );
      other.push_back( bins.second );
    }
};

bool operator<( const double& number, const Bin& bin ) ;  // { return number < bin.min(0) ; }
bool operator>( const double& number, const Bin& bin ) ;  // { return bin.min(0) > number ; } 
bool operator>( const Bin& bin , const double& number) ;  // { return bin.min(0) > number ; }
bool operator<( const Bin& bin , const double& number) ;  // { return bin.min(0) < number ; }


double deltaN( const Bin& bin0, const Bin& bin1 ) ; // { return abs( bin0.nEvents() - bin1.nEvents() ); }

class Binning : public std::vector<Bin>{
  public:
    //Binning( iterator itBegin, iterator itEnd ) : std::vector<Bin<Event>>( itBegin, itEnd ) {} ;
    Binning() : std::vector<Bin>(0) {};
    /// deserializer ///
    EventList events(const EventType& type){
      EventList returnEvents( type );
      for( auto& bin : *this )
        for( auto event = bin.beginData() ; event != bin.endData(); ++ event ) 
          returnEvents.push_back( *event );

      return returnEvents;      
    };

    Binning( const std::string& line ){
      std::vector<std::string> tokens = split(line,'#');
      for( auto& token : tokens ) std::vector<Bin>::emplace_back( token ) ;
    }
    void add( const std::vector<Bin>& bins ){ for( auto& bin : bins ) std::vector<Bin>::push_back( bin); }
    void add( const Bin& bin ){ std::vector<Bin>::push_back( bin); }
    void print( std::ostream& stream = std::cout ){
      for( auto& bin : *this ) bin.print(stream);
    };

    double getN() const {
      double nEvt=0;
      for( auto& bin : *this ) nEvt += bin.getN();
      return nEvt;
    }
    double getVal() const {
      double nEvt=0;
      for( auto& bin : *this ) nEvt += bin.getVal();
      return nEvt;
    }
    double getW2() const {
      double nEvt=0;
      for( auto& bin : *this ) nEvt += bin.getW2();
      return nEvt;
    }
    void lowerBoundSort(const unsigned int& index){
      std::sort( std::vector<Bin>::begin(), std::vector<Bin>::end(), 
        [index]( const Bin& b1, const Bin& b2 ){ return b1.min(index) < b2.min(index) ; } );
    }; /// sort bins by lower bound 

    Binning( Bin& bin, const unsigned int& index, const double& pos ) : std::vector<Bin>(2) {
      bin.sort(index ); //// Divide bin into two bins along direction "index" at position pos
      std::vector<Bin>::at(0).copyBoundaries(bin);
      std::vector<Bin>::at(0).setBoundary(index, bin.min(index),pos);
      std::vector<Bin>::at(1).copyBoundaries(bin);
      std::vector<Bin>::at(1).setBoundary(index,pos, bin.max(index ));  

      std::vector<Event>::const_iterator foo = bin.Data().begin() 
    ; //std::upper_bound( bin.Data().begin(),
      //    bin.Data().end(), pos, less_than<Event>(index) );

      std::vector<Bin>::at(0).addData( bin.beginData(), foo );
      std::vector<Bin>::at(1).addData( foo, bin.endData() );
    }
    void clear(){
      for( auto& b : *this ) b.clear() ; 
    }
    bool add( const Event& event ){
      auto bin = std::vector<Bin>::begin();
      for( ; bin != std::vector<Bin>::end(); ++bin){
        if( bin->add( event )) return true;
      }
      return false;
    }
    bool isIn( const Event& event ){
      auto bin = std::vector<Bin>::begin();
      for( ; bin != std::vector<Bin>::end(); ++bin){
        if( bin->isIn( event )) return true;
      }
      return false;
    };
    std::vector<Event> add( const std::vector<Event>& events ){
      clear();
      std::vector<Event> overflow;
      unsigned int nEvents = 0 ; 
      for( auto event = events.cbegin(); event != events.cend(); ++event  ){

        if( (event - events.begin() ) % 5000 == 0 ){
          INFO("Added " << (event - events.begin()) ) ; // << " events" << std::endl; 
        }
        nEvents += add( *event ) ; 
//        if( !add( event) ) 
//          overflow.push_back( *event );
      }
      INFO( "Events added successfully = " << nEvents) ; //  << std::endl; 
      return overflow;
    }
};} 

#endif

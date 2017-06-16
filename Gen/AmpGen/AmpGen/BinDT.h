#ifndef BINDT_H
#define BINDT_H
#include "AmpGen/EventList.h"
#include <queue>
#include <iomanip>
#include <iostream>

namespace AmpGen { 

  class BinDT {
    private : 
      class INode {
        public :
          virtual unsigned int operator()(const Event& evt) = 0 ;
          virtual void serialize(std::ostream& stream) = 0;
          INode* m_parent;
          INode() : m_parent(nullptr) {};
      };

      class Decision : public INode {

        public :
          Decision( std::function<double(const Event&)>& functor,
              const unsigned int& index, 
              const double& value ,
              std::shared_ptr<INode> left,
              std::shared_ptr<INode> right ) :
            INode() ,
            m_left(left),
            m_right(right),
            m_functor(functor),
            m_index(index),
            m_value(value)
        {
          if( m_left != nullptr ) m_left->m_parent  = this;
          if( m_right != nullptr ) m_right->m_parent = this;
        }
          virtual unsigned int operator()( const Event& evt ){
            return m_functor( evt )  > m_value ? (*m_right)(evt) : (*m_left)(evt) ;
          }
          virtual void serialize( std::ostream& stream ){
            stream << this << " " << m_index <<  " " <<   m_value << " " << m_left.get() << " " << m_right.get() << std::endl;
            m_left->serialize(stream);
            m_right->serialize(stream);
          }
          void setChildren( std::shared_ptr<INode> l , std::shared_ptr<INode> r ){
            m_left = l;
            m_right = r;
            m_left->m_parent = this;
            m_right->m_parent = this;
          };

        private :
          std::shared_ptr<INode> m_left;
          std::shared_ptr<INode> m_right;
          std::function<double(const Event&)>& m_functor; 
          unsigned int m_index;
          double m_value;
      };

      class EndNode : public INode {
        public:
          EndNode( const unsigned int& no ) : m_binNumber(no) {}
          virtual unsigned int operator()( const Event& evt ){
            return m_binNumber;
          }
          virtual void serialize( std::ostream& stream ){
            stream << this << " " << m_binNumber << std::endl;
          }
          unsigned int binNumber() const { return m_binNumber ; } 
        private :
          unsigned int m_binNumber ;
      };

    public : 
      BinDT() {};
      BinDT( const std::vector<Event>& evts , const unsigned int& dim,
          const unsigned int& minEvents  ) ;
      BinDT( const std::vector<Event>& evts, const std::vector<std::function<double(const Event&)>> functors,
          const unsigned int& minEvents );  
      BinDT( std::istream& stream ) ;
      BinDT( const std::string& filename ) ; 

      double nnUniformity( std::vector<const Event*> evts, const unsigned int& index ) const;
      unsigned int getBinNumber( const Event& evt ) const ;
      unsigned int size() const ; 
      void readFromStream( std::istream& stream ) ; 
      void serialize( std::ofstream& output ) ;
      void serialize(const std::string& filename ) ; 
      std::vector<std::shared_ptr<EndNode>>::iterator begin(){ return m_endNodes.begin() ; }
      std::vector<std::shared_ptr<EndNode>>::iterator   end(){ return m_endNodes.end() ; }

      void makeDefaultFunctors();
      void refreshQueue( const std::vector<const Event*>& evts, std::queue<unsigned int>& indexQueue ){
        if( evts.size() > m_minEvents * pow( 2 , m_dim ) ){
          for( unsigned int i = 0 ; i < m_dim ;++i) indexQueue.push(i);
        } else {
          std::vector<std::pair<unsigned int, double >> indices; 
          for( unsigned int i=0;i<m_dim;++i) 
            indices.emplace_back(i,nnUniformity(evts,i) );
          std::sort( indices.begin(), indices.end(), []( 
                const std::pair<unsigned int, double >& it1,
                const std::pair<unsigned int, double >& it2 ){ return it1.second > it2.second ; } );
          for( auto& item : indices ) indexQueue.push( item.first );
        }
      }
      std::shared_ptr<INode> makeNodes( std::vector<const Event*> evts , 
          std::queue<unsigned int> indexQueue)
      {
        unsigned int index = indexQueue.front();
        if( evts.size() < 2 * m_minEvents ){ 
          DEBUG("Returning end node as " << evts.size() << " less than 2 x " << m_minEvents ); 
          auto node = std::make_shared<EndNode>( m_counter++);
          m_endNodes.push_back( node );
          return node;
        }

        std::sort( evts.begin(), evts.end(),
            [this,&index]( const Event* a, const Event* b ) 
            { return m_functors[index](*a) > m_functors[index](*b) ; } );

        unsigned int midpoint = evts.size() / 2  ;
        midpoint = evts.size() / 2 ;
        auto& co =  m_functors[index];

        double midposition = evts.size() % 2 == 0 ? 
          ( co( *evts[midpoint-1] ) + co( *evts[midpoint] ) ) / 2 : 
          ( co( *evts[midpoint+1] ) + co( *evts[midpoint-1] ) + co( *evts[midpoint] ) ) / 3; 

        midpoint += midposition > co( *evts[midpoint] );

        std::vector<const Event*> leftEvents( evts.begin(), evts.begin() + midpoint );
        std::vector<const Event*> rightEvents( evts.begin() + midpoint, evts.end() );
        indexQueue.pop();
        if( indexQueue.empty() ) refreshQueue( evts, indexQueue ); 
        DEBUG("Making decision node " << index << " " << midposition 
            << "  " << leftEvents.size() << "  " << rightEvents.size() );
        DEBUG( "left node = " << co( (*leftEvents.rbegin()) ) 
            << " right node =  " << co( (*rightEvents.begin()) ) ); 
        std::shared_ptr<INode> left = makeNodes( leftEvents, indexQueue );
        std::shared_ptr<INode> right = makeNodes( rightEvents, indexQueue );
        std::shared_ptr<INode> node = std::make_shared<Decision>( m_functors[index], index , midposition,right,left );
        return node;
      }
      std::shared_ptr<INode> m_top;
      unsigned int m_counter;
      unsigned int m_dim;
      std::vector<std::shared_ptr<EndNode>> m_endNodes;
      std::vector<std::function<double(const Event&)>> m_functors; 
      unsigned int m_minEvents;
  };
}
#endif 

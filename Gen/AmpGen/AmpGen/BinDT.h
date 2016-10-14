#ifndef BINDT 
#define BINDT 
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
          Decision( const unsigned int& index,
              const double& value ,
              std::shared_ptr<INode> left,
              std::shared_ptr<INode> right ) :
            INode() ,
            m_left(left),
            m_right(right),
            m_index(index),
            m_value(value)
        {
          if( m_left != nullptr ) m_left->m_parent  = this;
          if( m_right != nullptr ) m_right->m_parent = this;
        }
          virtual unsigned int operator()( const Event& evt ){
            return evt.s( gChi2Indices[ m_index]  ) > m_value ? (*m_right)(evt) : (*m_left)(evt) ;
          }
          virtual void serialize( std::ostream& stream ){
            stream << this << " " << m_index << " " <<   m_value << " " << m_left.get() << " " << m_right.get() << std::endl;
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
            [&index]( const Event* a, const Event* b ) 
            { return a->s(gChi2Indices[index]) > b->s(gChi2Indices[index]) ; } );

        unsigned int midpoint = evts.size() / 2  ;
        midpoint = evts.size() / 2 ;
        auto& co =  gChi2Indices[index];

        double midposition = evts.size() % 2 == 0 ? 
          ( evts[midpoint-1]->s(co) + evts[midpoint]->s(co) ) / 2 : 
          ( evts[midpoint+1]->s(co) + evts[midpoint-1]->s(co) + evts[midpoint]->s(co) ) / 3; 

        midpoint += midposition > evts[midpoint]->s(co);

        std::vector<const Event*> leftEvents( evts.begin(), evts.begin() + midpoint );
        std::vector<const Event*> rightEvents( evts.begin() + midpoint, evts.end() );
        indexQueue.pop();
        if( indexQueue.empty() ) refreshQueue( evts, indexQueue ); 
        DEBUG("Making decision node " << index << " " << midposition 
            << "  " << leftEvents.size() << "  " << rightEvents.size() );
        DEBUG( "left node = " << (*leftEvents.rbegin())->s( gChi2Indices[index] ) 
            << " right node =  " << (*rightEvents.begin())->s( gChi2Indices[index] )  );
        std::shared_ptr<INode> left = makeNodes( leftEvents, indexQueue );
        std::shared_ptr<INode> right = makeNodes( rightEvents, indexQueue );
        std::shared_ptr<INode> node = std::make_shared<Decision>( index , midposition,right,left );
        return node;
      }
      std::shared_ptr<INode> m_top;
      unsigned int m_counter;
      unsigned int m_dim;
      std::vector<std::shared_ptr<EndNode>> m_endNodes;
      unsigned int m_minEvents;
  };
}
#endif 

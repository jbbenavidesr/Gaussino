#include "AmpGen/BinDT.h"

using namespace AmpGen;

BinDT::BinDT( const std::vector<Event>& evts , const unsigned int& dim,
    const unsigned int& minEvents ){
  m_dim = dim;
  m_counter = 0 ;
  m_minEvents = minEvents;

  std::vector<const Event*> pEvts;
  for( unsigned int i = 0 ; i < evts.size(); ++i ) pEvts.push_back( &( evts[i] ) );
  std::queue<unsigned int> iq;
  refreshQueue( pEvts, iq );
  m_top = makeNodes( pEvts, iq );
}

unsigned int BinDT::getBinNumber( const Event& evt ) const { return (*m_top)(evt) ; }

unsigned int BinDT::size() const { return m_endNodes.size() ; } 

BinDT::BinDT( std::istream& stream ){
  readFromStream( stream );
}

BinDT::BinDT( const std::string& filename ){
  std::ifstream stream;
  stream.open( filename );
  readFromStream( stream );
  stream.close();
}

void BinDT::readFromStream( std::istream& stream ){
  std::map<std::string,std::pair<std::string,std::shared_ptr<INode>>> nodes ;
  std::string line;
  std::string topAddress = "";
  while( getline( stream, line ) ) {

    auto tokens = split( line, ' ' );

    std::string address = tokens[0];
    if( topAddress == "" ) topAddress = address;
    if( tokens.size() == 5 ){
      auto node = std::make_shared<Decision>( stoi(tokens[1]), stod(tokens[2]),nullptr,nullptr);
      nodes[address] = std::make_pair( line, node );
    }
    else {
      auto node = std::make_shared<EndNode>( stoi(tokens[1]) );
      nodes[address] = std::make_pair( line,  node );
      m_endNodes.push_back( node );
    }
  }
  m_top = nodes[ topAddress ].second;
  for( auto& node : nodes ){
    Decision* node_ptr = dynamic_cast< Decision* >( node.second.second.get() );
    if( node_ptr != 0 ){
      auto tokens = split( node.second.first , ' ' );
      node_ptr->setChildren( nodes[  tokens[3]  ].second , nodes[ tokens[4]  ].second );
    }
  }
}

void BinDT::serialize( std::ofstream& output ){
  output << std::setprecision( 17 );
  m_top->serialize(output);
}

void BinDT::serialize(const std::string& filename ){
  std::ofstream output;
  serialize( output );
  m_top->serialize(output);
  output.close();
}

double BinDT::nnUniformity( std::vector<const Event*> evts, const unsigned int& index ) const {

  auto s = [&index]( const Event* evt ){ return evt->s( gChi2Indices[index] );} ; 
  std::sort( evts.begin(), evts.end(), [=](
       const Event* evt1, const Event* evt2 ) { return s( evt1 ) > s(evt2) ; } ) ;
  double averageNNdistance = 0 ;
  int size = evts.size();
  for( int i = 0 ; i < size; ++i ){
    int j=i-1;
    int k=i+1;

    if( i==0 ){
      averageNNdistance += fabs( s( evts[i] ) - s( evts[k] ) );
      continue;
    }
    if( k==size ){
      averageNNdistance += fabs( s( evts[i] )  - s( evts[j] ) );
      continue;
    }
    else {
      double low = fabs( s ( evts[i] )  - s( evts[k] )  );
      double high = fabs( s( evts[i] ) - s( evts[j] ) );
      averageNNdistance += low > high ? high : low;
    }
  }
  double avg = averageNNdistance / double( size ) ;

  double sigma=0;
  for( int i = 0 ; i < size; ++i ){
    int j=i-1;
    int k=i+1;
    if( i==0 ){
      sigma += pow( fabs( s( evts[i] ) - s ( evts[k] )  ) -avg, 2 );
      continue;
    }
    if( k==size ){
      sigma += pow( fabs( s ( evts[i] ) - s( evts[j] ) ) -avg , 2 );
      continue;
    }
    else {
      double low =  fabs( s( evts[i] ) - s( evts[k] ) );
      double high = fabs( s( evts[i] ) - s( evts[j] ) );
      sigma += fabs(low) > fabs(high) ? pow( high-avg,2) : pow(low-avg,2);
    }
  }
  return sigma / double(evts.size());
}


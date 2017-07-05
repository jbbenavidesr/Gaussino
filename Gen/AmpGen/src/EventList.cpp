#include <random>

#include "AmpGen/EventList.h"
#include "TTree.h"
#include "TFile.h"

using namespace AmpGen ;

EventList::EventList( const std::string& fname,
    const EventType& evtType,
    const unsigned int& pdfSize,
    std::function<bool(const Event&)> cut ) :   
  EventList( (TTree*)TFile::Open( fname.c_str(), "READ")->Get("DalitzEventList") ,
      evtType,
      pdfSize,
      cut ) {} 

  EventList::EventList( const EventType& type ) : 
    m_eventType(type), 
    m_extendedEventData( m_eventType.getEventFormat() ) { 
    }

EventList::EventList(TTree* tree,
    const EventType& particles ,
    const unsigned int& pdfsize,
    std::function<bool(const Event&)> cut ) : EventList( particles) {
  auto t_start = std::chrono::high_resolution_clock::now();

  INFO("Building eventlist with " << tree->GetEntries() );
  Event temp(4*particles.size(), pdfsize);
  std::vector<Event>::reserve( tree->GetEntries() );
  auto finalStatesMintStyle = particles.getPickledFinalStates();
  for( unsigned int ip=0;ip<particles.size();++ip ){
    std::string prefix="_"+std::to_string(ip+1)+"_"+finalStatesMintStyle[ip];
    DEBUG("Adding branches for " << prefix );
    tree->SetBranchAddress((prefix+"_Px").c_str() ,temp.address(4*ip+0) );
    tree->SetBranchAddress((prefix+"_Py").c_str() ,temp.address(4*ip+1) );
    tree->SetBranchAddress((prefix+"_Pz").c_str() ,temp.address(4*ip+2) );
    tree->SetBranchAddress((prefix+"_E" ).c_str() ,temp.address(4*ip+3) );
  }
  tree->SetBranchAddress("genPdf", temp.pGenPdf());
  tree->SetBranchAddress("weight", temp.pWeight());
  std::vector<Event>::reserve( tree->GetEntries());
  for( unsigned int i=0;i<tree->GetEntries(); ++i ){
    tree->GetEntry(i);
    if( temp.isNaN() ){
      ERROR("Event is " << i << " is NaN - skipping");
      continue;
    };
    if( temp.genPdf() == 0 ) *(temp.pGenPdf()) = 1;
    if( cut( temp) ) std::vector<Event>::push_back(temp);
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  double t_taken = std::chrono::duration<double, std::milli>(t_end-t_start).count() ;
  INFO("EventList.size() = " << std::vector<Event>::size() << " time taken = " << t_taken << "ms");
}

EventList::EventList( TTree* tree, 
    const std::vector<std::string>& branches, 
    const EventType& evtType, 
    const unsigned int& opt,  
    const std::vector<unsigned int>& eventList ) : m_eventType(evtType){
  Event temp( branches.size(), 0 );
  temp.setWeight(1);
  temp.setGenPdf(1);
  tree->SetBranchStatus( "*", 0 );
  bool getGenPdf = opt & OPTIONS::GETPDF;
  bool applyParity = opt & OPTIONS::PARITY;

  for( auto branch = branches.begin() ; branch != branches.end(); ++branch ){
    unsigned int pos = std::distance( branches.begin() , branch );
    m_extendedEventData[ *branch ] = pos;
    tree->SetBranchStatus( branch->c_str(), 1 );
    tree->SetBranchAddress( branch->c_str(), temp.address(pos));     
  }
  double genPdf;
  if( getGenPdf ){
    tree->SetBranchStatus("genPdf",1);
    tree->SetBranchAddress( "genPdf",&genPdf );
  }
  int motherID=1; 
  if( applyParity ){
    tree->SetBranchStatus( (evtType.mother() + "_ID").c_str(), 1 );
    tree->SetBranchAddress(  (evtType.mother() + "_ID").c_str() , &motherID );
  }
  std::mt19937 rng(12);    // random-number engine used (Mersenne-Twister in this case)

  auto shuffles = evtType.getBosePairs();

  bool hasEventList = eventList.size() != 0 ;
  unsigned int nEvents = hasEventList ? eventList.size() : tree->GetEntries() ;
  std::vector<Event>::reserve( nEvents );

  for( unsigned int evt = 0 ; evt < nEvents ; ++evt ){
    if( evt % 10000 == 0 ) INFO("Read " << evt << " events");

    if( hasEventList && eventList[evt] > tree->GetEntries() ){
      ERROR("Trying to read out of bounds : " << eventList[evt]);
    };
    tree->GetEntry( hasEventList ? eventList[evt] : evt );

    for( auto shuffled : shuffles ){
      for( unsigned int index = 0 ; index < shuffled.size() ; ++index ){
        unsigned int j = std::uniform_int_distribution<int>(0,index)(rng);
        if( index == j ) continue;
        std::swap( shuffled[index], shuffled[j] );
        temp.swap(shuffled[index],shuffled[j]);
      }
    }
    if( getGenPdf ) temp.setGenPdf( genPdf );  
    if( motherID < 0 && applyParity ) temp.invertParity();
    std::vector<Event>::push_back( temp );
  } 
}


TTree* EventList::tree(const std::string& name ){
  TTree* outputTree = new TTree(name.c_str() , name.c_str() );
  if( std::vector<Event>::size() == 0 ){
    ERROR("Trying to output empty tree");
    return 0;
  }
  Event tmp = *(std::vector<Event>::begin());
  double genPdf(1), weight(1);
  for( unsigned int ip=0;ip < m_eventType.size(); ++ip ){
    std::string name = "_"+std::to_string(ip+1)+"_"+m_eventType.getPickledFinalStates()[ip];
    outputTree->Branch( (name+"_E").c_str(), tmp.address(4*ip+3) );
    outputTree->Branch( (name+"_Px").c_str(),tmp.address(4*ip+0)  );
    outputTree->Branch( (name+"_Py").c_str(),tmp.address(4*ip+1) );
    outputTree->Branch( (name+"_Pz").c_str(),tmp.address(4*ip+2) );
  }
  outputTree->Branch( "genPdf",&genPdf );
  outputTree->Branch( "weight", &weight);
  for( auto& evt : *this ){
    tmp = evt ;
    genPdf = evt.genPdf();
    weight = evt.weight();
    outputTree->Fill();
  }
  return outputTree; 
}    

std::vector<TH1D*> EventList::makePlots(const std::string& prefix, 
    const unsigned int& nBins, 
    const unsigned int& category){
  std::vector<TH1D*> plots;
  auto axes = defaultProjections();
  TH1D* weights = new TH1D( (prefix+"_weights").c_str(),"",1000,0.,20.);
  for( auto evt = std::vector<Event>::begin(); evt != std::vector<Event>::end(); ++evt )
    weights->Fill( evt->weight(category) );
  plots.push_back( weights );
  for( auto& proj : axes ){
    TH1D* plot = new TH1D( (prefix+proj.name ).c_str() , "", nBins, proj.min, proj.max ) ;
    plot->GetXaxis()->SetTitle( proj.title.c_str() );
    char buffer[100];
    double binWidth = (proj.max-proj.min)/(double)nBins;
    sprintf(buffer," Events / (%0.2f GeV^{2}/c^{4})", binWidth);

    plot->GetYaxis()->SetTitle( buffer );
    for( auto evt = std::vector<Event>::begin(); evt != std::vector<Event>::end(); ++evt ){ 
      DEBUG( proj.title << "     " << evt->s( proj.indices ) / (1000.*1000.) << "    "  << evt->weight(category) );
      plot->Fill( evt->s( proj.indices ) / (1000.*1000.) , evt->weight(category) );
    }
    plots.push_back(plot);
  }
  return plots;
}

std::vector<TH2D*> EventList::makePlots2D( const std::string& prefix, const unsigned int& category, const unsigned int& nBins){
  std::vector<TH2D*> plots;
  std::vector<std::vector<unsigned int>> permutations;
  for( unsigned int r=2;r<m_eventType.size();++r){ /// loop over sizes ///
    auto combR = nCr(m_eventType.size(),r);
    for( auto& indices : combR ) permutations.push_back( indices );
  }
  for( auto x = permutations.begin(); x!= permutations.end() -1; ++x ){
    for( auto y = x+1; y != permutations.end(); ++y ){
      std::string stringified = "s"+vectorToString( *x ) + "_vs_s" + vectorToString(*y);
      auto xmax = minmax( *x );
      auto ymax = minmax( *y );
      TH2D* plot = new TH2D( (prefix + "_"+stringified).c_str(), "",nBins, xmax.first, xmax.second, nBins, ymax.first, ymax.second );
      plot->GetXaxis()->SetTitle(("s("+particleNamesFromIndices( *x ) + ") [GeVc^{-2}]^{2}").c_str());
      plot->GetYaxis()->SetTitle(("s("+particleNamesFromIndices( *y ) + ") [GeVc^{-2}]^{2}").c_str());
      for( auto evt = std::vector<Event>::begin(); evt != std::vector<Event>::end(); ++evt ){
        plot->Fill( evt->s( *x ) / (1000.*1000.) , 
            evt->s( *y )/ (1000.*1000.), 
            evt->weight(category) );
      }
      plots.push_back(plot);
    }
  }
  return plots;
}

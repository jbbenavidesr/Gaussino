#include "AmpGen/Particle.h"
#include <map>
#include <algorithm>
#include <fstream>

struct FitQuality {
  double chi2;
  double LL;
  double dof;
  FitQuality( const double& m_chi2, const double& m_LL, const double& m_dof ) : chi2(m_chi2), LL( m_LL ) , dof( m_dof ) {}
  void set ( const double& m_chi2, const double& m_LL, const double& m_dof ) { chi2 = (m_chi2) ;  LL = ( m_LL ) ;  dof = ( m_dof ) ; } 
};

namespace AmpGen { 
  class LatexTable { 


    std::vector<std::string> m_ordering ;
    std::string fname;
    public :
    LatexTable( const std::string& filename ){
      m_ordering = { "VV","VS", "PPK-pi+pi-","PPK+pi+pi-", "APK-pi+pi-", "APK+pi+pi-", "VK-pi+pi-", "VK+pi+pi-" , "TPK+pi+pi-", "TPK-pi+pi-","APpi+pi+pi-", "APpi+pi-pi-","PPPP" };\
                   fname = filename; 
    };
    void makeTable( const std::vector<std::shared_ptr<Particle>>& particles, const std::vector<std::string>& ff, const FitQuality& fq, bool incoherent = false ){
      std::ofstream model; model.open(fname);

      INFO("Making LaTeX table with " << particles.size() << " , " << ff.size() << " entries" );
      std::map<std::string, std::vector<std::pair< std::shared_ptr<Particle>, unsigned int>>> topos; /// STL MADNESS
      for( unsigned int i = 0 ; i < particles.size(); ++i ){
        auto particle = particles[i];
        topos[ particle->topologicalString() ].push_back( std::make_pair(particle,i ) );
      };

      for( auto& topologies :topos ) {
        if( topologies.first == "VV" ){
          std::sort( topologies.second.begin(), topologies.second.end() , [](auto& g1, auto& g2){
              double m1 = g1.first->daughter(0)->mass() + g1.first->daughter(1)->mass();
              double m2 = g2.first->daughter(0)->mass() + g2.first->daughter(1)->mass();
              double l1 = g1.first->orbital();
              double l2 = g2.first->orbital();
              return m1 == m2 ? l1 < l2 : m1 < m2 ; } );
        };
        std::string spinBit = topologies.first.substr( 0, 2 );
        if( spinBit == "AP" || spinBit == "TP" ){
          std::sort( topologies.second.begin(), topologies.second.end() , [](auto& g1, auto& g2){
              double m1 = g1.first->daughter(0)->mass();
              double m2 = g2.first->daughter(0)->mass();
              double l1 = g1.first->daughter(0)->orbital();
              double l2 = g2.first->daughter(0)->orbital();
              return m1 == m2 ? l1 < l2 : m1 < m2 ; 
              } );
        };
      }
      std::string endl = "\\\\\n";
      std::string rule = "\\midrule\n";
      std::string div = " & ";
      model << (incoherent ? "\\begin{tabular}{l l l}" : "\\begin{tabular}{l l l l}") << std::endl; 
      model << "\\toprule" << std::endl; 
      std::string params = "$\\mathcal{R}(g)$ & $\\mathcal{I}(g)$ ";
      if( incoherent ) params = "$g$";
      model << "\\textbf{State} & \\textbf{Fit Fraction} & " + params << endl; 
      model << rule;
      model << rule; 
      for( auto& content : m_ordering ){
        //   INFO( content );
        auto it = topos.find(content);
        std::string description = content == "PPPP" ? " (non-resonant) " : "";
        if( it != topos.end() ){
          auto container = it->second ;
          for( auto& f : container ) 
            model << "$" << f.first->getTex() << "$" << description << div << ff[ f.second] << endl;
          if( ! incoherent ) model << rule ;
        }
      };
      std::string delim = incoherent ? "" : div;
      model << delim << " $\\chi^2/ $ dof & dof & $\\mathcal{F}$" << endl;
      model << delim << fq.chi2 << div << fq.dof << div << fq.LL << endl; 
      model << "\\bottomrule" << std::endl; 
      model << "\\end{tabular}" << std::endl;
      model.close();
    };



  };
}

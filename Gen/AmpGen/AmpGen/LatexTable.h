#ifndef LATEXTABLE_H
#define LATEXTABLE_H
#include "AmpGen/Particle.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/FitResult.h"
#include <map>
#include <algorithm>
#include <fstream>
/*
namespace AmpGen {

  class LatexTable { 

    std::vector<std::string> m_ordering ;
    std::string fname;
    public :
    LatexTable( const std::string& filename ){
      m_ordering = { "VV","VS", "PPK-pi+pi-","PPK+pi+pi-", "APK-pi+pi-", "APK+pi+pi-", "VK-pi+pi-", "VK+pi+pi-" , "TPK+pi+pi-", "TPK-pi+pi-","APpi+pi+pi-", "APpi+pi-pi-","PPPP" };\
                   fname = filename; 
    };
    void makeTable( const std::vector<ProcessParameters>& processes, const FitQuality& fq, bool incoherent = false ){
      std::ofstream model; 
      model.open(fname);
      std::map<std::string, std::vector<const ProcessParameters*>> topos;
      for( auto& process : processes ){
        topos[ process.particle()->topologicalString() ].push_back( &process );
      };
      for( auto& topologies :topos ) {
        if( topologies.first == "VV" ){
          std::sort( topologies.second.begin(), topologies.second.end() , []( const ProcessParameters* g1, const ProcessParameters* g2){
              double m1 = g1->particle()->daughter(0)->mass() + g1->particle()->daughter(1)->mass();
              double m2 = g2->particle()->daughter(0)->mass() + g2->particle()->daughter(1)->mass();
              double l1 = g1->particle()->orbital();
              double l2 = g2->particle()->orbital();
              return m1 == m2 ? l1 < l2 : m1 < m2 ; } );
        };
        std::string spinBit = topologies.first.substr( 0, 2 );
        if( spinBit == "AP" || spinBit == "TP" ){
          std::sort( topologies.second.begin(), topologies.second.end() , []( const ProcessParameters* g1, const ProcessParameters* g2){
              double m1 = g1->particle()->daughter(0)->mass();
              double m2 = g2->particle()->daughter(0)->mass();
              double l1 = g1->particle()->daughter(0)->orbital();
              double l2 = g2->particle()->daughter(0)->orbital();
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
        auto it = topos.find(content);
        std::string description = content == "PPPP" ? " (non-resonant) " : "";
        if( it != topos.end() ){
          auto container = it->second ;
          for( auto& f : container ) 
            model << f->toTeX(description) << endl;
          if( ! incoherent ) model << rule ;
        }
      }
      std::string delim = incoherent ? "" : div;
      model << delim << " $\\chi^2/ $ dof & dof & $\\mathcal{F}$" << endl;
      model << delim << fq.chi2/fq.dof() << div << fq.dof() << div << fq.LL << endl; 
      model << "\\bottomrule" << std::endl; 
      model << "\\end{tabular}" << std::endl;
      model.close();
    };



  };
}
*/
//std::ostream& operator<<(std::ostream& os, const AmpGen::ProcessParameters& p){
//    return os << p.frac() << " " << p.frac_err() << " " << p.re() << " " << p.re_err() << " " << p.im() << " " << p.im_err(); 
//}
#endif

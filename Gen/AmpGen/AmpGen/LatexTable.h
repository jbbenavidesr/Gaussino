#ifndef LATEXTABLE_H
#define LATEXTABLE_H
#include "AmpGen/Particle.h"
#include "AmpGen/Utilities.h"
#include <map>
#include <algorithm>
#include <fstream>

namespace AmpGen {

  struct FitQuality {
    double chi2;
    double LL;
    double dof;
    FitQuality( const double& m_chi2, const double& m_LL, const double& m_dof ) : chi2(m_chi2), LL( m_LL ) , dof( m_dof ) {}
    void set ( const double& m_chi2, const double& m_LL, const double& m_dof ) { chi2 = (m_chi2) ;  LL = ( m_LL ) ;  dof = ( m_dof ) ; } 
  };


  class ProcessParameters
  {

    std::complex<double> m_amp;
    std::complex<double> m_err;

    double m_frac,
           m_frac_err;
    std::shared_ptr<AmpGen::Particle> m_particle; 
    public: 

    void setParticle( const std::shared_ptr<AmpGen::Particle>& particle ){
      m_particle = particle; 
    }
    std::shared_ptr<AmpGen::Particle> particle() const { return m_particle ; } 
    ProcessParameters() : 
      m_amp(std::complex<double>(0,0)),
      m_err(std::complex<double>(0,0)),
      m_frac(0),
      m_frac_err(0) {};

    ProcessParameters( const std::string& params ){
      auto tokens = split( params, ' ' );
      if( tokens.size() == 1 ){
        m_frac = 0;
        m_frac_err = 0;
        m_amp = std::complex<double>(0,0);
        m_err = std::complex<double>(0,0);
      } else {
        if( tokens.size() != 8 ){
          ERROR("Not enough tokens in " << params );
        }  
        m_frac = stod( tokens[2] ) ;
        m_frac_err = stod( tokens[3] );
        m_amp = std::complex<double>( stod(tokens[4]), stod(tokens[6] ) );
        m_err = std::complex<double>( stod(tokens[5]), stod(tokens[7] ) );
      }
    }
    void setAmplitude( const std::complex<double>& amp, const std::complex<double>& err){
      m_amp = amp;
      m_err = err;
    }

    void setAmplitude( double re, double im, double re_err=0, double im_err=0){
      setAmplitude( std::complex<double>( re,im ),
          std::complex<double>( re_err, im_err) );
    }

    double re() const { return m_amp.real(); } 
    double im() const { return m_amp.imag(); } 
    double re_err() const { return m_err.real(); } 
    double im_err() const { return m_err.imag(); } 
    std::string label() const { return m_particle->getTeX() ; }
    std::string name() const { return m_particle->uniqueString() ; } 
    double frac() const { return m_frac;}
    double frac_err() const { return m_frac_err;}
    std::complex<double> amp() const { return m_amp; }
    void setFraction( double frac, double frac_stat ){
      m_frac = frac;
      m_frac_err = frac_stat;
    }
    void operator*=(const std::complex<double>& other){
      m_amp*=other;
    }
    void operator+=(const ProcessParameters& other){
      m_amp += other.amp();
    }
    std::string toTeX(const std::string& description ) const {
      return "$" + label() + "$"+description +"& $"+numberWithError(frac(),frac_err(),4)+"$ & $"+numberWithError(re(),re_err(),4)+"$ & $"+numberWithError(im(),im_err(),4)+"$";
    }
  };

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
      model << delim << fq.chi2 << div << fq.dof << div << fq.LL << endl; 
      model << "\\bottomrule" << std::endl; 
      model << "\\end{tabular}" << std::endl;
      model.close();
    };



  };
}

//std::ostream& operator<<(std::ostream& os, const AmpGen::ProcessParameters& p){
//    return os << p.frac() << " " << p.frac_err() << " " << p.re() << " " << p.re_err() << " " << p.im() << " " << p.im_err(); 
//}
#endif

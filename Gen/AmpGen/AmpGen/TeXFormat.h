#ifndef TEXFORMAT_H
#define TEXFORMAT_H 1 
#include <string>
#include <map>

static std::string mappedReplacement( const std::vector< std::pair<std::string, std::string> >& conversionTable, const std::string& input , bool checkAll=false){

  std::string output = input;
  for( auto& im : conversionTable ){

    size_t  start_pos = output.find( im.first );
    if( start_pos  != std::string::npos ){
      output.replace( start_pos, im.first.length(), im.second );
      if( !checkAll) return output;
    }
  }
  return output;
}

static std::string getTexFromPDG( const std::string& input , const bool& isRoot=false ){

  std::vector<std::pair<std::string,std::string>> spins;
  for( unsigned int i = 0 ; i < 6; ++i ) 
    spins.emplace_back( "(" + std::to_string(i) +")", "_{" + std::to_string(i) +"}");

  std::vector<std::pair<std::string,std::string>> theGreeks;
  if( !isRoot ){
    theGreeks.emplace_back("rho","\\rho");
    theGreeks.emplace_back("pi","\\pi");
    theGreeks.emplace_back("kappa","\\kappa");
    theGreeks.emplace_back("Delta","\\Delta");
    theGreeks.emplace_back("Sigma","\\Sigma");
    theGreeks.emplace_back("Xi","\\Xi");
    theGreeks.emplace_back("Psi","\\Psi");
    theGreeks.emplace_back("Omega","\\Omega");
    theGreeks.emplace_back("Upsilon","\\Upsilon");
    theGreeks.emplace_back("eta","\\eta");
    theGreeks.emplace_back("K","\\mathrm{K}");
    theGreeks.emplace_back("a","\\mathrm{a}");
    theGreeks.emplace_back("f","\\mathrm{f}");
  } else {
    theGreeks.emplace_back("rho","#rho");
    theGreeks.emplace_back("pi","#pi");
    theGreeks.emplace_back("kappa","#kappa");
    theGreeks.emplace_back("Delta","#Delta");
    theGreeks.emplace_back("Sigma","#Sigma");
    theGreeks.emplace_back("Xi","#Xi");
    theGreeks.emplace_back("Psi","#Psi");
    theGreeks.emplace_back("Omega","#Omega");
    theGreeks.emplace_back("Upsilon","#Upsilon");
    theGreeks.emplace_back("eta","#eta");
    theGreeks.emplace_back("K","K");
    theGreeks.emplace_back("a","a");
    theGreeks.emplace_back("f","f");
  }


  std::vector<std::pair<std::string,std::string>> signs;
  signs.emplace_back("*","^{*}");
  signs.emplace_back("+","^{+}");
  signs.emplace_back("-","^{-}");
  signs.emplace_back("bar0","^{0}");
  signs.emplace_back("bar","");
  signs.emplace_back(")0",")");

  std::vector<std::pair<std::string,std::string>> AmpGenMarkers;
  AmpGenMarkers.emplace_back("#","-");
  AmpGenMarkers.emplace_back("~","+");

  std::vector<std::pair<std::string,std::string>> nonResonantStates;
  nonResonantStates.emplace_back("NonResS0","0^{+}");
  nonResonantStates.emplace_back("NonResP0","0^{-}");
  nonResonantStates.emplace_back("NonResV0","1^{-}");
  nonResonantStates.emplace_back("NonResA0","1^{+}");
  nonResonantStates.emplace_back("NonResSbar0","0^{+}");
  nonResonantStates.emplace_back("NonResPbar0","0^{-}");
  nonResonantStates.emplace_back("NonResVbar0","1^{-}");
  nonResonantStates.emplace_back("NonResAbar0","1^{+}");

  std::string replaced = 
             mappedReplacement( AmpGenMarkers, input );
  replaced = mappedReplacement( nonResonantStates, replaced );

  replaced = mappedReplacement( spins, replaced );
  replaced = mappedReplacement( theGreeks, replaced );  
  replaced = mappedReplacement( signs, replaced , true);  

  size_t bar= replaced.find("bar0");
  if( bar != std::string::npos && ! isRoot ){
    replaced.resize( replaced.size() -4 );
    size_t pos = replaced.find("(");
    if( pos == std::string::npos ){
      replaced.resize( replaced.size() + 1 );
      pos = replaced.size();
    }
    std::string blargh = replaced.insert( replaced.find("("),"}");
    return "\\xoverline[0.75]{"+blargh;  
  }
  return replaced;
}

#endif

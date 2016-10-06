#include "AmpGen/FastCoherentSum.h"

using namespace AmpGen; 

FastCoherentSum::FastCoherentSum( const EventType& type , 
    AmpGen::MinuitParameterSet& mps,
    const std::map<std::string, unsigned int>& extendEventFormat, 
    const std::string& prefix,
    unsigned int options  ) : 

  m_events(0),
  m_sim(0), 
  m_weight(1),
  m_prepareCalls(0), 
  m_lastPrint(0), 
  m_prefix(prefix),
  m_stateIsGood(true) {
    bool dbThis = options & OPTIONS::DEBUG;
    bool CPConjugate = options & OPTIONS::CPCONJUGATE;
    bool FlavConjugate = options & OPTIONS::FLAVCONJUGATE;

    std::map<std::string, std::pair<AmpGen::MinuitParameter*,AmpGen::MinuitParameter*>> tmpParams;
    std::map<std::string, AmpGen::MinuitParameter*> otherParameters; 

    for( unsigned int i =0 ;i < mps.size(); ++i ){
      AmpGen::MinuitParameter* parameter = mps.getParPtr(i);
      const std::string paramName = parameter->name();
      auto tokens = split( paramName, '_');
      std::string thisDecayName;
      std::string thisPrefix;
      std::string thisSuffix;
      if( tokens.size() == 3 ){ 
        thisPrefix = tokens[0];
        thisDecayName = tokens[1];    
        thisSuffix = tokens[2];
      }
      else if( tokens.size() == 2 ){
        thisDecayName = tokens[0];
        thisSuffix = tokens[1];
        thisPrefix = ""; /// assume no prefix ///
      }
      if( prefix == thisPrefix && ( thisSuffix == "Re" || thisSuffix == "Im" )){
        std::map<std::string, 
          std::pair<AmpGen::MinuitParameter*,
          AmpGen::MinuitParameter*>>::iterator ip = tmpParams.find( thisDecayName );

        if( ip == tmpParams.end() ){
          tmpParams[ thisDecayName ] = 
            std::pair<AmpGen::MinuitParameter*,AmpGen::MinuitParameter*>( 0 , 0 );

          DEBUG( "Adding: " << prefix << "   " << thisDecayName << "   " << thisSuffix );
          ip = tmpParams.find( thisDecayName );
        }
        if( thisSuffix == "Re" ) ip->second.first = parameter;
        else if( thisSuffix == "Im" ) ip->second.second = parameter; 
      }
      else {
        DEBUG("Another parameter = " << paramName );
        otherParameters[ paramName ] = parameter; 
      }
    }
    m_pdfs.reserve( tmpParams.size());
    m_decayTrees.reserve( tmpParams.size());
    for( auto& p : tmpParams ){
      std::vector<std::string> finalStates = type.finalStates();
      std::shared_ptr<Particle> decayTree = 
        std::make_shared<Particle>(p.first, finalStates );
      
      if( ! decayTree->isStateGood() ){
        ERROR("Decay tree not configured correctly");
        m_stateIsGood = false;
        return; 
      }
      if(CPConjugate){
        decayTree->CPConjugateThis();
        std::shared_ptr<Particle> decayTreeCP = 
          std::make_shared<Particle>(decayTree->uniqueString(),finalStates );
        m_decayTrees.push_back( decayTreeCP );
        INFO("Adding tree = " << decayTreeCP->uniqueString() );
      }
      else {
        m_decayTrees.push_back( decayTree );
        INFO("Adding tree = " << p.first );
      }

      DEBUG("Configuring external parameters");

      std::vector<DBSYMBOL> dbExpressions;
      if( FlavConjugate ) (*m_decayTrees.rbegin())->setConj( true );
      const std::string name = (*m_decayTrees.rbegin())->uniqueString();
      const Expression expression = 
        (*m_decayTrees.rbegin())->getExpression(dbThis?&dbExpressions:NULL);    
      DEBUG("Got expression for this tree");
      m_pdfs.emplace_back( expression , name , extendEventFormat, dbThis?&dbExpressions:NULL );
      auto& pdf = *( m_pdfs.rbegin() );
      

      /*
       *   This code needs to be refactored in such a way that one can flexibly add transfer logic
       *   and parameter resolution. Having an abstract "handler" that is mapped from the different 
       *   types has some appeal... 
       */

      std::map<std::string, 
        std::pair< unsigned int, double> > parameterNames = pdf.getAddressMapping();
      std::map<std::string, 
        std::pair< unsigned int, double> > splineParameters; 

      for( auto& param : parameterNames ){ /// all parameters for this PDF, including fixed ones ///  
        if( param.first.find("Spline") != std::string::npos ){
          ///INFO("Identified Spline Parameter = " << param.first );
          splineParameters[param.first] = param.second;
          continue; 
        };
        DEBUG( "Mapping parameter " << param.first << "  " << param.second.first ); 
        //       for( auto& param : otherParameters ) 
        auto it = otherParameters.find(param.first ); 

        if( it != otherParameters.end() ){
          DEBUG("Setting value of " << param.first << " from options file = " << (*it).second->mean() );
          m_addressMapping.push_back( 
              std::make_shared<CacheTransfer>( 
                (*it).second , /// MinuitParameter*
                &m_pdfs[ m_pdfs.size() -1 ] , //// CompiledExpression*
                param.second.first //// destination address 
                ) ) ; 
          (*m_addressMapping.rbegin())->transfer();
          continue;
        }
        else {
          DEBUG("Setting " << param.first << " to default value");
        };
        DEBUG("Parameter " << param.first << " not found in options file - checking for a default setting " );

        auto tokens = split( param.first , '_' );
        if( tokens.size() == 2 ){
          const ParticleProperties* props = ParticlePropertiesList::getMe()->get(tokens[0]) ;
          if( props != 0 ){
            if( tokens[1] == "mass" ){ 
              pdf.setExternal( props->mass(), param.second.first );
              DEBUG("Setting mass of " << tokens[0] << " from pdg = " << props->mass() << " memory address =" << param.second.first );
            }
            else if( tokens[1] == "width" ){
              pdf.setExternal( props->width(), param.second.first );
              DEBUG("Setting width of " << tokens[0] << " from pdg = " << props->width() << " memory address = " << param.second.first );
            }
            else if( tokens[1] == "radius" ){
              pdf.setExternal( props->radius(), param.second.first );                                                                                                                            //        "mass"
              DEBUG("Setting radius for " << tokens[0] << " from pdg = " << props->radius() << " memory address = " << param.second.first );
            }
          }
          else pdf.setExternal( param.second.second, param.second.first );
        }
        else {
          pdf.setExternal( param.second.second, param.second.first );
        }
      }
      if( splineParameters.size() != 0 ){
        /// this is if we want to go mental and have 2D splines ////
        std::map< std::string, std::shared_ptr<SplineTransfer> > paramMap;
        for( auto param : splineParameters ){

          auto tokens = split( param.first , ':' );
          const std::string particleName = tokens[0];
          const std::string splineName = tokens[0] + "::"+tokens[1]+"::"+tokens[2];
          DEBUG("Spline parameter for " << param.first << " configuring");
          auto 
            thisSpline = paramMap.find(splineName);
          if( thisSpline == paramMap.end() ){
            double min = 
              AmpGen::NamedParameter<double>(particleName+"::Spline::Min",0.).getVal();
            double max = 
              AmpGen::NamedParameter<double>(particleName+"::Spline::Max",1800*1800).getVal();
            unsigned int nBins = 
              AmpGen::NamedParameter<unsigned int>(particleName+"::Spline::N",10).getVal();
            paramMap[splineName] = 
              std::make_shared<SplineTransfer>( &m_pdfs[ m_pdfs.size() -1 ], nBins , min, max ) ;
            thisSpline = paramMap.find( splineName );
          }
          auto it = otherParameters.find( param.first );
          unsigned int index = stoi((*tokens.rbegin()));

          if ( it != otherParameters.end() ){  
            DEBUG(" -> to " << (*it).second <<"    " <<  param.first << "    " << (*it).second->mean() ); 
            thisSpline->second->set( index , (*it).second );
            if( index == 0 ) 
              thisSpline->second->setAddress( param.second.first );
          }

          else if( *(tokens.rbegin()+1) == "C"  ){ 
            if( index == 0 ) thisSpline->second->setCurveAddress( param.second.first );
          }
          else 
            ERROR( param.first << " spline parameter not understood");

        }
        for( auto& spline : paramMap ){
          if( spline.second->isConfigured() ){
            m_addressMapping.push_back(spline.second);
            (*m_addressMapping.rbegin())->transfer();
          }
          else {
            ERROR("Spline not configured correctly!");
            m_stateIsGood = false;
            return; 
          }
        }
      }
      std::pair<AmpGen::MinuitParameter*,AmpGen::MinuitParameter*> parameters = p.second;
      if (parameters.first == 0 || parameters.second == 0 ){
        ERROR("Amplitude " << name 
            << " not properly configured : pointers = (" 
            << parameters.first << " , " 
            << parameters.second << ") , prefix = " << prefix );
        m_stateIsGood = false;
        return; 
      }
      m_coefficients.push_back( 
          std::complex<double>( parameters.first->mean() , parameters.second->mean() ) );
      m_minuitparameters.push_back( parameters );
    }
    DEBUG("Configured all parameters");
    if( m_pdfs.size() == 0 ) 
      WARNING("No expressions found for amplitude with prefix = " << m_prefix ); 
    else 
      INFO("PDF has " << m_pdfs.size() << " amplitudes" ) ; 

    m_normalisations.resize( m_pdfs.size(), std::vector<std::complex<double>>( m_pdfs.size() ) );
  }

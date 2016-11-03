#ifndef RESOLVE_PARAMETERS_H
#define RESOLVE_PARAMETERS_H

#include "AmpGen/ParticleProperties.h"
#include "AmpGen/ParticlePropertiesList.h"
#include "AmpGen/MinuitParameter.h"
#include "AmpGen/CompiledExpression.h"
#include "AmpGen/Utilities.h"
#include "AmpGen/CacheTransfer.h"

/// Takes a pdf object, and a set of parameters. 
/// the pdf's "external" parameters are mapped to default values
/// taken for example from the PDG - or it takes them from the list 
/// of user specified parameters (otherParameters) 
/// It returns a vector of "cacheTransfer" objectes
/// These specify a set of behaviour's that transform MinuitParameters 
/// into the arrays used in evaluating the function. 
/// TO DO : Add flexibility in defining cacheTransfer objects based on common rule sets,
/// Then have a factory object that manages the transfer behaviours. 
/// This will allow straightforward extensions by the user to other "weird" sorts of PDFs 
/// that require specialised behaviour. 
namespace AmpGen{ 
  template <class T>
    std::vector< std::shared_ptr<CacheTransfer<T>>>  
    resolveParameters( AmpGen::CompiledExpression<T>* ppdf,
        const std::map<std::string,AmpGen::MinuitParameter*>& otherParameters  ){
      using namespace AmpGen;
      AmpGen::CompiledExpression<T>& pdf = *ppdf;
      std::map<std::string,
        std::pair< unsigned int, double> > parameterNames = pdf.getAddressMapping();
      std::map<std::string,
        std::pair< unsigned int, double> > splineParameters;

      std::vector< std::shared_ptr<CacheTransfer<T> > > addressMapping;

      for( auto& param : parameterNames ){ /// all parameters for this PDF, including fixed ones ///  
        if( param.first.find("Spline") != std::string::npos ){
          splineParameters[param.first] = param.second;
          continue;
        }

        DEBUG( "Mapping parameter " << param.first << "  " << param.second.first ); 
        //       for( auto& param : otherParameters ) 
        auto it = otherParameters.find(param.first ); 

        if( it != otherParameters.end() ){
          DEBUG("Setting value of " << param.first << " from options file = " << (*it).second->mean() );
          addressMapping.push_back( 
              std::make_shared<CacheTransfer<T>>( 
                (*it).second , /// MinuitParameter*
                &pdf , //// CompiledExpression*
                param.second.first //// destination address 
                ) ) ; 
          (*addressMapping.rbegin())->transfer();
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
        std::map< std::string, std::shared_ptr<SplineTransfer<T>> > paramMap;
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
              std::make_shared<SplineTransfer<T>>( &pdf, nBins , min, max ) ;
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
            addressMapping.push_back(spline.second);
            (*addressMapping.rbegin())->transfer();
          }
          else {
            ERROR("Spline not configured correctly!");
            return {};
          }
        }
      }
      return addressMapping; 
    }
}
#endif
